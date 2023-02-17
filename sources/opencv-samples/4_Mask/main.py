# https://docs.opencv.org/4.x/d7/d37/tutorial_mat_mask_operations.html
# https://raw.githubusercontent.com/opencv/opencv/4.x/samples/python/tutorial_code/core/mat_mask_operations/mat_mask_operations.py

from __future__ import print_function
import sys
import time
import argparse

import numpy as np
import cv2 as cv

## [basic_method]
def is_grayscale(my_image):
    return len(my_image.shape) < 3


def saturated(sum_value):
    if sum_value > 255:
        sum_value = 255
    if sum_value < 0:
        sum_value = 0

    return sum_value


def sharpen(my_image):
    if is_grayscale(my_image):
        height, width = my_image.shape
    else:
        my_image = cv.cvtColor(my_image, cv.CV_8U)
        height, width, n_channels = my_image.shape

    result = np.zeros(my_image.shape, my_image.dtype)
    ## [basic_method_loop]
    for j in range(1, height - 1):
        for i in range(1, width - 1):
            if is_grayscale(my_image):
                sum_value = 5 * my_image[j, i] - my_image[j + 1, i] - my_image[j - 1, i] \
                            - my_image[j, i + 1] - my_image[j, i - 1]
                result[j, i] = saturated(sum_value)
            else:
                for k in range(0, n_channels):
                    sum_value = 5 * my_image[j, i, k] - my_image[j + 1, i, k]  \
                                - my_image[j - 1, i, k] - my_image[j, i + 1, k]\
                                - my_image[j, i - 1, k]
                    result[j, i, k] = saturated(sum_value)
    ## [basic_method_loop]
    return result
## [basic_method]

def main():
	args = parser.parse_args()

	filename = args.image_path
	img_codec = cv.IMREAD_COLOR
	
	if args.grayscale:
		img_codec = cv.IMREAD_GRAYSCALE

	src = cv.imread(cv.samples.findFile(args.image_path), img_codec)
	if src is None:
		print(f"Can't open image {args.image_path}")
		return -1

	t = round(time.time())

	dst0 = sharpen(src)

	t = (time.time() - t)
	print("Hand written function time passed in seconds: %s" % t)

	t = time.time()
	## [kern]
	kernel = np.array([[0, -1, 0],
                       [-1, 5, -1],
                       [0, -1, 0]], np.float32)  # kernel should be floating point type
    ## [kern]
    ## [filter2D]
	dst1 = cv.filter2D(src, -1, kernel)
    # ddepth = -1, means destination image has depth same as input image
    ## [filter2D]

	t = (time.time() - t)
	print("Built-in filter2D time passed in seconds:     %s" % t)

	cv.namedWindow("Input", cv.WINDOW_AUTOSIZE)
	cv.namedWindow("Output 1", cv.WINDOW_AUTOSIZE)
	cv.namedWindow("Output 2", cv.WINDOW_AUTOSIZE)

	cv.imshow("Input", src)
	cv.imshow("Output 1", dst0)
	cv.imshow("Output 2", dst1)

	print("Press a key to quit")
	cv.waitKey(0)
	cv.destroyAllWindows()
	

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Mask main.py CLI")
	parser.add_argument("image_path", type=str, help="path to image file")
	parser.add_argument("-g", "--grayscale", action="store_true", help="read the image with GRAYSCALE")
	main()
