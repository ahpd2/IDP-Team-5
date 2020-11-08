import numpy as np
import cv2

img = cv2.imread("test_image.png", cv2.IMREAD_COLOR)

# specific pixel colour value
px = img[55, 55] # can be set like this also
print(px)

# ROI = Region Of Image
# img[100:150, 100:150] = [255, 255, 255]

region = img[200:250, 500:600]
img[500:550, 700:800] = region

cv2.rectangle(img, (700, 500), (800, 550), (255, 255, 255), 1)

cv2.imshow("image", img)
cv2.waitKey(0)
cv2.destroyAllWindows()