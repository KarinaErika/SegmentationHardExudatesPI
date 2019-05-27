// SegmentationHardExudatosPI.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h"
#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

//Abre o imagem mostrando apenas canal verde
Mat greenChannel(Mat img) {

	Mat bgr[3];   //destination array
	split(img, bgr);//fonte dividida  

	//Note: OpenCV uses BGR color order
	imwrite("green.png", bgr[1]); //green channel


	Mat green = imread("green.png");
	/*namedWindow("green", WINDOW_NORMAL);
	imshow("green", green);*/

	return green;
}

int main()
{
	Mat src = imread("teste/IDRiD_01.jpg");
	
	if (!src.data){
		cout << "Não foi possível abrir ou encontrar a imagem";
		return -1;
	}

	
	//Abre o imagem mostrando apenas canal verde
	namedWindow("green", WINDOW_NORMAL);
	imshow("green", greenChannel(src));
	
	waitKey(0);
	return 0;
}