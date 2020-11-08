import numpy as np
import cv2

def order_points(pts):
	# initialzie a list of coordinates that will be ordered
	# such that the first entry in the list is the top-left,
	# the second entry is the top-right, the third is the
	# bottom-right, and the fourth is the bottom-left
	rect = np.zeros((4, 2), dtype = "float32")
	# the top-left point will have the smallest sum, whereas
	# the bottom-right point will have the largest sum
	s = pts.sum(axis = 1)
	rect[0] = pts[np.argmin(s)]
	rect[2] = pts[np.argmax(s)]
	# now, compute the difference between the points, the
	# top-right point will have the smallest difference,
	# whereas the bottom-left will have the largest difference
	diff = np.diff(pts, axis = 1)
	rect[1] = pts[np.argmin(diff)]
	rect[3] = pts[np.argmax(diff)]
	# return the ordered coordinates
	return rect

def four_point_transform(image, pts, size):
	# obtain a consistent order of the points and unpack them
	# individually
	rect = order_points(pts)
	(tl, tr, br, bl) = rect

    # get destination array
	dst = np.array([
		[0, 0],
		[size - 1, 0],
		[size - 1, size - 1],
		[0, size - 1]], dtype = "float32")
	# compute the perspective transform matrix and then apply it
	M = cv2.getPerspectiveTransform(rect, dst)
	warped = cv2.warpPerspective(image, M, (size, size))
	# return the warped image
	return warped

def clean_perspective(src):
    width  = src.shape[1]
    height = src.shape[0]

    distCoeff = np.zeros((4,1),np.float64)

    # TODO: add your coefficients here!
    k1 = -6.7e-5; # negative to remove barrel distortion
    k2 = 0.0
    p1 = 0.0
    p2 = 0.0

    distCoeff[0,0] = k1
    distCoeff[1,0] = k2
    distCoeff[2,0] = p1
    distCoeff[3,0] = p2

    # assume unit matrix for camera
    cam = np.eye(3,dtype=np.float32)



    cam[0,2] = width/2.0  # define center x
    cam[1,2] = height/2.0 # define center y
    cam[0,0] = 10.        # define focal length x
    cam[1,1] = 10.        # define focal length y

    # calculate new camera so as to not cut off too much of image
    newcam, roi = cv2.getOptimalNewCameraMatrix(cam, distCoeff, (width, height), 0.21, (width, height))

    # here the undistortion will be computed
    dst = cv2.undistort(src,cam,distCoeff, newCameraMatrix = newcam)



    # warp the image to get table correctly positioned
    points = np.array([[188, 106], [771,  46], [858, 654], [229, 715]])
    warped = four_point_transform(dst, points, 600)
    return warped


def find_cubes(mask : np.array):
    ylen, xlen = mask.shape
    groups = np.zeros((ylen, xlen), dtype=np.uint8)
    groupIndex = [[], ]
    for y in range(ylen):
        for x in range(xlen):
            if mask[y, x] == 255:
            # this pixel is white

                if x > 0 and y > 0:
                # not at the borders

                    if groups[y-1, x] != 0:
                    # pixel above has a group
                        # add this pixel to that group
                        groups[y, x] = groups[y-1, x]
                        groupIndex[groups[y, x]].append([y, x])

                        if groups[y, x-1] != groups[y-1, x] and groups[y, x-1] != 0:
                        #pixel to the left has a different group
                            newgroup = groups[y-1, x]
                            oldgroup = groups[y, x-1]
                            #merge the two groups
                            for pixel in groupIndex[oldgroup]:
                                groups[pixel[0], pixel[1]] = newgroup

                            groupIndex[newgroup] += groupIndex[oldgroup]
                            groupIndex[oldgroup] = []

                    elif groups[y, x-1] != 0:
                    #pixel to left has a group and above doesn't
                        # add this pixel to the group
                        groups[y, x] = groups[y, x-1]
                    
                    elif groups[y, x] == 0:
                        groups[y, x] = len(groupIndex)
                        groupIndex.append([[y, x]])

    rectangles = []
    for group in groupIndex:
        if len(group) > 10:

            minx = min(group, key = lambda t: t[1])[1]
            maxx = max(group, key = lambda t: t[1])[1]
            miny = min(group, key = lambda t: t[0])[0]
            maxy = max(group, key = lambda t: t[0])[0]

            corners = [(minx, miny), (maxx, maxy)]
            rectangles.append(corners)
    
    rectangles.sort(reverse = True, key = lambda rect: (rect[1][1] - rect[0][1]) * (rect[1][0] - rect[0][0]))

    print(len(rectangles))
    return rectangles

font = cv2.FONT_HERSHEY_SIMPLEX


cap = cv2.VideoCapture("http://localhost:8081/stream/video.mjpeg")



while True:

    ret, src = cap.read()
    clean = clean_perspective(src)

    imageHSV = cv2.cvtColor(clean, cv2.COLOR_BGR2HSV)


    

    # Red area detection

    redROI = imageHSV[0:600,0:250]
    clean_red_roi = clean[0:600,0:250]
    # lower mask (0-10)
    lower_red = np.array([0,50,50])
    upper_red = np.array([15,255,255])
    mask0 = cv2.inRange(redROI, lower_red, upper_red)

    # upper mask (170-180)
    lower_red = np.array([165,50,50])
    upper_red = np.array([180,255,255])
    mask1 = cv2.inRange(redROI, lower_red, upper_red)

    # join masks
    red_mask = mask0+mask1

    # Clean mask from noise
    kernel = np.ones((3, 3), np.uint8)
    red_mask = cv2.erode(red_mask, kernel, iterations = 1)
    
    #run blob detection
    rectangles = find_cubes(red_mask)

    #draw detected regions on image
    for rect in rectangles[:2]:
        cv2.rectangle(clean, rect[0], rect[1], (255, 255, 255), 1)
        cv2.putText(clean, "Red Target", rect[0], font, 0.3, (0, 0, 255), 1, cv2.LINE_AA)
    for rect in rectangles[2:]:
        cv2.rectangle(clean, rect[0], rect[1], (255, 255, 255), 1)
        cv2.putText(clean, "Block", rect[0], font, 0.3, (0, 0, 0), 1, cv2.LINE_AA)

    # Blue area detection

    blueROI = imageHSV[460:575, 525:550]

    # Threshold of blue in HSV space 
    lower_blue = np.array([100, 64, 127]) #hsv(254, 24%, 94%)
    upper_blue = np.array([140, 245, 240]) #hsv(221, 96%, 49%)
  
    # preparing the mask to overlay 
    blue_mask = cv2.inRange(blueROI, lower_blue, upper_blue)

    rectangles = find_cubes(blue_mask)

    #draw detected regions on image
    for rect in rectangles[:2]:
        cv2.rectangle(clean, (525 + rect[0][0], 460 + rect[0][1]), (525 + rect[1][0], 460 + rect[1][1]), (255, 255, 255), 1)
        cv2.putText(clean, "Blue Target", (525 + rect[0][0], 460 + rect[0][1]), font, 0.3, (255, 0, 0), 1, cv2.LINE_AA)



    #cv2.imshow("source", src)
    cv2.imshow("image", clean)


    
    #cv2.imshow("blue", blue)

    #cv2.imshow("red mask", red_mask)
    #cv2.imshow("red", red)



    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()