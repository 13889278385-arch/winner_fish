"""50%-100% height horizontal fit + original binary-bottom following. Q exits."""
import cv2
import numpy as np
import time
import signal
from pathlib import Path

from route_recording import Recorder

BASE = Path(__file__).resolve().parent

def publish(stamp, x, horizontal, present=False):
    tmp = BASE / 'wide_search_state.tmp'
    tmp.write_text('%.9f %.6f %d %d\n' % (stamp, x, horizontal, present))
    tmp.replace(BASE / 'wide_search_state.txt')

def bottom(mask):
    n, labels, stats, _ = cv2.connectedComponentsWithStats(mask)
    ids = [i for i in range(1, n) if stats[i, cv2.CC_STAT_AREA] >= 40]
    if not ids:
        return None
    i = max(ids, key=lambda j: stats[j, cv2.CC_STAT_AREA])
    ys, xs = np.where(labels == i)
    y = int(ys.max())
    return float(np.median(xs[ys >= y - 5])), y

def upper_line(mask):
    y0, y1 = mask.shape[0] // 2, mask.shape[0]
    upper = mask[y0:y1]
    # A short horizontal opening separates crossbars from vertical stems.
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (21, 1))
    selected = cv2.morphologyEx(upper, cv2.MORPH_OPEN, kernel)
    contours, _ = cv2.findContours(selected, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    candidates = []
    for c in contours:
        if cv2.contourArea(c) < 40:
            continue
        vx, vy, x, y = cv2.fitLine(c, cv2.DIST_HUBER, 0, .01, .01).ravel()
        angle = float(np.degrees(np.arctan2(vy, vx)))
        angle = (angle + 90) % 180 - 90
        points = c[:, 0, :]
        span = int(points[:, 0].max() - points[:, 0].min())
        short, long = sorted(cv2.minAreaRect(c)[1])
        if abs(angle) <= 15 and span >= .30 * mask.shape[1] and long / max(short, 1) >= 3:
            c = c.copy()
            c[:, 0, 1] += y0
            candidates.append((span, c, (vx, vy, x, y + y0), angle))
    return max(candidates, key=lambda c: c[0]) if candidates else None

def any_line(mask):
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    for c in contours:
        if cv2.contourArea(c)<40:continue
        short,long=sorted(cv2.minAreaRect(c)[1])
        if long>=.2*mask.shape[0] and long/max(short,1)>=3:
            return True
    return False

def main():
    def stop(signum, frame):
        raise KeyboardInterrupt
    signal.signal(signal.SIGTERM, stop)
    signal.signal(signal.SIGHUP, stop)
    publish(0, -1, False)
    cap = cv2.VideoCapture('/dev/v4l/by-id/usb-H264_USB_Camera_H264_USB_Camera_2020032801-video-index0', cv2.CAP_V4L2)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
    win = 'Swing search: Y50-100% + bottom | mask - Q exits'
    recorder = Recorder(BASE)
    try:
        if not cap.isOpened():
            raise RuntimeError('Camera unavailable')
        cv2.namedWindow(win, cv2.WINDOW_NORMAL)
        cv2.resizeWindow(win, 1000, 600)
        cv2.createTrackbar('S max', win, 60, 255, lambda _: None)
        cv2.createTrackbar('V min', win, 220, 255, lambda _: None)
        while True:
            # Timestamp before acquisition: a blocked read does not look fresh.
            stamp = time.monotonic()
            ok, frame = cap.read()
            if not ok:
                raise RuntimeError('Camera read failed')
            roi = frame[:, int(frame.shape[1] * .2):].copy()
            mask = cv2.inRange(cv2.cvtColor(roi, cv2.COLOR_BGR2HSV),
                               (0, 0, cv2.getTrackbarPos('V min', win)),
                               (179, cv2.getTrackbarPos('S max', win), 255))
            point = bottom(mask)
            x = point[0] / roi.shape[1] if point else -1
            upper = upper_line(mask)
            publish(stamp, x, upper is not None, upper is not None or any_line(mask))
            view = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)
            for image in (roi, view):
                h, w = image.shape[:2]
                for by in (h//2, h-1):
                    cv2.line(image, (0, by), (w-1, by), (0,255,255), 1)
                for ratio in (.4,.6):
                    px=int(w*ratio);cv2.line(image,(px,0),(px,h-1),(255,0,0),1)
                if point:
                    cv2.circle(image,(int(point[0]),point[1]),6,(0,0,255),2)
                if upper:
                    _, c, (vx,vy,cx,cy), angle=upper
                    p1=(int(cx-vx*w*2),int(cy-vy*w*2))
                    p2=(int(cx+vx*w*2),int(cy+vy*w*2))
                    valid,p1,p2=cv2.clipLine((0,h//2,w,h-h//2),p1,p2)
                    if valid:cv2.line(image,p1,p2,(0,255,0),2)
            status='PREVIEW ONLY'
            stage,pwm,elapsed=0,0,0
            try:
                ts,stage,pwm,elapsed=map(float,(BASE/'wide_search_status.txt').read_text().split())
                if 0<=time.monotonic()-ts<.5:status='Stage %d PWM %d %.1fs'%(stage,pwm,elapsed)
                else:stage,pwm,elapsed=0,0,0
            except (OSError,ValueError):pass
            cv2.putText(roi,status,(8,22),cv2.FONT_HERSHEY_SIMPLEX,.5,(0,255,255),1)
            cv2.putText(roi,'Bottom %.1f%% Horizontal %s'%(x*100,upper is not None),(8,44),cv2.FONT_HERSHEY_SIMPLEX,.45,(0,255,255),1)
            combined=np.hstack((roi,view))
            recorder.save(combined,stamp,int(stage),int(pwm),elapsed,x,upper is not None,upper[3] if upper else '')
            cv2.imshow(win,combined)
            if cv2.waitKey(1)&255 in (27,ord('q')):break
    finally:
        publish(0,-1,False)
        recorder.close()
        cap.release()
        cv2.destroyAllWindows()

if __name__=='__main__':main()
