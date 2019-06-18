#include "pch.h"
#include<opencv2/opencv.hpp>
#include<iostream>
#include "opencv2/core/core_c.h"
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int MAX_KERNEL_LENGTH = 31;
int DELAY_BLUR = 100;

//Pega maior valor de uma matriz
int maior(Mat img) {

	int maior = img.at<uchar>(0, 0);
	int valorIntensidade;


	for (int row = 0; row < img.rows; row++) {
		for (int col = 0; col < img.cols; col++) {
			valorIntensidade = (int)img.at<uchar>(row, col);
			if (valorIntensidade > maior) {
				maior = valorIntensidade;
			}
		}
	}
	return maior;

}

//Pega menor valor de uma matriz
int menor(Mat img) {

	int menor = img.at<uchar>(0, 0);
	int valorIntensidade;

	for (int row = 0; row < img.rows; row++) {
		for (int col = 0; col < img.cols; col++) {
			valorIntensidade = (int)img.at<uchar>(row, col);
			if (valorIntensidade < menor) {
				menor = valorIntensidade;
			}
		}
	}
	return menor;

}

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

//1° rgb para hsl e extrair a banda L
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

	/*namedWindow("hsl", WINDOW_NORMAL);
	imshow("hsl", hsl);*/
	imwrite("resultado/removal of OD/rgbForHSLCanalVerde.jpg", bgr[1]); //Salva a imagem

	return bgr[1];

}

//2° CLAHE
Mat clahe(Mat img) {

	img = imread("resultado/removal of OD/rgbForHSLCanalVerde.jpg", IMREAD_GRAYSCALE);
	//img.copyTo(resultado);

	Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(4);

	Mat resultado;

	clahe->apply(img, resultado);

	namedWindow("CLAHE - OD", WINDOW_NORMAL);
	imshow("CLAHE - OD", resultado);
	imwrite("resultado/removal of OD/CLAHE - OD.jpg", resultado); //Salva a imagem

	return resultado;
}

//3° Contrat streching 
Mat contrastStreching(Mat imgCLAHE) {
	Mat imgResultado;
	imgCLAHE.copyTo(imgResultado);

	int z, valorPixel, contraste;

	int maiorIntensidadeOriginal = maior(imgCLAHE);
	int menorIntensidadeOriginal = menor(imgCLAHE);
	float pixelSubMenor;
	float contrasteEpixelSub;

	maiorIntensidadeOriginal = maior(imgCLAHE);
	menorIntensidadeOriginal = menor(imgCLAHE);

	contraste = maiorIntensidadeOriginal - menorIntensidadeOriginal;

	for (int row = 0; row < imgCLAHE.rows; row++) {
		for (int col = 0; col < imgCLAHE.cols; col++) {
			valorPixel = (int)imgCLAHE.at<uchar>(row, col);

			pixelSubMenor = valorPixel - menorIntensidadeOriginal;
			contrasteEpixelSub = pixelSubMenor / contraste;

			z = 255 * contrasteEpixelSub;

			imgResultado.at<uchar>(row, col) = z;

		}
	}

	namedWindow("HistStretching.jpg", WINDOW_NORMAL);
	imshow("HistStretching.jpg", imgResultado);
	imwrite("resultado/removal of OD/Contrast streching/HistStretching.jpg", imgResultado); //Salva a imagem

	return imgResultado;
}

int display_dst(int delay){
	int c = waitKey(delay);
	if (c >= 0) { return -1; }
	return 0;
}

//4° Filtro da mediana
Mat medianFiltering(Mat imgContratStreching) {
	Mat resultMedianFiltering;

	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2){
		medianBlur(imgContratStreching, resultMedianFiltering, i);
		if (display_dst(DELAY_BLUR) != 0) { 
			break;
		}
	}

	namedWindow("Median Filtering - OD", WINDOW_NORMAL);
	imshow("Median Filtering - OD", resultMedianFiltering);
	imwrite("resultado/removal of OD/Median Filtering/Median Filtering - OD.jpg", resultMedianFiltering); //Salva a imagem

	return resultMedianFiltering;
	
}

//5° Binarização da imagem com otsu
Mat bynarizationOtsu(Mat imgMedianFiltering) {
	/*Mat resultBynarizationOtsu;
	cv::threshold(imgMedianFiltering, resultBynarizationOtsu, 0, 255, THRESH_BINARY | THRESH_OTSU);*/


	//Mat theFrame = imread("teste/otsu.jpg"); // opencv

	Mat resultGray, resultBynarizationOtsu;

	imgMedianFiltering.copyTo(resultGray);
	imgMedianFiltering.copyTo(resultBynarizationOtsu);
	//cvtColor(imgMedianFiltering, resultGray, cv::COLOR_RGB2GRAY);
	threshold(resultGray, resultBynarizationOtsu, 200, 255, THRESH_BINARY | THRESH_OTSU);
	//imwrite("result.jpg", binary);


	namedWindow("Bynarization Otsu - OD", WINDOW_NORMAL);
	imshow("Bynarization Otsu - OD", resultBynarizationOtsu);
	imwrite("resultado/removal of OD/Bynarization Otsu/Bynarization Otsu - OD.jpg", resultBynarizationOtsu); //Salva a imagem

	return resultBynarizationOtsu;
}

Mat deteopticalDiscDetection(Mat img) {

	//1° Converter para HSL e extrair banda L
	img = rgbForHSLAndLBand(img); 

	//2° Aplicar CLAHE
	img = clahe(img);  

	Mat new_image;
	img.copyTo(new_image);

	//3° Contrast Stretching
	new_image = contrastStreching(img);

	//4° Filtro da mediana
	new_image = medianFiltering(new_image);
	
	//5° Binarização da imagem com otsu
	new_image = bynarizationOtsu(new_image);

	//5° Radius enlargement

	return new_image;

}


void mostraAntes(Mat src) {
	namedWindow("Antes.jpg", WINDOW_AUTOSIZE);
	imshow("Antes.jpg", src);
}


int main(){
	
	Mat src = imread("IDRID/A. Segmentation/1. Original Images/a. Training Set/IDRiD_04.jpg");
	//Mat src = imread("resultado/removal of OD/Median Filtering/Median Filtering - OD.jpg", IMREAD_GRAYSCALE);
	//Mat src = imread("teste/usarcontrastenesse.jpg", IMREAD_GRAYSCALE);

	if (!src.data){
		cout << "Não foi possível abrir ou encontrar a imagem";
		return -1;
	}

	mostraAntes(src);

	deteopticalDiscDetection(src);

	//contrastStreching(src);

	

	waitKey(0);
	return 0;
}