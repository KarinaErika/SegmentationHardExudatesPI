// SegmentationHardExudatosPI.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h"
#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;


void greenChannel(Mat img) {

}

int main()
{
	Mat src = imread("teste/IDRiD_01.jpg");
	
	if (!src.data){
		cout << "Não foi possível abrir ou encontrar a imagem";
		return -1;
	}
	


		

		Mat bgr[3];   //destination array
		split(src, bgr);//fonte dividida  

		//Note: OpenCV uses BGR color order
		imwrite("blue.png", bgr[0]); //blue channel
		imwrite("green.png", bgr[1]); //green channel
		imwrite("red.png", bgr[2]); //red channel



		Mat blue = imread("blue.png");
		namedWindow("blue", WINDOW_NORMAL);
		imshow("blue", blue);



		//imshow("blue.png", bgr[0]);
		Mat green = imread("green.png");
		namedWindow("green", WINDOW_NORMAL);
		imshow("green",green);
		//imshow("red.png", bgr[2]);

		
		Mat red = imread("red.png");
		namedWindow("red", WINDOW_NORMAL);
		imshow("red", red);

		
	
	waitKey(0);
	return 0;
}