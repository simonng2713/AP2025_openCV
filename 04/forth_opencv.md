# トラッキング

[背景差分，色検出](../03/third_opencv.md)で説明した方法を使うと，画像中で移動する対象の抽出ができるようになります．次は，それをトラッキングしてみます．


## 輪郭の抽出

抽出した対象の輪郭を抽出してみます．

```cpp
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p, void *) { cap.set(CAP_PROP_POS_FRAMES, p); }

int main() {
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, mask, dst, gray, back;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;
  double thresh = 50;

  cap.read(src);
  cvtColor(src, back, COLOR_RGB2GRAY);

  while (loopflag) {
    if (playing && cap.read(src)) {
      cvtColor(src, gray, COLOR_RGB2GRAY);
      absdiff(gray, back, mask);
      threshold(mask, dst, thresh, 255, THRESH_BINARY);

      std::vector<std::vector<Point> > contours;
      std::vector<Vec4i> hierarchy;
      findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
      for (int i = 0; i < contours.size(); i++) {
        drawContours(src, contours, (int)i, Scalar(0, 0, 255), 2);
      }
      imshow("movie", src);
    }

    char c = waitKey(30);
    switch (c) {
      case ' ':
        playing = !playing;
        break;
      case 'e':
        loopflag = false;
        break;
      case 's':
        cap.set(CAP_PROP_POS_FRAMES, 0);
      default:
        break;
    }
  }

  return 0;
}
```

輪郭の抽出は，[`findContours()`](https://docs.opencv.org/4.5.0/d3/dc0/group__imgproc__shape.html#gadf1ad6a0b82947fa1fe3c3d497f260e0)でできます．

```cpp
std::vector<std::vector<Point> > contours;
std::vector<Vec4i> hierarchy;
findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
```

`findContours()`の引数は，
- 一つ目: 入力画像
- 二つ目: 抽出した輪郭の点の情報をいれる変数
- 三つ目: 抽出した輪郭が階層構造を持つ場合に，その情報を入れる変数
- 四つ目: [抽出のモード](https://docs.opencv.org/4.5.0/d3/dc0/group__imgproc__shape.html#ga819779b9857cc2f8601e6526a3a5bc71)
- 五つ目: [輪郭の抽出手法](https://docs.opencv.org/4.5.0/d3/dc0/group__imgproc__shape.html#ga4303f45752694956374734a03c54d5ff)

上の例だと，

- RETR_EXTERNAL: 最も外側の輪郭のみを抽出
- CHAIN_APPROX_SIMPLE: 水平・垂直・斜めの線分を圧縮し，それらの端点のみを残す

で，輪郭が抽出され，その点のデータが，`cv::Point`の2次元ベクトル`contours`に格納されます．
上を実行してみるとよくわかると思いますが，輪郭は複数抽出されることがあるので，各輪郭毎に点がまとまった2次元ベクトルとして抽出結果が格納されています．

それを，抽出された輪郭の点群を表示するには，[`drawContours()`](https://docs.opencv.org/4.5.0/d6/d6e/group__imgproc__draw.html#ga746c0625f1781f1ffc9056259103edbc)を使います．

```cpp
for (int i = 0; i < contours.size(); i++) {
  drawContours(src, contours, (int)i, Scalar(0, 0, 255), 2);
}
```

`drawContours()`の引数は，
- 一つ目: 出力画像
- 二つ目: 抽出した輪郭が入った変数
- 三つ目: どの輪郭を描くか
- 四つ目: 輪郭の色
- 五つ目: 輪郭線の太さ

です． 
contoursには各輪郭毎に点がまとまった2次元ベクトルとして抽出結果が入っているので，
各輪郭を一つずつ取り出し，その点群を表示という形で，抽出した輪郭の表示をしています．



## 面積の計算

画像にノイズやちらつきが存在したり，二値化の閾値によっては，非常に小さい輪郭として抽出されるものがあります．
それを除くために面積の計算をしてみます．
[`contourArea()`](https://docs.opencv.org/4.5.0/d3/dc0/group__imgproc__shape.html#ga2c759ed9f497d4a618048a2f56dc97f1)を使って計算します．

```cpp
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p, void *) { cap.set(CAP_PROP_POS_FRAMES, p); }

int main() {
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, mask, dst, gray, back;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;
  double thresh = 50;

  cap.read(src);
  cvtColor(src, back, COLOR_RGB2GRAY);

  while (loopflag) {
    if (playing && cap.read(src)) {
      cvtColor(src, gray, COLOR_RGB2GRAY);
      absdiff(gray, back, mask);
      threshold(mask, dst, thresh, 255, THRESH_BINARY);

      std::vector<std::vector<Point> > contours;
      std::vector<Vec4i> hierarchy;

      findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

      for (int i = 0; i < contours.size(); i++) {
        if (contourArea(contours[i]) > 50) {
          drawContours(src, contours, (int)i, Scalar(0, 0, 255), 2);
        }
      }
      imshow("movie", src);
    }

    char c = waitKey(30);
    switch (c) {
      case ' ':
        playing = !playing;
        break;
      case 'e':
        loopflag = false;
        break;
      case 's':
        cap.set(CAP_PROP_POS_FRAMES, 0);
      default:
        break;
    }
  }

  return 0;
}
```

変わっているのは

```cpp
for (int i = 0; i < contours.size(); i++) {
  if (contourArea(contours[i]) > 50) {
    drawContours(src, contours, (int)i, Scalar(0, 0, 255), 2);
  }
}
```
の部分だけです．

`contourArea()`で`contours[i]`の面積の計算をし，それが50以上のときだけ輪郭を表示しています．
この面積の閾値(50)や，二値化の閾値`thresh`を変えて，どうなるか確認してみてくだい．



## 輪郭を囲む外接矩形

抽出した輪郭を輪郭を囲む外接矩形を求め，表示してみます．

```cpp
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p, void *) { cap.set(CAP_PROP_POS_FRAMES, p); }

int main() {
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, mask, dst, gray, back;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;
  double thresh = 50;

  cap.read(src);
  cvtColor(src, back, COLOR_RGB2GRAY);

  while (loopflag) {
    if (playing && cap.read(src)) {
      cvtColor(src, gray, COLOR_RGB2GRAY);
      absdiff(gray, back, mask);
      threshold(mask, dst, thresh, 255, THRESH_BINARY);

      std::vector<std::vector<Point> > contours;
      std::vector<Vec4i> hierarchy;

      findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

      std::vector<Rect> boundRect(contours.size());

      for (int i = 0; i < contours.size(); i++) {
        if (contourArea(contours[i]) > 100) {
          drawContours(src, contours, (int)i, Scalar(0, 0, 255), 2);

          boundRect[i] = boundingRect(contours[i]);
          rectangle(src, boundRect[i].tl(), boundRect[i].br(),
                    Scalar(255, 0, 0), 2);
          // top-left, bottom-right
        }
      }
      imshow("movie", src);
    }

    char c = waitKey(30);
    switch (c) {
      case ' ':
        playing = !playing;
        break;
      case 'e':
        loopflag = false;
        break;
      case 's':
        cap.set(CAP_PROP_POS_FRAMES, 0);
      default:
        break;
    }
  }

  return 0;
}
```

点群の外接矩形は，[`boundingRect()`](https://docs.opencv.org/4.5.0/d3/dc0/group__imgproc__shape.html#ga103fcbda2f540f3ef1c042d6a9b35ac7)にて求められます．


上の例では，
```cpp
std::vector<Rect> boundRect(contours.size());
```
と，求めた外接矩形の情報`Rect`を入れる変数を準備しておき，ループの中で

```cpp
boundRect[i] = boundingRect(contours[i]);
rectangle(src, boundRect[i].tl(), boundRect[i].br(),
          Scalar(255, 0, 0), 2);
```
として，`boundingRect()`で`contours[i]`を囲む外接矩形をもとめ，
[`rectangle()`](https://docs.opencv.org/4.5.0/d6/d6e/group__imgproc__draw.html#ga07d2f74cadcf8e305e810ce8eed13bc9)でその矩形を表示しています．

[`Rect`](https://docs.opencv.org/4.5.0/d2/d44/classcv_1_1Rect__.html)は矩形を扱うclass(正確にはClass Templateですが）で，
矩形の左上の座標や幅や高さの情報を持っています．
また，`tl()`や`br()`というメソッドで，矩形の左上及び右下の座標を得ることができます．[参照]

矩形を表示する`rectangle()`の引数は，
- 一つ目: 出力画像
- 二つ目: 描く矩形の左上(top-left)の座標
- 三つ目: 描く矩形の右下(bottom-right)の座標
- 四つ目: 線の色
- 五つ目: 線の太さ（これを負の値にすると，塗りつぶす）

です．

## 抽出した領域の重心

抽出した領域の重心を求めて表示してみます．
OpenCVでは，領域のモーメントを求めて，それを使って重心を計算できます．


```cpp
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p, void *) { cap.set(CAP_PROP_POS_FRAMES, p); }

int main() {
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, mask, dst, gray, back;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;
  double thresh = 50;

  cap.read(src);
  cvtColor(src, back, COLOR_RGB2GRAY);

  while (loopflag) {
    if (playing && cap.read(src)) {
      cvtColor(src, gray, COLOR_RGB2GRAY);
      absdiff(gray, back, mask);
      threshold(mask, dst, thresh, 255, THRESH_BINARY);

      std::vector<std::vector<Point> > contours;
      std::vector<Vec4i> hierarchy;

      findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

      std::vector<Rect> boundRect(contours.size());

      for (int i = 0; i < contours.size(); i++) {
        if (contourArea(contours[i]) > 100) {
          drawContours(src, contours, (int)i, Scalar(0, 0, 255), 2);

          boundRect[i] = boundingRect(contours[i]);
          rectangle(src, boundRect[i].tl(), boundRect[i].br(),
                    Scalar(255, 0, 0), 2);
          // top-left, bottom-right

          Moments mu = moments(contours[i]);
          Point2f mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);

          circle(src, mc, 4, Scalar(0, 255, 0), 2, 4);
        }
      }
      imshow("movie", src);
    }

    char c = waitKey(30);
    switch (c) {
      case ' ':
        playing = !playing;
        break;
      case 'e':
        loopflag = false;
        break;
      case 's':
        cap.set(CAP_PROP_POS_FRAMES, 0);
      default:
        break;
    }
  }

  return 0;
}
```

追加されたのは，
```
Moments mu = moments(contours[i]);
Point2f mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);

circle(src, mc, 4, Scalar(0, 255, 0), 2, 4);
```

[`moment()`](https://docs.opencv.org/4.5.0/d3/dc0/group__imgproc__shape.html#ga556a180f43cab22649c23ada36a8a139)
にて，領域`contours[i]`のモーメントを求め，

$$
( \frac{m_{10}}{m_{00}}, \frac{m_{01}}{m_{00}} ) 
$$

で重心を計算．

[`circle()`](https://docs.opencv.org/4.5.0/d6/d6e/group__imgproc__draw.html#gaf10604b069374903dbd0f0488cb43670)
で，重心の位置に○を表示しています．


円を表示する`circle()`の引数は，
- 一つ目: 出力画像
- 二つ目: 描く円の中心位置
- 三つ目: 描く円の半径
- 四つ目: 線の色
- 五つ目: 線の太さ（これを負の値にすると，塗りつぶす）

です．

重心位置を格納した`mc`は，`Point2f`クラスのインスタンスで，　メンバー変数`x`と`y`を持ちます．
`mc.x`や`ｍｃ．ｙ`とすれば，重心の画像上の座標がわかります．
`printf()`や`std::cout`を用いて，重心の座標位置を表示してみてください．

ちなみに`Point2f`はテンプレート[`Point_`](https://docs.opencv.org/4.5.0/db/d4e/classcv_1_1Point__.html)から生成されたクラスです．

