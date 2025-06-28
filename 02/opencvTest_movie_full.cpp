#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

int main() {
  
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, gray, color;
  namedWindow("movie", WINDOW_AUTOSIZE);

  //key detection
  bool loopflag = true;
  bool isColor = false;
  bool isGray = false;

  while (loopflag) {
    if (cap.read(src)) {
      if (isColor) {
        cvtColor(src, color, COLOR_BGR2RGB);
        imshow("movie", color);
      } else if (isGray) {
        cvtColor(src, gray, COLOR_BGR2GRAY);
        imshow("movie", gray);
      } else {
        imshow("movie", src);
      }
    } else {
      // End of video or error
      break;
    }

    int c = waitKey(10);
    switch (c) {
      case 'e':
        loopflag = false;
        break;
      case 'c':
        isColor = true;
        isGray = false;
        break;
      case 'g':
        isColor = false;
        isGray = true;
        break;
      case 'o': // original
        isColor = false;
        isGray = false;
        break;
      case ' ': // pause/play
        // Wait for next key press to resume
        while (true) {
          int k = waitKey(0);
          if (k == ' ' || k == 'e') {
            if (k == 'e') loopflag = false;
            break;
          }else if (k == 'a') { // previous frame
            if (cap.get(CAP_PROP_POS_FRAMES) > 1) {
              cap.set(CAP_PROP_POS_FRAMES, cap.get(CAP_PROP_POS_FRAMES) - 2);
                  if (cap.read(src)) {
                    if (isColor) {
                        cvtColor(src, color, COLOR_BGR2RGB);
                        imshow("movie", color);
                    } else if (isGray) {
                        cvtColor(src, gray, COLOR_BGR2GRAY);
                        imshow("movie", gray);
                    } else {
                        imshow("movie", src);
                    }
                } 
            }
          } else if (k == 'd') { // next frame
            if (cap.get(CAP_PROP_POS_FRAMES) < cap.get(CAP_PROP_FRAME_COUNT)) {
                  if (cap.read(src)) {
                    if (isColor) {
                        cvtColor(src, color, COLOR_BGR2RGB);
                        imshow("movie", color);
                    } else if (isGray) {
                        cvtColor(src, gray, COLOR_BGR2GRAY);
                        imshow("movie", gray);
                    } else {
                        imshow("movie", src);
                    }
                } 

            }
          } 
          else if (k == 'r') { // reset
            cap.set(CAP_PROP_POS_FRAMES, 0);
            if (cap.read(src)) {
              if (isColor) {
                cvtColor(src, color, COLOR_BGR2RGB);
                imshow("movie", color);
              } else if (isGray) {
                cvtColor(src, gray, COLOR_BGR2GRAY);
                imshow("movie", gray);
              } else {
                imshow("movie", src);
              }
            }
          }
          else if(k == 'c'){
            isColor = true;
            isGray = false;
            if (cap.get(CAP_PROP_POS_FRAMES) > 1) {
              cap.set(CAP_PROP_POS_FRAMES, cap.get(CAP_PROP_POS_FRAMES) - 1);
            }
            if (cap.read(src)) {
              cvtColor(src, color, COLOR_BGR2RGB);
              imshow("movie", color);
            }
          }
          else if(k == 'g'){
            isColor = false;
            isGray = true;
            if (cap.get(CAP_PROP_POS_FRAMES) > 1) {
              cap.set(CAP_PROP_POS_FRAMES, cap.get(CAP_PROP_POS_FRAMES) - 1);
            }
            if (cap.read(src)) {
              cvtColor(src, gray, COLOR_BGR2GRAY);
              imshow("movie", gray);
            }
          }
          else if(k == 'o'){
            isColor = false;
            isGray = false;
            if (cap.get(CAP_PROP_POS_FRAMES) > 1) {
              cap.set(CAP_PROP_POS_FRAMES, cap.get(CAP_PROP_POS_FRAMES) - 1);
            }
            if (cap.read(src)) {
              imshow("movie", src);
            }
          }
        }
        break;
      /*case 'a': // previous frame
        if (cap.get(CAP_PROP_POS_FRAMES) > 1) {
          cap.set(CAP_PROP_POS_FRAMES, cap.get(CAP_PROP_POS_FRAMES) - 2);
        }
        break;
      case 'd': // next frame
        if (cap.get(CAP_PROP_POS_FRAMES) < cap.get(CAP_PROP_FRAME_COUNT)) {
          // nothing needed, next read will advance
        }
        break;*/
      case 'r': // reset
        cap.set(CAP_PROP_POS_FRAMES, 0);
        break;
      default:
        break;
    }
  }

  return 0;
}
