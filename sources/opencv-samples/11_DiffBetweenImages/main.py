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
	
	path1 = cv.samples.findFile(args.img_path_1)
	path2 = cv.samples.findFile(args.img_path_2)
	
	if not path1:
		sys.exit(f"Image {args.img_path_1} not found")
	if not path2:
		sys.exit(f"Image {args.img_path_2} not found")
	
	image1 = cv.imread(path1, cv.IMREAD_UNCHANGED)
	image2 = cv.imread(path2, cv.IMREAD_UNCHANGED)
	
	if image1 is None:
		sys.exit(f"Could not read the image {path1}")
	if image2 is None:
		sys.exit(f"Could not read the image {path1}")
	
	print("")
	print("press Left Arrow to decrease the threshold value")
	print("press Rigth Arrow to increase the threshold value\n")
	print("press Up Arrow to increase the kernel size")
	print("press Down Arrow to decrease the kernel size")
	print("press ESC to quit\n")

	k = 0

	window1 = "Image 1"
	window2 = "Image 2"
	window3 = "Différence"
	window4 = "Différence (ouverture morphologique)"

	gray_image1 = None
	gray_image2 = None
	diff_image = None

	# Pour la binarisation de l'image
	threshold_image = None
	threshold_value = 30   # seuil de binarisation

	# Pour l'ouverture morphologique
	opened_image = None
	kernel_size = 3
	kernel = np.ones((kernel_size, kernel_size), np.uint8)
	
	t = 0.0
	has_to_compute = True
	
	# Convertir les images en niveaux de gris
	t = getTime()
	gray_image1 = cv.cvtColor(image1, cv.COLOR_BGR2GRAY)
	print(f"conversion en gris image 1 : {getTimeDelta(t)} ms")
	
	t = getTime()
	gray_image2 = cv.cvtColor(image2, cv.COLOR_BGR2GRAY)
	print(f"conversion en gris image 2 : {getTimeDelta(t)} ms")
	
	t = getTime()
	# Calculer la différence absolue entre les deux images
	diff_image = cv.absdiff(gray_image1, gray_image2)
	print(f"différence absolue : {getTimeDelta(t)} ms")
 	
	cv.namedWindow(window1, cv.WINDOW_NORMAL)
	cv.namedWindow(window2, cv.WINDOW_NORMAL)
	cv.namedWindow(window3, cv.WINDOW_NORMAL)
	cv.namedWindow(window4, cv.WINDOW_NORMAL)
	cv.imshow(window1, image1)
	cv.imshow(window2, image2)

	while True:
		if has_to_compute:
			has_to_compute = False
			print("")
			print(f"threshold : {threshold_value}")
			print(f"kernel size : {kernel_size}")
			
			t = getTime()
			
        	# Appliquer un seuil pour obtenir une image binaire
			_, threshold_image = cv.threshold(diff_image, threshold_value, 255, cv.THRESH_BINARY)
        	
			print(f"threshold : {getTimeDelta(t)} ms")
			
			t = getTime()

        	# Appliquer une ouverture morphologique
			opened_image = cv.morphologyEx(threshold_image, cv.MORPH_OPEN, kernel)

			print(f"ouverture morphologique : {getTimeDelta(t)} ms")

			cv.imshow(window3, threshold_image)
			cv.imshow(window4, opened_image)

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

		elif k == 84:  # Down Arrow
			if kernel_size > 0:
				kernel_size -= 1
				kernel = np.ones((kernel_size, kernel_size), np.uint8)
				has_to_compute = True

		elif k == 82:  # Up Arrow
			if kernel_size < 255:
				kernel_size += 1
				kernel = np.ones((kernel_size, kernel_size), np.uint8)
				has_to_compute = True
	

if __name__ == '__main__':
    signal.signal(signal.SIGINT, sigint_handler)
    parser = argparse.ArgumentParser(description="Diff between Images main.py CLI")
    parser.add_argument("img_path_1", type=str, help="image 1 path to open")
    parser.add_argument("img_path_2", type=str, help="image 2 path to open")
    main()
