import numpy as np
import cv2

img = cv2.imread("test_image.png")
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
red = img[:,:,2]
retval, threshold = cv2.threshold(red, 120, 255, cv2.THRESH_BINARY)

cv2.imshow("original", img)
cv2.imshow("red", red)
cv2.imshow("threshold", threshold)

cv2.waitKey(0)
cv2.destroyAllWindows()
