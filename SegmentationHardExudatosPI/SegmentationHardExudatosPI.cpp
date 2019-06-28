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
//https://www.programming-techniques.com/2013/01/contrast-stretching-using-c-and-opencv-image-processing.html
//https://theailearner.com/2019/01/30/contrast-stretching/
//https://theailearner.com/2019/01/30/contrast-stretching/
int computeOutput(int x, int r1, int s1, int r2, int s2)
{
	float result;
	if (0 <= x && x <= r1) {
		result = (float)s1 / r1 * x;
	}
	else if (r1 < x && x <= r2) {
		result = (float)((s2 - s1) / (r2 - r1)) * (x - r1) + s1;
	}
	else if (r2 < x && x <= 255) {
		result = (float)((255 - s2) / (255 - r2)) * (x - r2) + s2;
	}
	return (int)result;
}

//3° Contrast Stretching
Mat contrastStretching2(Mat imgCLAHE) {

	Mat new_image;
	imgCLAHE.copyTo(new_image);

	for (int y = 0; y < imgCLAHE.rows; y++) {
		for (int x = 0; x < imgCLAHE.cols; x++) {

			int output = computeOutput(imgCLAHE.at<uchar>(y, x), 70, 0, 140, 255);
			new_image.at<uchar>(y, x) = saturate_cast<uchar>(output);

		}
	}

	namedWindow("Contrast Stretching", WINDOW_NORMAL);
	imshow("Contrast Stretching", new_image);
	imwrite("resultado/removal of OD/Contrast Stretching.jpg", new_image); //Salva a imagem

	return new_image;
}



//3° Contrat streching 
Mat contrastStreching(Mat imgCLAHE) {
	Mat imgResultado;
	imgCLAHE.copyTo(imgResultado);

	int z, valorPixel, dif;

	int maiorIntensidadeOriginal = maior(imgCLAHE);
	int menorIntensidadeOriginal = menor(imgCLAHE);
	float pixelSubMenor;
	float contrasteEpixelSub;

	maiorIntensidadeOriginal = maior(imgCLAHE);
	menorIntensidadeOriginal = menor(imgCLAHE);

	dif = maiorIntensidadeOriginal - menorIntensidadeOriginal;

	for (int row = 0; row < imgCLAHE.rows; row++) {
		for (int col = 0; col < imgCLAHE.cols; col++) {
			valorPixel = (int)imgCLAHE.at<uchar>(row, col);

			pixelSubMenor = valorPixel - menorIntensidadeOriginal;
			contrasteEpixelSub = pixelSubMenor / dif;

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
	threshold(resultGray, resultBynarizationOtsu, 100, 255, THRESH_BINARY | THRESH_OTSU);
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
	new_image = contrastStretching2(img);

	//4° Filtro da mediana
	new_image = medianFiltering(new_image);
	
	//5° Binarização da imagem com otsu
	new_image = bynarizationOtsu(new_image);

	//5° Radius enlargement

	return new_image;

}


//Pega maior valor de uma matriz
int maior1(vector<vector<float>> mat, Mat imgA, Mat imgB) {

	int maior = mat[0][0];

	for (int row = 0; row < imgA.rows; row++) {
		for (int col = 0; col < imgA.cols; col++) {
			if (mat[row][col] > maior) {
				maior = mat[row][col];
			}
		}
	}
	return maior;

}

//Pega menor valor de uma matriz
int menor1(vector<vector<float>> mat, Mat imgA, Mat imgB) {

	int menor = mat[0][0];

	for (int row = 0; row < imgA.rows; row++) {
		for (int col = 0; col < imgA.cols; col++) {
			if (mat[row][col] < menor) {
				menor = mat[row][col];
			}
		}
	}
	return menor;
}

//Realizar a multiplicação para remover o disco ópitico da imagem
void multiplicacaoNormalizacao(Mat imgA, Mat imgB) {

	float y;
	int z;

	y = 350 - 45;
	z = (255 / y) * (300 - 45);

	Mat imgFinal01;
	imgA.copyTo(imgFinal01);
	int k;
	int maiorValor = 0, menorValor = 0;
	vector<vector<float>> mat(imgA.rows, vector<float>(imgA.cols));

	//Copia na matriz mat os valores da soma de imgA e imgB
	for (int row = 0; row < imgA.rows; row++) {
		for (int col = 0; col < imgA.cols; col++) {
			k = (int)imgA.at<uchar>(row, col) * (int)imgB.at<uchar>(row, col);
			if (k = 0)
				mat[row][col] = (int)imgA.at<uchar>(row, col);
			else
				mat[row][col] = k;

		}
	}

	maiorValor = maior1(mat, imgA, imgB);
	menorValor = menor1(mat, imgA, imgB);

	y = maiorValor - menorValor;
	//Faz a normalização
	for (int row = 0; row < imgA.rows; row++) {
		for (int col = 0; col < imgA.cols; col++) {
			z = (255 / y) * (mat[row][col] - menorValor); //Fómula para normalização

			imgFinal01.at<uchar>(row, col) = z;
		}
	}

	namedWindow("Normalizacao - multiplicacao", WINDOW_AUTOSIZE);
	imshow("Normalizacao - multiplicacao", imgFinal01);
	imwrite("resultado/Normalizacao - multiplicacao.jpg", imgFinal01); //Salva a imagem
}


void mostraAntes(Mat src) {
	namedWindow("Antes.jpg", WINDOW_AUTOSIZE);
	imshow("Antes.jpg", src);
}


int main(){
	
	//Mat src = imread("IDRID/A. Segmentation/1. Original Images/a. Training Set/IDRiD_47.jpg");
	//Mat src = imread("resultado/removal of OD/Median Filtering/Median Filtering - OD.jpg", IMREAD_GRAYSCALE);
	Mat src = imread("teste/originalBanco.jpg");


	Mat imgA = imread("teste/IDRiD_01.jpg", IMREAD_GRAYSCALE);
	Mat imgB = imread("teste/IDRiD_01_OD.tif", IMREAD_GRAYSCALE);


	/*namedWindow("a", WINDOW_NORMAL);
	imshow("a", imgA);


	namedWindow("b", WINDOW_NORMAL);
	imshow("b", imgB);*/


	if (!src.data){
		cout << "Não foi possível abrir ou encontrar a imagem";
		return -1;
	}

	//bynarizationOtsu(imgB);

	/*namedWindow("Bynarization Otsu - OD", WINDOW_NORMAL);
	imshow("Bynarization Otsu - OD", imgB);*/

	namedWindow("Original", WINDOW_NORMAL);
	imshow("original", src);

	//mostraAntes(src);

	deteopticalDiscDetection(src);

	//contrastStreching(src);

	//multiplicacaoNormalizacao(imgA, imgB);



	

	waitKey(0);
	return 0;
}