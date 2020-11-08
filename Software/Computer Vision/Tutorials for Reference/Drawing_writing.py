import numpy as np
import cv2

img = cv2.imread("test_image.png", cv2.IMREAD_COLOR)

# Basic shapes
cv2.line(img, (0,0), (150, 150), (255, 255, 255), 15)
cv2.rectangle(img, (15, 25), (200, 150), (255, 255, 255), 2)
cv2.circle(img, (100, 150), 55, (255, 255, 0), -1)

# Polygons
points = np.array([[200, 400], [400, 450], [70, 500], [400, 70]], np.int32)
# points = points.reshape((-1, 1, 2))
cv2.polylines(img, [points], True, (0, 0, 255), 3)

# Text
font = cv2.FONT_HERSHEY_SIMPLEX
cv2.putText(img, "Testy Boi!", (600, 130), font, 1, (0, 0, 0), 1, cv2.LINE_AA)


cv2.imshow('image', img)
cv2.waitKey(0)
cv2.destroyAllWindows()