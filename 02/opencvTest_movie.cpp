#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{

    VideoCapture cap("vtest.avi");
  
    if(!cap.isOpened()){
        return -1;
    }
    
    Mat frame;
    namedWindow("movie", WINDOW_AUTOSIZE);
    
    while(cap.read(frame)){
        imshow("movie", frame);
        waitKey(30);
    }
    return 0;
}
