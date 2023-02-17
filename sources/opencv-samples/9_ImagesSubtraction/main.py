# https://docs.opencv.org/4.x/db/deb/tutorial_display_image.html

import cv2 as cv
import argparse
from time import time


def main():

    args = parser.parse_args()

    path1 = cv.samples.findFile(args.img1_path)
    path2 = cv.samples.findFile(args.img2_path)

    if not path1:
        sys.exit(f"Image {args.img1_path} not found")
    if not path2:
        sys.exit(f"Image {args.img2_path} not found")

    mode = cv.IMREAD_UNCHANGED
    if args.grayscale:
        mode = cv.IMREAD_GRAYSCALE
    img1 = cv.imread(path1, mode)
    img2 = cv.imread(path2, mode)

    if img1 is None:
        exit(f"Could not read image {path1}")
    if img2 is None:
        exit(f"Could not read image {path2}")

    if img1.shape != img2.shape or img1.dtype != img2.dtype:
        exit(f"Not the same dimensions or data type!\timage 1: {img1.shape} ({img1.dtype})\timage 2: {img2.shape} ({img2.dtype})")

    delay = time()
    subtracted = cv.subtract(img1, img2)
    delay = 1000 * (time() - delay)
    print(f"Subtraction delay: { round(delay * 1000) / 1000} ms")

    cv.imshow("Image 1", img1)
    cv.imshow("Image 2", img2)
    cv.imshow("Subtract", subtracted)
    print("Press a key to quit")
    cv.waitKey(0)
    cv.destroyAllWindows()


if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description="Images Subtraction main.py CLI")
    parser.add_argument("img1_path", type=str, help="image 1 path to open")
    parser.add_argument("img2_path", type=str, help="image 2 path to open")
    parser.add_argument("-g", "--grayscale", action="store_true", help="read the images with GRAYSCALE")

    main()
