#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
using namespace cv;
using namespace std;

string norma_type;
const int median_max = 9; 
int median_slider = 0; int center = 2; int radius;

Mat initial_picture, Filter_picture;

vector<Mat> in_picture_channels(3); vector<Mat> vec_picture_channels(3);

vector<vector<int>> supportRegion;

void on_trackbar_median(int, void*)
{
	radius = 2 * (median_slider + 1) - 1;
	medianBlur(initial_picture, Filter_picture, radius);
	imshow("Scalar Median Filter", Filter_picture);
	//if(median_slider==5) imwrite("D:\\picture.jpg", Median_Filter_picture);
	waitKey(0);
}

void getSupportRegion(int x, int y)
{
	// fill 'supportRegion' for current mask position
	for (int i = 0; i < radius; i++) // pass through the pixels around (x, y) and initialize the vector array
	{
		int xi = x + i - center;
		for (int j = 0; j < radius; j++) 
		{
				int yj = y + j - center;
				vector<int> temp;
				for (int ii = 0; ii < 3; ++ii) temp.push_back(in_picture_channels[ii].at<unsigned char>(xi, yj));
				supportRegion.push_back(temp);
		}		
	}	
}

double evclid_distance(int p[3], int n) // euclidean distance
{
	double sum = 0.0;
	for (int i = 0; i < 3; i++) 
	{
		double d = p[i] - supportRegion[n][i];
		sum = sum + pow(d, 2);;
	}
	return sqrt(sum);
}

double manhattan_distance(int p[3], int n) // Manhattan distance
{
	double sum = 0.0;
	for (int i = 0; i < 3; i++)
	{
		double d = p[i] - supportRegion[n][i];
		sum = sum + abs(d);
	}
	return sum;
}

double aggregateDistance(int p[3]) 
{
	double d = 0; int n = 0;

	for (int i = 0; i < pow(radius,2); i++) 
	{
		if (norma_type == "-ed") d = d + evclid_distance(p, n);
		if (norma_type == "-md")d = d + manhattan_distance(p, n);
		n++;
	}
	return d;
}

void filterPixel(int x, int y)
{
	int pCtr[3]; //central pixel
	for (int i = 0; i < 3; i++) pCtr[i] = in_picture_channels[i].at<unsigned char>(x, y);

	supportRegion.clear();

	getSupportRegion(x, y);
	double dCtr = aggregateDistance(pCtr);
	double dMin = DBL_MAX;

	int jMin = -1;

	for (int j = 0; j < pow(radius,2); j++) 
	{
		int p[3];
		for (int i = 0; i < 3; i++) p[i] = supportRegion[j][i];
		double d = aggregateDistance(p);
		if (d < dMin)
		{
			jMin = j;
			dMin = d;
		}
	}

	int pmin[3]; 
	for (int i = 0; i < 3; i++) pmin[i] = supportRegion[jMin][i];

	float pF[3];	
	if (dMin < dCtr)
	{			for (int i = 0; i < 3; i++) pF[i] = pmin[i];	}
	else { for (int i = 0; i < 3; i++) pF[i] = pCtr[i]; }

	for (int i = 0; i < 3; i++) vec_picture_channels[i].at<unsigned char>(x,y) = pF[i];
}

void VectorMedianFilter()
{
	split(initial_picture, in_picture_channels);
	split(initial_picture, vec_picture_channels);

	radius = 2 * center + 1;
	for (double x = center; x < initial_picture.rows - center; x++)
	{
		for (int y = center; y < initial_picture.cols - center; y++)
		{
 			filterPixel(x,y);
		}
		cout << round(100*x/initial_picture.rows) << "%\n";
	}	
}

int main(int argc, char* argv[])
{
	string initial_image_path = argv[1];
	string mode = argv[2];
	string path_for_save_image;

	initial_picture = imread(initial_image_path);

	if (mode == "-s")
	{
		if (argc == 3 || argc == 4)//visual mode
		{
			namedWindow("Initial picture", 1);
			namedWindow("Scalar Median Filter", 1);
			imshow("Initial picture", initial_picture);

			//Filter_picture = initial_picture.clone();
			if (argc == 4){ stringstream stream1; stream1.str(argv[3]); stream1 >> center; }
			median_slider = center;
			createTrackbar("Scalar Median Filter", "Scalar Median Filter", &median_slider, median_max, on_trackbar_median);
			on_trackbar_median(median_slider, 0);

		}

		if (argc == 5)
		{
			//Filter_picture = initial_picture.clone();
			stringstream stream1; stream1.str(argv[3]); stream1 >> center;
			radius = 2 * (center + 1) - 1;
			
			medianBlur(initial_picture, Filter_picture, radius);

			path_for_save_image = argv[4];
			imwrite(path_for_save_image, Filter_picture);
		}
	}

	if (mode == "-v")
	{

		if (argc == 5)//visual mode
		{
			namedWindow("Initial picture", 1);
			//namedWindow("Scalar Median Filter", 1);
			imshow("Initial picture", initial_picture);
			norma_type = argv[4];

			VectorMedianFilter();

			//namedWindow("Median Filter", 1);
			//cout << picture_channels[0].at<unsigned char>(1, 1);
			//cout << in_picture_channels[0].at<unsigned char>(1, 1)<<"\n";

			merge(vec_picture_channels, Filter_picture);
			imshow("vector", Filter_picture);
			waitKey(0);
		}

		if (argc == 6)
		{
			namedWindow("Initial picture", 1);
			//namedWindow("Scalar Median Filter", 1);
			imshow("Initial picture", initial_picture);
			norma_type = argv[4];

			VectorMedianFilter();

			//namedWindow("Median Filter", 1);
			//cout << picture_channels[0].at<unsigned char>(1, 1);
			//cout << in_picture_channels[0].at<unsigned char>(1, 1)<<"\n";

			merge(vec_picture_channels, Filter_picture);
			imshow("vector", Filter_picture);

			path_for_save_image = argv[5];
			imwrite(path_for_save_image, Filter_picture);
			waitKey(0);
		}
	}

	waitKey(0);
}