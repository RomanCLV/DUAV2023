import cv2 as cv


def main():
	cv.namedWindow("test");

	#print("\nwaitKey")
	#k = 0
	#while k != 27:
	#	k = cv.waitKey(0)
	#	print(k)

	#print("\nwaitKey & 0xFF")
	#k = 0
	#while k != 27:
	#	k = cv.waitKey(0) & 0xFF
	#	print(k)

	#print("\nwaitKeyEx")
	#k = 0
	#while k != 1048603:
	#	k = cv.waitKeyEx(0)
	#	print(k)

	print("\nwaitKeyEx & 0xFFFF")
	k = 0
	while k != 27:
		k = cv.waitKeyEx(0) & 0xFFFF
		print(k)

	cv.destroyAllWindows()


if __name__ == '__main__':
	main()