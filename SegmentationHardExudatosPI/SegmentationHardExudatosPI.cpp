#include "pch.h"
#include<opencv2/opencv.hpp>
#include<iostream>
#include "opencv2/core/core_c.h"
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

//1° extração do canal verde: o canal verde contém informações suficientes para detectar exsudatos duros, pois tem o maior contraste entre os outros.
Mat greenChannelExtraction(Mat img) {

	//https://answers.opencv.org/question/59529/how-do-i-separate-the-channels-of-an-rgb-image-and-save-each-one-using-the-249-version-of-opencv/
	Mat bgr[3];   //destination array
	split(img, bgr);//fonte dividida  

	//Note: OpenCV uses BGR color order
	imwrite("green.png", bgr[1]); //green channel
	


	Mat green = imread("green.png", IMREAD_GRAYSCALE);
	
	//Abre o imagem mostrando apenas canal verde
	namedWindow("green", WINDOW_NORMAL);
	imshow("green", green);
	imwrite("resultado/Canal verde - exudatos.jpg", green); //Salva a imagem

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
	imwrite("resultado/Complemento - exudatos.jpg", img); //Salva a imagem


	return img;

}

//Seguimentar disco ópitico
//1° Conversão RGB para HSL;
//Mat rgbForHSL(Mat img) {
//
//	Mat hsl;
//
//	cvtColor(img, hsl, COLOR_RGB2HLS);  
//
//	Mat bgr[3];   //destination array
//	split(hsl, bgr);//fonte dividida  
//
//	imwrite("canalL.png", bgr[1]); //
//
//	Mat green = imread("green.png", IMREAD_GRAYSCALE);
//	////Abre o imagem mostrando apenas canal verde
//	//namedWindow("green", WINDOW_NORMAL);
//	//imshow("green", green);
//
//	namedWindow("canalL", WINDOW_NORMAL);
//	imshow("canalL", bgr[2]);
//	imwrite("resultado/rgbForHSLCanalVerde.jpg", bgr[2]); //Salva a imagem
//
//
//	//cv::Mat hslChannels[3];
//	//cv::split(hsl, hslChannels);
//
//	//Mat Lchannel = hsl[:, : , 1];
//	
//	return bgr[2];
//	
//	
//	
//	//cv::COLOR_RGB2GRAY
//	//cvtColor(image, gray, cv::COLOR_RGB2HLS); // cv::COLOR_RGB2GRAY
//
//	/*
//	cv::Mat src;
//	cv::Mat hsl;
//
//	cv::cvtColor(srcRgba, src, CV_RGBA2RGB);
//	cv::cvtColor(src, hsl, CV_RGB2HLS);
//
//	cv::Mat hslChannels[3];
//	cv::split(hsl, hslChannels);
//	*/
//}

Mat rgbForHSLAndLBand(Mat img) {

	Mat hsl;

	cvtColor(img, hsl, COLOR_RGB2HLS);

	Mat bgr[3];   //destination array
	split(hsl, bgr);//fonte dividida  

	imwrite("green.png", bgr[1]); //

	Mat green = imread("green.png", IMREAD_GRAYSCALE);
	//Abre o imagem mostrando apenas canal verde
	namedWindow("green", WINDOW_NORMAL);
	imshow("green", green);

	namedWindow("hsl", WINDOW_NORMAL);
	imshow("hsl", hsl);
	imwrite("resultado/removal of OD/rgbForHSLCanalVerde.jpg", bgr[1]); //Salva a imagem

	return hsl;

}

//2° CLARE
Mat clahe(Mat img) {

	img = imread("resultado/removal of OD/rgbForHSLCanalVerde.jpg", IMREAD_GRAYSCALE);
	//img.copyTo(resultado);

	Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(4);

	Mat resultado;

	clahe->apply(img, resultado);
	imshow("CLAHE - OD", resultado);
	imwrite("resultado/removal of OD/CLAHE - OD.jpg", resultado); //Salva a imagem

	return resultado;
}

//Contrat streching 
//https://www.programming-techniques.com/2013/01/contrast-stretching-using-c-and-opencv-image-processing.html
//https://theailearner.com/2019/01/30/contrast-stretching/
int computeOutput(int x, int r1, int s1, int r2, int s2)
{
	float result;
	if (0 <= x && x <= r1) {
		result = (float) s1 / r1 * x;
	}
	else if (r1 < x && x <= r2) {
		result = (float) ((s2 - s1) / (r2 - r1)) * (x - r1) + s1;
	}
	else if (r2 < x && x <= 255) {
		result = (float) ((255 - s2) / (255 - r2)) * (x - r2) + s2;
	}
	return (int) result;
}



Mat medianFiltering(Mat img) {
	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		medianBlur(src, dst, i);
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}
}

Mat deteopticalDiscDetection(Mat img) {
	img = rgbForHSLAndLBand(img); //1° Converter para HSL e extrair banda L
	img = clahe(img); //2° Aplicar CLAHE 

	Mat new_image;
	img.copyTo(new_image);

	//3° Contrast Stretching
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {

			int output = computeOutput(img.at<uchar>(y, x), 70, 0, 140, 255);
			new_image.at<uchar>(y, x) = saturate_cast<uchar>(output);

		}
	}


	namedWindow("Contrast Stretching", WINDOW_NORMAL);
	imshow("Contrast Stretching", new_image);
	imwrite("resultado/removal of OD/Contrast Stretching.jpg", new_image); //Salva a imagem

	
	//4° Filtro da mediana
	//5° Radius enlargement

	return new_image;

}

int main(){
	
	Mat src = imread("IDRID/A. Segmentation/1. Original Images/a. Training Set/IDRiD_02.jpg");

	if (!src.data){
		cout << "Não foi possível abrir ou encontrar a imagem";
		return -1;
	}

	deteopticalDiscDetection(src);

	/*src = greenChannelExtraction(src);
	src = complementOperation(src);*/


	

	waitKey(0);
	return 0;
}