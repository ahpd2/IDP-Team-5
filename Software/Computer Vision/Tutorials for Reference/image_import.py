import cv2
import numpy as np
import matplotlib.pyplot as plt

img = cv2.imread('test_image.png', cv2.IMREAD_GRAYSCALE)
# Other Options
# IMREAD_GRAYSCALE : 0
# IMREAD_COLOR : 1
# IMREAD_UNCHANGED : -1


# Plot image with openCV
# cv2.imshow("title", img)
# cv2.waitKey(0)
# cv2.destroyAllWindows()

# Plot image with matplotlib
# plt.imshow(img, cmap="gray", interpolation='bicubic')
# plt.plot([50, 100],[80, 100], "c", linewidth="5")
# plt.show()

# To save an image:
# cv2.imwrite("output.png", img)

