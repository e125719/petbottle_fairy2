#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <stdio.h>

using namespace std;
using namespace cv;

class ObjDetect{
    
    cv::Mat backImg;
    cv::Mat frame;
    cv::Mat grayFrame;
    cv::Mat diff;
    cv::Mat texture;
    float area1 = 0;
    float area = 0;
    cv::Rect brect;
    int roiCnt = 0;
    int i = 0;
    
    float cx;
    float cy;
    
public:
    
    ObjDetect(void) {
        // Constructer
    }
    
    int getBackImg(){
        
        cout << "please press key" << endl;
        
       // getchar();
        
        cv::VideoCapture cap(0);
        if (!cap.isOpened())
        {
            cerr << "camera cannot open" << endl;
            return -1;
        }
        cap >> backImg;
        
        cv::cvtColor(backImg, backImg, CV_BGR2GRAY);
        
        cv::Mat smallframe;
        resize(backImg, smallframe, Size(640,480));
        backImg = smallframe;
        
        return 0;
        
    }
    
    
    bool detectObject(){
        cv::VideoCapture cap1(0);
        if (!cap1.isOpened())
        {
            cerr << "camera cannot open" << endl;
            return -1;
        }
        cap1 >> frame;
        cv::Mat smallframe;
        resize(frame, smallframe, Size(640,480));
        frame = smallframe;
        
        
        
        cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);
        cv::absdiff(backImg,grayFrame,diff);
        cv::threshold(diff,diff,20,255,CV_THRESH_BINARY);
        
        cv::erode(diff,diff,getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        cv::dilate(diff,diff,getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        cv::dilate(diff,diff,getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        cv::erode(diff,diff,getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5)));
        
        vector< vector< cv::Point > > contours;
        cv::findContours(diff, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        
        for (auto contour = contours.begin(); contour != contours.end(); contour++){
            vector< cv::Point > approx;
            cv::approxPolyDP(cv::Mat(*contour), approx, 0.01 * cv::arcLength(*contour, true), true);
            area1 = cv::contourArea(approx);
        }
        
        if(area1 > 1000)
            return true;
        else
            return false;
        
    }
    
    
    void makeTexture(){
        vector< vector< cv::Point > > contours;
        vector< cv::Point > approx;
        cv::findContours(diff, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        
        cv::Moments mom = cv::moments(contours[i]);
        cx = mom.m10/mom.m00;
        cy = mom.m01/mom.m00;
        
        cout << "重心座標（"<< cx << "," << cy << "）" << endl;
        
        float maxarea = 0;
        
        for (auto contour = contours.begin(); contour != contours.end(); contour++){
            cv::convexHull(*contour, approx);
            area = cv::contourArea(approx);
            
            if (area > maxarea) {
                maxarea = area;
                brect = cv::boundingRect(cv::Mat(approx).reshape(2));
                texture = cv::Mat(frame, brect);
            }
            
            i++;
        }
       
        //w = texture.cols;
        //h = texture.rows;

    }
    
    
    cv::Mat getTexture(){
        
        return texture;
        
    }
    
    float getPosition(){
        
        return cx;
        
    }
    
    
    
};