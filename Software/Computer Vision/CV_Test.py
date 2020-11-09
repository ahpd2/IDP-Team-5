
import numpy as np
import cv2

# Import the C optimised functions using cython
import pyximport
pyximport.install()
import c_optimised

def order_points(pts):
    '''
    initialzie a list of coordinates that will be ordered
    such that the first entry in the list is the top-left,
    the second entry is the top-right, the third is the
    bottom-right, and the fourth is the bottom-left.
    '''
    rectagle = np.zeros((4, 2), dtype="float32")
    # the top-left point will have the smallest sum, whereas
    # the bottom-right point will have the largest sum
    s = pts.sum(axis=1)
    rectagle[0] = pts[np.argmin(s)]
    rectagle[2] = pts[np.argmax(s)]
    # now, compute the difference between the points, the
    # top-right point will have the smallest difference,
    # whereas the bottom-left will have the largest difference
    diff = np.diff(pts, axis=1)
    rectagle[1] = pts[np.argmin(diff)]
    rectagle[3] = pts[np.argmax(diff)]
    # return the ordered coordinates
    return rectagle


def four_point_transform(image, pts, size=600):
    '''
    Apply a four point transformation on the image such that the four points make the four corners of a square image.
    '''
    # obtain a consistent order of the points and unpack them
    # individually
    rect = order_points(pts)
    (tl, tr, br, bl) = rect

    # get destination array
    dst = np.array([
        [0, 0],
        [size - 1, 0],
        [size - 1, size - 1],
        [0, size - 1]], dtype="float32")
    # compute the perspective transform matrix and then apply it
    M = cv2.getPerspectiveTransform(rect, dst)
    warped = cv2.warpPerspective(image, M, (size, size))
    # return the warped image
    return warped


def clean_perspective(src):
    '''
    Warp the image to remove barrel distortion and also align the test zone to be square.
    '''
    width = src.shape[1]
    height = src.shape[0]

    distCoeff = np.zeros((4, 1), np.float64)

    # distortion coefficients
    k1 = -6.7e-5  # negative to remove barrel distortion
    k2 = 0.0
    p1 = 0.0
    p2 = 0.0

    distCoeff[0, 0] = k1
    distCoeff[1, 0] = k2
    distCoeff[2, 0] = p1
    distCoeff[3, 0] = p2

    # assume unit matrix for camera
    cam = np.eye(3, dtype=np.float32)

    cam[0, 2] = width/2.0  # define center x
    cam[1, 2] = height/2.0  # define center y
    cam[0, 0] = 10.        # define focal length x
    cam[1, 1] = 10.        # define focal length y

    # calculate new camera so as to not cut off too much of image
    newcam, roi = cv2.getOptimalNewCameraMatrix(
        cam, distCoeff, (width, height), 0.21, (width, height))

    # here the undistortion will be computed
    dst = cv2.undistort(src, cam, distCoeff, newCameraMatrix=newcam)

    # warp the image to get table correctly positioned
    points = np.array([[188, 106], [771,  46], [858, 654], [229, 715]])
    warped = four_point_transform(dst, points, 600)
    return warped


def find_blobs(mask, min_size: int = 10):
    '''
    Returns a list of rectangles around detected blobs of a minimum size
    '''
    groups = c_optimised.find_groups(mask)

    # Discard groups with less than min_size pixels and find the rectangle that bounds the remaining groups
    rectangles = []
    for group in groups:
        if len(group) > min_size:

            minx = min(t[1] for t in group)
            maxx = max(t[1] for t in group)
            miny = min(t[0] for t in group)
            maxy = max(t[0] for t in group)

            corners = [(minx, miny), (maxx, maxy)]
            rectangles.append(corners)

    # sort rectangles in descending order by area
    rectangles.sort(reverse=True, key=lambda rect: (
        rect[1][1] - rect[0][1]) * (rect[1][0] - rect[0][0]))

    # print(len(rectangles))
    return rectangles


font = cv2.FONT_HERSHEY_SIMPLEX

cap = cv2.VideoCapture("http://localhost:8081/stream/video.mjpeg")


while True:

    ret, src = cap.read()  # get the current frame of video
    clean = clean_perspective(src)  # clean the perspective and align nicely
    # cv2.imshow("undistorted", clean)

    # convert to HSV so that colours are numerically more distant
    imageHSV = cv2.cvtColor(clean, cv2.COLOR_BGR2HSV)

    # =========Red area detection==============

    redROI = imageHSV[0:600, 0:250]
    clean_red_roi = clean[0:600, 0:250]
    # lower mask (0-10)
    lower_red = np.array([0, 50, 50])
    upper_red = np.array([15, 255, 255])
    mask0 = cv2.inRange(redROI, lower_red, upper_red)

    # upper mask (170-180)
    lower_red = np.array([165, 50, 50])
    upper_red = np.array([180, 255, 255])
    mask1 = cv2.inRange(redROI, lower_red, upper_red)

    # join masks
    red_mask = mask0+mask1

    # Clean mask from noise
    kernel = np.ones((3, 3), np.uint8)
    red_mask = cv2.erode(red_mask, kernel, iterations=1)

    # run blob detection
    rectangles = find_blobs(red_mask)

    # draw detected regions on image
    for rect in rectangles[:2]:
        cv2.rectangle(clean, rect[0], rect[1], (255, 255, 255), 1)
        cv2.putText(clean, "Red Target",
                    rect[0], font, 0.3, (0, 0, 255), 1, cv2.LINE_AA)
    for rect in rectangles[2:]:
        cv2.rectangle(clean, rect[0], rect[1], (255, 255, 255), 1)
        cv2.putText(clean, "Block", rect[0],
                    font, 0.3, (0, 0, 0), 1, cv2.LINE_AA)

    # =========Blue area detection==============

    blueROI = imageHSV[460:575, 525:550]

    # Threshold of blue in HSV space
    lower_blue = np.array([100, 0, 127])  # hsv(254, 24%, 94%)
    upper_blue = np.array([140, 255, 240])  # hsv(221, 96%, 49%)

    # preparing the mask to overlay
    blue_mask = cv2.inRange(blueROI, lower_blue, upper_blue)

    rectangles = find_blobs(blue_mask)

    # draw detected regions on image
    for rect in rectangles[:2]:
        cv2.rectangle(clean, (525 + rect[0][0], 460 + rect[0][1]),
                      (525 + rect[1][0], 460 + rect[1][1]), (255, 255, 255), 1)
        cv2.putText(clean, "Blue Target", (525 +
                                           rect[0][0], 460 + rect[0][1]), font, 0.3, (255, 0, 0), 1, cv2.LINE_AA)

    # ========Display on screen===================

    #cv2.imshow("source", src)
    cv2.imshow("image", clean)

    #cv2.imshow("red", red)

    # Quit if q is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()  # release video stream to avoid unclosed streams
cv2.destroyAllWindows()
