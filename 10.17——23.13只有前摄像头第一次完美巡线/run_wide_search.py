#!/usr/bin/env python3
"""Preview by default; --run starts the authorized 300-second controller."""
import argparse
import os
import signal
import subprocess
import sys
import time
from pathlib import Path

base = Path(__file__).resolve().parent

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--run',action='store_true',help='Start CH10 control, maximum 300 seconds')
    args=parser.parse_args()
    os.chdir(base)
    def stop(signum, frame):
        raise KeyboardInterrupt
    signal.signal(signal.SIGTERM, stop)
    signal.signal(signal.SIGHUP, stop)
    os.environ.setdefault('DISPLAY', ':0')
    os.environ.setdefault('XAUTHORITY', '/home/auv8/.Xauthority')
    lock=open(base/'route_launcher.lock','w')
    import fcntl
    fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
    (base/'wide_search_state.txt').write_text('0 -1 0 0\n')
    vision=subprocess.Popen([sys.executable,'-u','wide_search_vision.py'],start_new_session=True)
    control=None
    try:
        if not args.run:
            return vision.wait()
        deadline=time.monotonic()+10
        while True:
            if vision.poll() is not None:raise RuntimeError('Camera preview failed')
            try:
                ts,*_=map(float,(base/'wide_search_state.txt').read_text().split())
                if 0<=time.monotonic()-ts<.5:break
            except (ValueError,OSError):pass
            if time.monotonic()>deadline:raise RuntimeError('No fresh camera data')
            time.sleep(.05)
        name='wide_search_'+time.strftime('%Y%m%d_%H%M%S')
        with open(name+'.log','w') as log:
            control=subprocess.Popen(['./wide_search_control','--duration','300','--log',name+'.csv'],stdout=log,stderr=subprocess.STDOUT,start_new_session=True)
            print('Started 300-second route. Log:',name+'.log',flush=True)
            result=control.wait()
        print((base/(name+'.log')).read_text(),flush=True)
        return result
    except KeyboardInterrupt:
        return 130
    finally:
        if control is not None and control.poll() is None:
            control.send_signal(signal.SIGINT)
            control.wait()  # Allow stop and restore acknowledgements.
        if vision.poll() is None:
            vision.terminate()
            vision.wait()
        (base/'wide_search_state.txt').write_text('0 -1 0 0\n')
        lock.close()

if __name__=='__main__':sys.exit(main())
