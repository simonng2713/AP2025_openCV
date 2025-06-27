#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat src;
cv::Mat dst;

int g_blur = 1;         // global blur (odd, 1-21)
double g_contrast = 1.0; // global contrast (0.0-1.0)
int g_brightness = 0;    // global brightness (-50~50)

void apply_all() {
    cv::Mat temp;
    // Apply blur first
    int blur_ksize = g_blur;
    cv::medianBlur(src, temp, blur_ksize);
    // Apply contrast and brightness
    double alpha = g_contrast;
    double beta = g_brightness;
    temp.convertTo(dst, -1, alpha, beta);
    cv::imshow("Display", dst);
}

void on_tracker_blur(int blur, void *) {
    g_blur = blur * 2 + 1; // 0~20 -> 1~21
    apply_all();
}

void on_tracker_contrast(int contrast, void *) {
    g_contrast = contrast / 100.0; // 0~100 -> 0.0~1.0
    apply_all();
}

void on_tracker_brightness(int brightness, void *) {
    g_brightness = brightness - 50; // 0~100 -> -50~50
    apply_all();
}

int main() {
    src = cv::imread("lena.jpg");

    cv::namedWindow("Display", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("blur", "Display", nullptr, 10, on_tracker_blur);
    cv::createTrackbar("contrast", "Display", nullptr, 100, on_tracker_contrast);
    cv::createTrackbar("brightness", "Display", nullptr, 100, on_tracker_brightness);

    cv::setTrackbarPos("blur", "Display", 1);
    cv::setTrackbarPos("contrast", "Display", 100); // default 1.0
    cv::setTrackbarPos("brightness", "Display", 50); // default 0
    apply_all();
    while (cv::waitKey(100) < 0) {
    }

    return 0;
}
