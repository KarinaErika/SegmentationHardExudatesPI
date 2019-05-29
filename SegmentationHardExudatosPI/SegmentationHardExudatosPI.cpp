#include "pch.h"
#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

//1° extração do canal verde: o canal verde contém informações suficientes para detectar exsudatos duros, pois tem o maior contraste entre os outros.
Mat greenChannelExtraction(Mat img) {

	//https://answers.opencv.org/question/59529/how-do-i-separate-the-channels-of-an-rgb-image-and-save-each-one-using-the-249-version-of-opencv/
	Mat bgr[3];   //destination array
	split(img, bgr);//fonte dividida  

	//Note: OpenCV uses BGR color order
	imwrite("green.png", bgr[1]); //green channel
	imwrite("green2.png", bgr[1]); //green channel


	Mat green = imread("green.png", IMREAD_GRAYSCALE);
	
	//Abre o imagem mostrando apenas canal verde
	namedWindow("green", WINDOW_NORMAL);
	imshow("green", green);

	return green;
	
}

//2° Aplicação da operação de complemento, pois áreas claras tornam-se mais escuras, enquanto as áreas escuras ficam mais claras na imagem de saída.
Mat complementOperation(Mat img) {

	int i, j, k;

	for (i = 0; i < img.rows; i++) {
		for (j = 0; j < img.cols; j++) {
			k = img.at<uchar>(i, j);
			k = 255 - k;
			img.at<uchar>(i, j) = k;
		}
	}
	namedWindow("Complemento", WINDOW_NORMAL);
	imshow("Complemento", img);

	return img;

}

int main(){
	
	Mat src = imread("IDRID/A. Segmentation/1. Original Images/a. Training Set/IDRiD_02.jpg");

	if (!src.data){
		cout << "Não foi possível abrir ou encontrar a imagem";
		return -1;
	}

	namedWindow("original", WINDOW_NORMAL);
	imshow("original", src);

	src = greenChannelExtraction(src);
	
	src = complementOperation(src);

	waitKey(0);
	return 0;
}