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


//Seguimentar disco ópitico
//1° Conversão RGB para HSL;
Mat rgbForHSL(Mat img) {

	Mat hsl;

	cvtColor(img, hsl, COLOR_RGB2HLS);  

	Mat bgr[3];   //destination array
	split(hsl, bgr);//fonte dividida  

	imwrite("canalL.png", bgr[1]); //

	//Mat green = imread("green.png", IMREAD_GRAYSCALE);
	////Abre o imagem mostrando apenas canal verde
	//namedWindow("green", WINDOW_NORMAL);
	//imshow("green", green);

	namedWindow("canalL", WINDOW_NORMAL);
	imshow("canalL", bgr[1]);


	//cv::Mat hslChannels[3];
	//cv::split(hsl, hslChannels);

	//Mat Lchannel = hsl[:, : , 1];
	
	return bgr[1];
	
	
	
	//cv::COLOR_RGB2GRAY
	//cvtColor(image, gray, cv::COLOR_RGB2HLS); // cv::COLOR_RGB2GRAY

	/*
	cv::Mat src;
	cv::Mat hsl;

	cv::cvtColor(srcRgba, src, CV_RGBA2RGB);
	cv::cvtColor(src, hsl, CV_RGB2HLS);

	cv::Mat hslChannels[3];
	cv::split(hsl, hslChannels);
	*/
}

//2° CLARE
//https://answers.opencv.org/question/12024/use-of-clahe/
Mat claheGO(Mat src, int _step)
{
	Mat CLAHE_GO = src.clone();
	int block = _step;//pblock
	int width = src.cols;
	int height = src.rows;
	int width_block = width / block; //每个小格子的长和宽
	int height_block = height / block;
	//存储各个直方图  
	int tmp2[8 * 8][256] = { 0 };
	float C2[8 * 8][256] = { 0.0 };
	//分块
	int total = width_block * height_block;
	for (int i = 0; i < block; i++)
	{
		for (int j = 0; j < block; j++)
		{
			int start_x = i * width_block;
			int end_x = start_x + width_block;
			int start_y = j * height_block;
			int end_y = start_y + height_block;
			int num = i + block * j;
			//遍历小块,计算直方图
			for (int ii = start_x; ii < end_x; ii++)
			{
				for (int jj = start_y; jj < end_y; jj++)
				{
					int index = src.at<uchar>(jj, ii);
					tmp2[num][index]++;
				}
			}
			//裁剪和增加操作，也就是clahe中的cl部分
			//这里的参数 对应《Gem》上面 fCliplimit  = 4  , uiNrBins  = 255
			int average = width_block * height_block / 255;
			//关于参数如何选择，需要进行讨论。不同的结果进行讨论
			//关于全局的时候，这里的这个cl如何算，需要进行讨论 
			int LIMIT = 40 * average;
			int steal = 0;
			for (int k = 0; k < 256; k++)
			{
				if (tmp2[num][k] > LIMIT) {
					steal += tmp2[num][k] - LIMIT;
					tmp2[num][k] = LIMIT;
				}
			}
			int bonus = steal / 256;
			//hand out the steals averagely  
			for (int k = 0; k < 256; k++)
			{
				tmp2[num][k] += bonus;
			}
			//计算累积分布直方图  
			for (int k = 0; k < 256; k++)
			{
				if (k == 0)
					C2[num][k] = 1.0f * tmp2[num][k] / total;
				else
					C2[num][k] = C2[num][k - 1] + 1.0f * tmp2[num][k] / total;
			}
		}
	}
	//计算变换后的像素值  
	//根据像素点的位置，选择不同的计算方法  
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			//four coners  
			if (i <= width_block / 2 && j <= height_block / 2)
			{
				int num = 0;
				CLAHE_GO.at<uchar>(j, i) = (int)(C2[num][CLAHE_GO.at<uchar>(j, i)] * 255);
			}
			else if (i <= width_block / 2 && j >= ((block - 1)*height_block + height_block / 2)) {
				int num = block * (block - 1);
				CLAHE_GO.at<uchar>(j, i) = (int)(C2[num][CLAHE_GO.at<uchar>(j, i)] * 255);
			}
			else if (i >= ((block - 1)*width_block + width_block / 2) && j <= height_block / 2) {
				int num = block - 1;
				CLAHE_GO.at<uchar>(j, i) = (int)(C2[num][CLAHE_GO.at<uchar>(j, i)] * 255);
			}
			else if (i >= ((block - 1)*width_block + width_block / 2) && j >= ((block - 1)*height_block + height_block / 2)) {
				int num = block * block - 1;
				CLAHE_GO.at<uchar>(j, i) = (int)(C2[num][CLAHE_GO.at<uchar>(j, i)] * 255);
			}
			//four edges except coners  
			else if (i <= width_block / 2)
			{
				//线性插值  
				int num_i = 0;
				int num_j = (j - height_block / 2) / height_block;
				int num1 = num_j * block + num_i;
				int num2 = num1 + block;
				float p = (j - (num_j*height_block + height_block / 2)) / (1.0f*height_block);
				float q = 1 - p;
				CLAHE_GO.at<uchar>(j, i) = (int)((q*C2[num1][CLAHE_GO.at<uchar>(j, i)] + p * C2[num2][CLAHE_GO.at<uchar>(j, i)]) * 255);
			}
			else if (i >= ((block - 1)*width_block + width_block / 2)) {
				//线性插值  
				int num_i = block - 1;
				int num_j = (j - height_block / 2) / height_block;
				int num1 = num_j * block + num_i;
				int num2 = num1 + block;
				float p = (j - (num_j*height_block + height_block / 2)) / (1.0f*height_block);
				float q = 1 - p;
				CLAHE_GO.at<uchar>(j, i) = (int)((q*C2[num1][CLAHE_GO.at<uchar>(j, i)] + p * C2[num2][CLAHE_GO.at<uchar>(j, i)]) * 255);
			}
			else if (j <= height_block / 2) {
				//线性插值  
				int num_i = (i - width_block / 2) / width_block;
				int num_j = 0;
				int num1 = num_j * block + num_i;
				int num2 = num1 + 1;
				float p = (i - (num_i*width_block + width_block / 2)) / (1.0f*width_block);
				float q = 1 - p;
				CLAHE_GO.at<uchar>(j, i) = (int)((q*C2[num1][CLAHE_GO.at<uchar>(j, i)] + p * C2[num2][CLAHE_GO.at<uchar>(j, i)]) * 255);
			}
			else if (j >= ((block - 1)*height_block + height_block / 2)) {
				//线性插值  
				int num_i = (i - width_block / 2) / width_block;
				int num_j = block - 1;
				int num1 = num_j * block + num_i;
				int num2 = num1 + 1;
				float p = (i - (num_i*width_block + width_block / 2)) / (1.0f*width_block);
				float q = 1 - p;
				CLAHE_GO.at<uchar>(j, i) = (int)((q*C2[num1][CLAHE_GO.at<uchar>(j, i)] + p * C2[num2][CLAHE_GO.at<uchar>(j, i)]) * 255);
			}
			//双线性插值
			else {
				int num_i = (i - width_block / 2) / width_block;
				int num_j = (j - height_block / 2) / height_block;
				int num1 = num_j * block + num_i;
				int num2 = num1 + 1;
				int num3 = num1 + block;
				int num4 = num2 + block;
				float u = (i - (num_i*width_block + width_block / 2)) / (1.0f*width_block);
				float v = (j - (num_j*height_block + height_block / 2)) / (1.0f*height_block);
				CLAHE_GO.at<uchar>(j, i) = (int)((u*v*C2[num4][CLAHE_GO.at<uchar>(j, i)] +
					(1 - v)*(1 - u)*C2[num1][CLAHE_GO.at<uchar>(j, i)] +
					u * (1 - v)*C2[num2][CLAHE_GO.at<uchar>(j, i)] +
					v * (1 - u)*C2[num3][CLAHE_GO.at<uchar>(j, i)]) * 255);
			}
			//smooth
			CLAHE_GO.at<uchar>(j, i) = CLAHE_GO.at<uchar>(j, i) + (CLAHE_GO.at<uchar>(j, i) << 8) + (CLAHE_GO.at<uchar>(j, i) << 16);
		}
	}
	return CLAHE_GO;
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


int main(){
	
	Mat src = imread("IDRID/A. Segmentation/1. Original Images/a. Training Set/IDRiD_02.jpg");

	if (!src.data){
		cout << "Não foi possível abrir ou encontrar a imagem";
		return -1;
	}

	namedWindow("original", WINDOW_NORMAL);
	imshow("original", src);

	//Seguimentar disco ópitico
	//1° Conversão RGB para HSL;

	//src = rgbForHSL(src);

	//src = claheGO(src, 1);


	//Teste mediana
	//https://docs.opencv.org/3.1.0/d4/d13/tutorial_py_filtering.html
	//https://docs.opencv.org/2.4/doc/tutorials/imgproc/gausian_median_blur_bilateral_filter/gausian_median_blur_bilateral_filter.html
	/*
	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		medianBlur(src, dst, i);
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}
	median = cv2.medianBlur(img, 5)*/

	//Teste CLAHE
	Mat m = imread("teste/hslbandl.jpg", IMREAD_GRAYSCALE); //input image
	Mat dst;
	m.copyTo(dst);
	imshow("hslbandl_original", m);

	Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(1);

	
	//Mat new_image = dst.clone();
	clahe->apply(m, dst);
	namedWindow("clare", WINDOW_NORMAL);
	imshow("clare", dst);

	//Fim teste CLAHE

	Mat new_image;
	m.copyTo(new_image);

	for (int y = 0; y < dst.rows; y++) {
		for (int x = 0; x < dst.cols; x++) {
			
				int output = computeOutput(dst.at<uchar>(y, x), 70, 0, 140, 255);
				new_image.at<uchar>(y, x) = saturate_cast<uchar>(output);
			
		}
	}


	//dst = histogramaDeStretching(dst);

	namedWindow("Stretching", WINDOW_NORMAL);
	imshow("Stretching", new_image);

	//src = greenChannelExtraction(src);
	
	//src = complementOperation(src);

	waitKey(0);
	return 0;
}