# https://docs.opencv.org/4.x/db/deb/tutorial_display_image.html

import cv2 as cv
import sys
import argparse


def main():

    args = parser.parse_args()

    path = cv.samples.findFile(args.img_path)
    if not path:
        sys.exit(f"Image {args.img_path} not found")
    img = cv.imread(path, cv.IMREAD_UNCHANGED)

    if img is None:
        sys.exit("Could not read the image")

    cv.imshow("Read Image", img)
    print("Press a key to quit")
    cv.waitKey(0)
    cv.destroyAllWindows();


if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description="Read Image main.py CLI")
    parser.add_argument("img_path", type=str, help="image path to open")
    main()
