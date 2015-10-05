#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
using namespace cv;
using namespace std;

const int gamma_max = 9; const int alpha_max = 10; const int beta_max = 9;
int gamma_slider = 0; int alpha_slider = 0; int beta_slider = 0;
Mat initial_picture, gamma_picture, sigmoida_picture;
double new_intensive[256];
Mat grahp(256, 256, CV_8U, Scalar(255)); Mat gamma_graph; Mat sigmoida_graph;

void gamma_change(double gamma)
{
	gamma /= 5;
	double q;
	gamma_graph = grahp.clone();
	for (int i = 0; i < 256; i++) //calculation of the transition matrix of intensity
	{
		q = i; q /= 255;
		new_intensive[i] = pow(q, gamma) * 255;
	}

	for (int i = 0; i < gamma_graph.rows; i++) //display of transition graph
	{
		circle(gamma_graph, Point(i, 256 - new_intensive[i]), 0, Scalar(100), 2);
	}

	for (int i = 0; i < initial_picture.rows; i++) //change in intensity of the output image
		for (int j = 0; j < initial_picture.cols; j++)
		{
			gamma_picture.at<unsigned char>(i, j) = new_intensive[initial_picture.at<unsigned char>(i, j)];
		}

}
void on_trackbar_gamma(int, void*)
{
	gamma_change(gamma_slider + 1);
	imshow("Gamma", gamma_picture);
	imshow("Gamma correction", gamma_graph);
}

void sigmoida_change(double alpha, double beta)
{
	alpha *= 5;
	beta /= 10;
	double q;
	sigmoida_graph = grahp.clone();
	for (int i = 0; i < 256; i++) //calculation of the transition matrix of intensity
	{
		q = i; q /= 255;

		new_intensive[i] = 255 * (1 / (1 + exp(alpha*(beta - q))));

	}

	for (int i = 0; i < sigmoida_graph.rows; i++) //display of transition graph
	{
		circle(sigmoida_graph, Point(i, 256 - new_intensive[i]), 0, Scalar(100), 2);
	}

	imshow("Sigmoida correction", sigmoida_graph);

	double intensity;
	for (int i = 0; i < initial_picture.rows; i++)
		for (int j = 0; j < initial_picture.cols; j++)
		{
			sigmoida_picture.at<unsigned char>(i, j) = new_intensive[initial_picture.at<unsigned char>(i, j)];
		}
}
void on_trackbar_sigmoida(int, void*)
{
	sigmoida_change(alpha_slider + 1, beta_slider + 1);
	imshow("Sigmoida", sigmoida_picture);
}


void main(int argc, char* argv[])
{
	string initial_image_path = argv[1];
	string mode = argv[2];
	string path_for_save_image;
	string path_for_save_graph;

	initial_picture = imread(initial_image_path, CV_8UC1);

	if (mode == "-g")
	{
		if (argc == 3 || argc == 4)//visual mode
		{
			
			namedWindow("Initial picture", 1);
			namedWindow("Gamma correction", 1);
			imshow("Initial picture", initial_picture);

			gamma_picture = initial_picture.clone();
			if (argc == 4){ stringstream stream1; stream1.str(argv[3]); stream1 >> gamma_slider; }
			createTrackbar("Gamma Correction", "Gamma correction", &gamma_slider, gamma_max, on_trackbar_gamma);
			on_trackbar_gamma(gamma_slider, 0);
		}

		if (argc == 5 || argc == 6)
		{
			gamma_picture = initial_picture.clone();
			stringstream stream1; stream1.str(argv[3]); stream1 >> gamma_slider;
			gamma_change(gamma_slider);
			path_for_save_image = argv[4];
			imwrite(path_for_save_image, gamma_picture);
			if (argc == 6)
			{
				path_for_save_graph = argv[5];
				imwrite(path_for_save_graph, gamma_graph);
			}
		}

	}

	if (mode == "-s")
	{
		if (argc == 3 || argc == 5)//visual mode
		{
			namedWindow("Initial picture", 1);
			namedWindow("Sigmoida correction", 1);
			imshow("Initial picture", initial_picture);

			sigmoida_picture = initial_picture.clone();
			if (argc == 5)
			{
				stringstream stream1, stream2;
				stream1.str(argv[3]); stream2.str(argv[4]);
				stream1 >> alpha_slider; stream2 >> beta_slider;
			}
			createTrackbar("Alpha ", "Sigmoida correction", &alpha_slider, alpha_max, on_trackbar_sigmoida);
			createTrackbar("Beta ", "Sigmoida correction", &beta_slider, beta_max, on_trackbar_sigmoida);
			on_trackbar_sigmoida(alpha_slider, 0);
		}

		if (argc == 6 || argc == 7)
		{
			sigmoida_picture = initial_picture.clone();
			stringstream stream1, stream2;
			stream1.str(argv[3]); stream2.str(argv[4]);
			stream1 >> alpha_slider; stream2 >> beta_slider;

			sigmoida_change(alpha_slider, beta_slider);
			path_for_save_image = argv[5];
			imwrite(path_for_save_image, gamma_picture);
			if (argc == 7)
			{
				path_for_save_graph = argv[6];
				imwrite(path_for_save_graph, gamma_graph);
			}
		}
	}

	waitKey(0);
}