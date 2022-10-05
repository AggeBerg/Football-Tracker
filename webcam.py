import cv2
import numpy as np

vc = cv2.VideoCapture(2)
vc.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
vc.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)
fourcc = cv2.VideoWriter_fourcc(*"MJPG")
vc.set(cv2.CAP_PROP_FOURCC, fourcc)
vc.set(cv2.CAP_PROP_FPS, 30)
def test(v):
    pass
    
cv2.namedWindow("result", cv2.WINDOW_NORMAL)
cv2.createTrackbar("min_h", "result", 138, 255, test)
cv2.createTrackbar("min_s", "result", 123, 255, test)
cv2.createTrackbar("min_v", "result", 147, 255, test)


cv2.createTrackbar("max_h", "result", 175, 255, test)
cv2.createTrackbar("max_s", "result", 255, 255, test)
cv2.createTrackbar("max_v", "result", 255, 255, test)

if vc.isOpened(): # try to get the first frame
    rval, frame = vc.read()
else:
    rval = False

while rval:
    min_h = cv2.getTrackbarPos("min_h", "result")
    min_s = cv2.getTrackbarPos("min_s", "result")
    min_v = cv2.getTrackbarPos("min_v", "result")
    max_h = cv2.getTrackbarPos("max_h", "result")
    max_s = cv2.getTrackbarPos("max_s", "result")
    max_v = cv2.getTrackbarPos("max_v", "result")
    frame_to_tresh = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    thresh = cv2.inRange(frame_to_tresh, (min_h, min_s, min_v), (max_h, max_s, max_v))
    mask = cv2.erode(thresh, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=2)
    left = np.vstack((cv2.cvtColor(mask, cv2.COLOR_GRAY2RGB), frame))
    all = np.hstack((cv2.resize(left, (0, 0), fx=0.5, fy=0.5), cv2.bitwise_and(frame, frame, mask=mask)))
    cv2.imshow("result", all)
    rval, frame = vc.read()
    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

vc.release()
cv2.destroyWindow("result")