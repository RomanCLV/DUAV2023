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
	print("press Left Arrow to decrease the threshold value")
	print("press Rigth Arrow to increase the threshold value\n")
	print("press ESC to quit\n")

	k = 0

	window1 = "Image"
	window2 = "Result"

	# Pour la binarisation de l'image
	threshold_value = 100   # seuil de binarisation

	t = 0.0
	has_to_compute = True

	# Convertir en niveaux de gris
	gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)

	cv.imshow(window1, image)

	while True:

		if has_to_compute:
			has_to_compute = False

			print("")
			print(f"threshold : {threshold_value}")

			t = getTime()

			_, binary = cv.threshold(gray, threshold_value, 255, cv.THRESH_BINARY)

			# Inverser l'image binaire pour obtenir les régions de l'ombre
			binary_inverse = cv.bitwise_not(binary)

			# Remplacer les pixels de l'ombre par des pixels de l'image d'origine
			image_no_shadow = cv.bitwise_or(image, image, mask=binary_inverse)

			print(f"duration: {getTimeDelta(t)} ms")

			cv.imshow(window2, image_no_shadow)

		k = cv.waitKey(0) & 0xFF

		if has_to_break or k == 27:
			break

		elif k == 81:  # Left Arrow
			if threshold_value > 0:
				threshold_value -= 1
				has_to_compute = True

		elif k == 83:  # Right Arrow
			if threshold_value < 255:
				threshold_value += 1
				has_to_compute = True

	cv.waitKey(0)
	cv.destroyAllWindows()

if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Image Segmentation main.py CLI")
    parser.add_argument("img_path", type=str, help="image path to open")
    main()
