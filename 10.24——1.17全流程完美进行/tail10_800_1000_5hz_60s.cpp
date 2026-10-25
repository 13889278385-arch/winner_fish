#include <mavlink/v2.0/ardupilotmega/mavlink.h>
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <poll.h>
#include <sstream>
#include <set>
#include <stdexcept>
#include <string>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// Only SERVO10 is addressed. Never arm, change mode, or send RC overrides.
static const char *DEFAULT_PORT = "/dev/serial/by-id/usb-ArduPilot_Pixhawk6X-bdshot_20001F001551333234343538-if00";
static volatile sig_atomic_t interrupted = 0;
static void on_signal(int) { interrupted = 1; }
static double seconds() {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

struct Link {
    int fd = -1;
    termios previous{};
    bool saved = false;
    uint8_t system = 0, component = 0;
    bool armed = true;
    double heartbeat = 0, output_time = 0;
    int output = -1;
    unsigned output_seq = 0; unsigned rejected=0; double ack_time=0;
    std::map<std::string, float> params;
    std::map<std::string, unsigned> param_seq;
    std::map<int, std::pair<unsigned, int>> acks;
    std::set<std::string> pending_notices, shown_notices;
    mavlink_message_t message{};
    mavlink_status_t parser{};

    explicit Link(const std::string &port) {
        fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd < 0) throw std::runtime_error("Cannot open serial port: " + std::string(strerror(errno)));
        if (flock(fd, LOCK_EX | LOCK_NB) < 0 || ioctl(fd, TIOCEXCL) < 0 || tcgetattr(fd, &previous) < 0) {
            close(fd); fd = -1; throw std::runtime_error("Cannot exclusively lock/configure serial port");
        }
        saved = true;
        termios config = previous;
        cfmakeraw(&config);
        cfsetispeed(&config, B115200); cfsetospeed(&config, B115200);
        config.c_cflag |= CLOCAL | CREAD;
        config.c_cflag &= ~(CRTSCTS | HUPCL);
        if (tcsetattr(fd, TCSANOW, &config) < 0) {
            ioctl(fd, TIOCNXCL); close(fd); fd = -1;
            throw std::runtime_error("Cannot configure serial port");
        }
    }
    ~Link() {
        if (fd >= 0) {
            if (saved) tcsetattr(fd, TCSANOW, &previous);
            ioctl(fd, TIOCNXCL); close(fd);
        }
    }
    void send(mavlink_message_t &msg) {
        uint8_t data[MAVLINK_MAX_PACKET_LEN];
        int len = mavlink_msg_to_send_buffer(data, &msg), done = 0;
        double deadline = seconds() + 0.02;
        while (done < len && seconds() < deadline) {
            int n = write(fd, data + done, len - done);
            if (n > 0) done += n;
            else if (errno != EAGAIN && errno != EINTR) throw std::runtime_error("Serial write failed");
            else { pollfd p{fd, POLLOUT, 0}; poll(&p, 1, 1); }
        }
        if (done != len) throw std::runtime_error("Serial write timeout");
    }
    void pump(int milliseconds = 50) {
        pollfd p{fd, POLLIN, 0};
        int ready = poll(&p, 1, milliseconds);
        if (ready < 0 && errno != EINTR) throw std::runtime_error("Serial poll failed");
        if (p.revents & (POLLERR | POLLHUP | POLLNVAL)) throw std::runtime_error("Serial disconnected");
        if (!(p.revents & POLLIN)) return;
        uint8_t bytes[4096]; int n = read(fd, bytes, sizeof(bytes));
        if (n < 0 && errno != EAGAIN && errno != EINTR) throw std::runtime_error("Serial read failed");
        for (int i = 0; i < n; ++i) {
            if (!mavlink_parse_char(MAVLINK_COMM_0, bytes[i], &message, &parser)) continue;
            if (!system && message.msgid == MAVLINK_MSG_ID_HEARTBEAT && message.compid == 1) {
                mavlink_heartbeat_t h{}; mavlink_msg_heartbeat_decode(&message, &h);
                if (h.autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA && h.type == MAV_TYPE_SUBMARINE) {
                    system = message.sysid; component = message.compid;
                }
            }
            if (!system || message.sysid != system || message.compid != component) continue;
            if (message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
                mavlink_heartbeat_t h{}; mavlink_msg_heartbeat_decode(&message, &h);
                heartbeat = seconds(); armed = (h.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) != 0;
            } else if (message.msgid == MAVLINK_MSG_ID_PARAM_VALUE) {
                mavlink_param_value_t v{}; mavlink_msg_param_value_decode(&message, &v);
                std::string key(v.param_id, strnlen(v.param_id, 16));
                params[key] = v.param_value; ++param_seq[key];
            } else if (message.msgid == MAVLINK_MSG_ID_COMMAND_ACK) {
                mavlink_command_ack_t v{}; mavlink_msg_command_ack_decode(&message, &v);
                ++acks[v.command].first; acks[v.command].second = v.result; if(v.command==MAV_CMD_DO_SET_SERVO){ack_time=seconds(); if(v.result!=MAV_RESULT_ACCEPTED)++rejected;}
            } else if (message.msgid == MAVLINK_MSG_ID_SERVO_OUTPUT_RAW) {
                mavlink_servo_output_raw_t v{}; mavlink_msg_servo_output_raw_decode(&message, &v);
                if (v.port == 0) { output = v.servo10_raw; output_time = seconds(); ++output_seq; }
            } else if (message.msgid == MAVLINK_MSG_ID_STATUSTEXT) {
                mavlink_statustext_t v{}; mavlink_msg_statustext_decode(&message, &v);
                std::string notice(v.text, strnlen(v.text, 50));
                if (!shown_notices.count(notice) && pending_notices.size() < 64)
                    pending_notices.insert(notice);
            }
        }
    }
    void print_notices() {
        // Only call between input lines, never while the user is typing.
        for (const auto &notice : pending_notices) {
            std::cout << "FC: " << notice << '\n';
            if (shown_notices.size() < 256) shown_notices.insert(notice);
        }
        pending_notices.clear();
    }
    void require_idle() {
        if (interrupted) throw std::runtime_error("Interrupted");
        if (!system || seconds() - heartbeat > 2.5)
            throw std::runtime_error("Heartbeat timeout (system=" + std::to_string(system) + ", parse_errors=" + std::to_string(parser.parse_error) + ")");
        if (armed) throw std::runtime_error("Flight controller is armed; test stopped");
    }
    void command(int cmd, float p1, float p2 = 0) {
        mavlink_message_t m{};
        mavlink_msg_command_long_pack(245, 190, &m, system, component, cmd, 0, p1, p2, 0, 0, 0, 0, 0);
        send(m);
    }
    float parameter(const std::string &name) {
        char id[16]{};
        name.copy(id, sizeof(id));
        for (int attempt = 0; attempt < 3; ++attempt) {
            unsigned seq = param_seq[name];
            mavlink_message_t m{};
            mavlink_msg_param_request_read_pack(245, 190, &m, system, component, id, -1);
            send(m);
            double end = seconds() + 1;
            while (seconds() < end) { pump(); if (param_seq[name] > seq) return params[name]; }
        }
        throw std::runtime_error("No parameter reply: " + name);
    }
    void function(int value) {
        for (int attempt = 0; attempt < 3; ++attempt) {
            unsigned seq = param_seq["SERVO10_FUNCTION"];
            mavlink_message_t m{};
            mavlink_msg_param_set_pack(245, 190, &m, system, component, "SERVO10_FUNCTION", value, MAV_PARAM_TYPE_INT16);
            send(m);
            double end = seconds() + 1;
            while (seconds() < end) {
                pump();
                if (param_seq["SERVO10_FUNCTION"] > seq && params["SERVO10_FUNCTION"] == value) return;
            }
        }
        throw std::runtime_error("SERVO10_FUNCTION change was not confirmed");
    }
    void pwm(int value) {
        unsigned seq = acks[MAV_CMD_DO_SET_SERVO].first;
        command(MAV_CMD_DO_SET_SERVO, 10, value);
        double end = seconds() + 1.5;
        while (seconds() < end) {
            pump();
            if (acks[MAV_CMD_DO_SET_SERVO].first > seq) {
                if (acks[MAV_CMD_DO_SET_SERVO].second != MAV_RESULT_ACCEPTED)
                    throw std::runtime_error("SET_SERVO rejected, result=" + std::to_string(acks[MAV_CMD_DO_SET_SERVO].second));
                return;
            }
        }
        throw std::runtime_error("SET_SERVO acknowledgement timeout");
    }
    bool verify_output(int expected, double timeout = 1.5) {
        unsigned seq = output_seq;
        command(MAV_CMD_REQUEST_MESSAGE, MAVLINK_MSG_ID_SERVO_OUTPUT_RAW);
        double end = seconds() + timeout;
        while (seconds() < end) { pump(); if (output_seq > seq && output == expected) return true; }
        return false;
    }
};




int main(){signal(SIGINT,on_signal);signal(SIGTERM,on_signal);signal(SIGHUP,on_signal);try{Link l(DEFAULT_PORT);double end=seconds()+10;while(!l.system&&seconds()<end)l.pump();l.require_idle();int f=l.parameter("SERVO10_FUNCTION");if(f!=0)throw std::runtime_error("CH10 function changed");int rc=0;try{l.pwm(900);double start=seconds(),next=start;unsigned n=0;std::cout<<"START CH10 800..1000 5Hz 60s"<<std::endl;while(seconds()-start<60){l.pump(1);l.require_idle();double now=seconds();if(now>=next){if(now-next>.1)throw std::runtime_error("Schedule stalled");int pwm=std::lround(900+100*std::sin(2*std::acos(-1.)*5*(now-start)));l.command(MAV_CMD_DO_SET_SERVO,10,pwm);++n;next+=.02;}if(l.rejected)throw std::runtime_error("Command rejected");if(now-start>1&&now-l.ack_time>.5)throw std::runtime_error("ACK stalled");}std::cout<<"COMPLETE seconds="<<seconds()-start<<" commands="<<n<<std::endl;}catch(const std::exception&e){std::cerr<<e.what()<<std::endl;rc=1;}l.pwm(0);bool off=l.verify_output(0);std::cout<<"STOP_OUTPUT_CONFIRMED="<<off<<std::endl;return off?rc:1;}catch(const std::exception&e){std::cerr<<e.what()<<std::endl;return 1;}}
