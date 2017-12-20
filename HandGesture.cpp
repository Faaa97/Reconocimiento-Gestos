#include "HandGesture.hpp"

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

HandGesture::HandGesture() {
	
}


double HandGesture::getAngle(Point s, Point e, Point f) {
	
	double v1[2],v2[2];
	v1[0] = s.x - f.x;
	v1[1] = s.y - f.y;

	v2[0] = e.x - f.x;
	v2[1] = e.y - f.y;

	double ang1 = atan2(v1[1],v1[0]);
	double ang2 = atan2(v2[1],v2[0]);

	double angle = ang1 - ang2;
	if (angle > CV_PI) angle -= 2 * CV_PI;
	if (angle < -CV_PI) angle += 2 * CV_PI;
	return (angle * 180.0/CV_PI);
}
void HandGesture::FeaturesDetection(Mat mask, Mat output_img) {
	
	vector<vector<Point> > contours;
	Mat temp_mask;
	mask.copyTo(temp_mask);
	int index = 0;

        // CODIGO 3.1
        // detección del contorno de la mano y selección del contorno más largo
        //...
    findContours(temp_mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    Rect boundRect;

    for(int i = 0; i < linea.size(); i++)
        circle(output_img, linea[i], 2, Scalar(0, 255, 255), 8);
    
    for (int i=1; i<contours.size(); i++)
    {
        if(contours[i].size()>contours[index].size())
            index=i;
    }
    if(contours.size() > index){
        boundRect = boundingRect(Mat(contours[index]));
        rectangle(output_img,boundRect.tl(), boundRect.br(), Scalar(0, 255, 0), 2);
    }
        
        // pintar el contorno
        //...
    drawContours(output_img, contours, index, cv::Scalar(255, 0, 0), 2, 8, vector<Vec4i>(), 0, Point());

	//obtener el convex hull	
	vector<int> hull;
    if(contours.size() > index)
	    convexHull(contours[index],hull);
    else
        return;
	
	// pintar el convex hull    
    Point pt0;
    if(contours[index].size() >  hull[hull.size()-1])
	    pt0 = contours[index][hull[hull.size()-1]];
    else
        return;
        
	for (int i = 0; i < hull.size(); i++)
	{
        Point pt;
        if(contours.size() > index && contours[index].size() > hull[i])
		    pt = contours[index][hull[i]];
        else
            return;
		line(output_img, pt0, pt, Scalar(0, 0, 255), 2, CV_AA);
		pt0 = pt;
	}
	
        //obtener los defectos de convexidad
	vector<Vec4i> defects;
    if(contours.size() > index)
	    convexityDefects(contours[index], hull  , defects);
    else
        return;
		
	Point latest;
	int cont = 0;
	for (int i = 0; i < defects.size(); i++) {
		    Point s = contours[index][defects[i][0]];
		    Point e = contours[index][defects[i][1]];
		    Point f = contours[index][defects[i][2]];
		    float depth = (float)defects[i][3] / 256.0;
		    double angle = getAngle(s, e, f);
	
                        // CODIGO 3.2
                        // filtrar y mostrar los defectos de convexidad
                        //...
            if (angle < 92 && depth > 100)
            {
                cont++;
                circle(output_img, f, 5, Scalar(0, 255, 0), 3);
                if(cont == 1)
                    latest = f;
            }
    }
    if(cont == 1)
        linea.push_back(latest);

    putText(output_img, to_string(boundRect.area()), Point(output_img.cols/2,output_img.rows), FONT_HERSHEY_SCRIPT_SIMPLEX, 2, Scalar(255,255,255));

    if (cont!=0)
        putText(output_img, to_string(cont+1), Point(0,output_img.rows), FONT_HERSHEY_SCRIPT_SIMPLEX, 2, Scalar(255,255,255));
    else
        if (boundRect.area()>80000)
            putText(output_img, to_string(1), Point(0,output_img.rows), FONT_HERSHEY_SCRIPT_SIMPLEX, 2, Scalar(255,255,255));
        else
            putText(output_img, to_string(0), Point(0,output_img.rows), FONT_HERSHEY_SCRIPT_SIMPLEX, 2, Scalar(255,255,255));
}
