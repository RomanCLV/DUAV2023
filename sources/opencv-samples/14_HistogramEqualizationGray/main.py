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
	
	path = cv.samples.findFile(args.img_path)
	
	if not path:
		sys.exit(f"Image {args.img_path} not found")
	
	image = cv.imread(path)

	if image is None:
		sys.exit(f"Could not read the image {path}")

	window1 = "Image gray"
	window2 = "Equalized"

	# Convertir en niveaux de gris
	gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)

	t = getTime()

	# Appliquer l'égalisation d'histogramme
	equalized = cv.equalizeHist(gray)

	print(f"duration: {getTimeDelta(t)} ms")

	cv.imshow(window1, gray)
	cv.imshow(window2, equalized)

	cv.waitKey(0)
	cv.destroyAllWindows()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Histogram Equalization Gray main.py CLI")
    parser.add_argument("img_path", type=str, help="image path to open")
    main()
