# https://docs.opencv.org/4.x/db/deb/tutorial_display_image.html
# https://github.com/opencv/opencv/blob/4.x/samples/python/tutorial_code/introduction/display_image/display_image.py

import cv2 as cv
import sys
import argparse


def main():

    args = parser.parse_args()

    path = cv.samples.findFile(args.image_path)
    if not path:
        sys.exit(f"Image {args.image_path} not found")

    if not args.saved_image_path:
        sys.exit(f"Path {args.saved_image_path} is not valid")

    mode = cv.IMREAD_UNCHANGED

    if args.mode:
        mode_name = ""
        if args.mode == "u" or args.mode == "U":
            mode_name = "IMREAD_UNCHANGED"
        elif args.mode == "c" or args.mode == "C":
            mode_name = "IMREAD_COLOR"
            mode = cv.IMREAD_COLOR
        elif args.mode == "g" or args.mode == "G":
            mode_name = "IMREAD_GRAYSCALE"
            mode = cv.IMREAD_GRAYSCALE
        else:
            print("Unexpected mode value")
            print("Read the README.md")
            sys.exit()
        print(f"Read mode: {mode_name}")

    img = cv.imread(path, mode)

    if img is None:
        sys.exit("Could not read the image")

    cv.imshow("Read Image", img)
    cv.imwrite(args.saved_image_path, img)
    print(f"Image saved to {args.saved_image_path}")
    print("Press a key to quit")
    cv.waitKey(0)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Read and Write Image main.py CLI")
    parser.add_argument("image_path", type=str, help="path to image file")
    parser.add_argument("saved_image_path", type=str, help="path to image file to save")
    parser.add_argument("-m", "--mode", type=str, help="mode to read the image")
    main()
