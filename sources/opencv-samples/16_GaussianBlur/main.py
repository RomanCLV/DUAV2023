import cv2 as cv
import sys
import argparse
from time import time


def getTime():
	return time()


def getTimeDelta(t):
	t = 1000 * (getTime() - t)
	return round(1000 * t) / 1000


def main():
	args = parser.parse_args()
	
	

	cv.waitKey(0)
	cv.destroyAllWindows()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="GaussianBlur main.py CLI")
    parser.add_argument("img_path", type=str, help="image path to open")
    main()


import cv2 as cv
import numpy as np
import sys
import signal
import argparse
from time import time


global has_to_break


def sigint_handler(signal, frame):
    global has_to_break
    has_to_break = True


def getTime():
	return time()


def getTimeDelta(t):
	t = 1000 * (getTime() - t)
	return round(1000 * t) / 1000


def main():
	global has_to_break
	has_to_break = False

	args = parser.parse_args()
	
	path = cv.samples.findFile(args.img_path)
	
	if not path:
		sys.exit(f"Image {args.img_path} not found")
	
	image = cv.imread(path)

	if image is None:
		sys.exit(f"Could not read the image {path}")


	print("")
	print("press Left Arrow to decrease the blur size value")
	print("press Rigth Arrow to increase the blur size value\n")
	print("press ESC to quit\n")

	window2 = "Blur"
	window1 = "Image"

	k = 0

	# Pour la binarisation de l'image
	blur_size = 5

	t = 0.0
	has_to_compute = True
 	
	cv.imshow(window1, image)

	while True:
		if has_to_compute:
			has_to_compute = False
			print("")
			print(f"blur size : {blur_size}")
			t = getTime()

			blurred = cv.GaussianBlur(image, (blur_size, blur_size), 0)
			print(f"duration: {getTimeDelta(t)} ms")

			cv.imshow(window2, blurred)

		k = cv.waitKey(0) & 0xFF

		if has_to_break or k == 27:
			break

		elif k == 81:  # Left Arrow
			if blur_size > 1:
				blur_size -= 2
				has_to_compute = True

		elif k == 83:  # Right Arrow
			if blur_size < 101:
				blur_size += 2
				has_to_compute = True


if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Gaussian Blur main.py CLI")
    parser.add_argument("img_path", type=str, help="image path to open")
    main()
