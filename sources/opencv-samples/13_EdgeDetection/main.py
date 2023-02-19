import cv2 as cv
import numpy as np
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
	
	path = cv.samples.findFile(args.img_path)
	
	if not path:
		sys.exit(f"Image {args.img_path} not found")
	
	image = cv.imread(path, cv.IMREAD_UNCHANGED)

	if image is None:
		sys.exit(f"Could not read the image {path}")

	window1 = "Image"
	window2 = "Result"

	# Convertir en niveaux de gris
	gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)

	# cv.imshow(window1, image)

	t = getTime()

	# Appliquer un seuillage adaptatif pour réduire le bruit
	threshold = cv.adaptiveThreshold(gray, 255, cv.ADAPTIVE_THRESH_GAUSSIAN_C, cv.THRESH_BINARY, 11, 2)
	
	# Appliquer la détection de contour
	contours, hierarchy = cv.findContours(threshold, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
	
	# Créer un masque pour l'ombre
	shadow_mask = cv.drawContours(np.zeros(image.shape[:2], dtype="uint8"), contours, -1, 255, 0)
	
	# Inverser le masque pour remplacer l'ombre par des pixels de l'image d'origine
	image_no_shadow = cv.bitwise_or(image, image, mask=cv.bitwise_not(shadow_mask))
	
	print(f"duration: {getTimeDelta(t)} ms")

	cv.imshow(window2, image_no_shadow)

	cv.waitKey(0)
	cv.destroyAllWindows()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Edge Detection main.py CLI")
    parser.add_argument("img_path", type=str, help="image path to open")
    main()
