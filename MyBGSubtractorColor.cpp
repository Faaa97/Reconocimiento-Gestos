#include "MyBGSubtractorColor.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;



MyBGSubtractorColor::MyBGSubtractorColor(VideoCapture vc) {

	cap = vc;
	max_samples = MAX_HORIZ_SAMPLES * MAX_VERT_SAMPLES;
	
	lower_bounds = vector<Scalar>(max_samples);
	upper_bounds = vector<Scalar>(max_samples);
	means = vector<Scalar>(max_samples);
	
	h_low = 80;
    h_up = 100;
	l_low = 100;
	l_up = 21;
	s_low = 99;
	s_up = 100;
	
	namedWindow("Trackbars");

	createTrackbar("H low:", "Trackbars", &h_low, 100, &MyBGSubtractorColor::Trackbar_func);
	createTrackbar("H high:", "Trackbars", &h_up, 100, &MyBGSubtractorColor::Trackbar_func);
	createTrackbar("L low:", "Trackbars", &l_low, 100, &MyBGSubtractorColor::Trackbar_func);
	createTrackbar("L high:", "Trackbars", &l_up, 100, &MyBGSubtractorColor::Trackbar_func);
	createTrackbar("S low:", "Trackbars", &s_low, 100, &MyBGSubtractorColor::Trackbar_func);
	createTrackbar("S high:", "Trackbars", &s_up, 100, &MyBGSubtractorColor::Trackbar_func);


}

void MyBGSubtractorColor::Trackbar_func(int, void*)
{

}


void MyBGSubtractorColor::LearnModel() {


	


	Mat frame, tmp_frame, hls_frame;
	std::vector<cv::Point> samples_positions;

	cap >> frame;

	//almacenamos las posiciones de las esquinas de los cuadrados 
	Point p;
	for (int i = 0; i < MAX_HORIZ_SAMPLES; i++) {
		for (int j = 0; j < MAX_VERT_SAMPLES; j++) {
			p.x = frame.cols / 2 + (-MAX_HORIZ_SAMPLES / 2 + i)*(SAMPLE_SIZE + DISTANCE_BETWEEN_SAMPLES);
			p.y = frame.rows / 2 + (-MAX_VERT_SAMPLES / 2 + j)*(SAMPLE_SIZE + DISTANCE_BETWEEN_SAMPLES);
			samples_positions.push_back(p);
		}
	}

	namedWindow("Cubre los cuadrados con la mano y pulsa espacio");

	for (;;) {
		
		flip(frame, frame, 1); //Gira la imágen (efecto espejo).
		
		frame.copyTo(tmp_frame);   //copia la matriz a otra

		//dibujar los cuadrados
		
		for (int i = 0; i < max_samples; i++) {
			rectangle(tmp_frame, Rect(samples_positions[i].x, samples_positions[i].y,
				      SAMPLE_SIZE, SAMPLE_SIZE), Scalar(0, 255, 0), 2);
		}
		
	

		imshow("Cubre los cuadrados con la mano y pulsa espacio", tmp_frame);
		char c = cvWaitKey(40);
		if (c == ' ')
		{
			break;
		}
		cap >> frame;
	}

        // CODIGO 1.1
        // Obtener las regiones de interés y calcular la media de cada una de ellas
        // almacenar las medias en la variable means

	cvtColor(frame,hls_frame,CV_BGR2HLS);
	Scalar media;

	for(int i=0;i<max_samples;i++) {

	   Mat roi=hls_frame(Rect(samples_positions[i].x,samples_positions[i].y,SAMPLE_SIZE,SAMPLE_SIZE));
	   media=mean(roi);
	   means[i]=media;
	}
	
        destroyWindow("Cubre los cuadrados con la mano y pulsa espacio");

}
void  MyBGSubtractorColor::ObtainBGMask(cv::Mat frame, cv::Mat &bgmask) {
        
        // CODIGO 1.2
        // Definir los rangos máximos y mínimos para cada canal (HLS) 
        // umbralizar las imágenes para cada rango y sumarlas para
        // obtener la máscara final con el fondo eliminado

    Mat hls_frame;
    cvtColor(frame,hls_frame,CV_BGR2HLS);
	Mat tmp_bgmask(frame.rows,frame.cols,CV_8UC1,Scalar(0));
	Mat aux;
    //bgmask.setTo(Scalar(0));
    
	for(int i=0;i<max_samples;i++) {
	   
	   if(means[i][0]-h_low<0)
		lower_bounds[i][0]=0;
	   else
	   	lower_bounds[i][0]=means[i][0]-h_low;
	   
	   if(means[i][1]-l_low<0)
		lower_bounds[i][1]=0;
	   else
	   	lower_bounds[i][1]=means[i][1]-l_low;
	   
	   if(means[i][2]-s_low<0)
		lower_bounds[i][2]=0;
	   else
		lower_bounds[i][2]=means[i][2]-s_low;
   
	   if(means[i][0]+h_up>255)
		upper_bounds[i][0]=255;
	   else
		upper_bounds[i][0]=means[i][0]+h_up;
	   
	   if(means[i][1]+l_up>255)
		upper_bounds[i][1]=255;
	   else
                upper_bounds[i][1]=means[i][1]+l_up;
	   
	   if(means[i][2]+s_up>255)
		upper_bounds[i][2]=255;
	   else
		upper_bounds[i][2]=means[i][2]+s_up;

	   inRange(hls_frame,lower_bounds[i],upper_bounds[i],aux);
	   tmp_bgmask+=aux;
	}
	tmp_bgmask.copyTo(bgmask);	
}
