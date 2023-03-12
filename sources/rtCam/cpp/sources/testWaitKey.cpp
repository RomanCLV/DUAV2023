#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	int k;
	namedWindow("test");

	cout << endl << "waitKey" << endl;
	k = 0;
	while (k != 27)
	{
		k = waitKey(0);
		cout << k << endl;
	}

	cout << endl << "waitKey & 0xFF" << endl;
	k = 0;
	while (k != 27)
	{
		k = waitKey(0) & 0xFF;
		cout << k << endl;
	}

	cout << endl << "waitKeyEx" << endl;
	k = 0;
	while (k != 1048603)
	{
		k = waitKeyEx(0);
		cout << k << endl;
	}

	cout << endl << "waitKeyEx & 0xFFFF" << endl;
	k = 0;
	while (k != 27)
	{
		k = waitKeyEx(0) & 0xFFFF;
		cout << k << endl;
	}

	destroyAllWindows();
	return 0;
}