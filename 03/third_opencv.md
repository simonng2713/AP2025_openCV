# 背景差分，色検出

## 背景差分

動画中，動いている物体をトラッキングするためには，動いている物体を検出する必要があります．
そのための多種多様な方法が提案されていますが，
その中の（おそらく一番）単純な方法の一つが背景差分です．

背景差分では，背景のみが映った画像をまず準備しておいて，現在の画像との引き算を行います．
移動物体（背景差分に映っていない物体）があれば，引き算したときの残りとなるので，移動物体を抽出できます．
背景画像が必要なので，移動ロボットに搭載されたカメラの画像に対しては，カメラの向きや位置が変わり続けるため利用は難しいですが，
固定カメラによる画像に対してはそれなりに使えます．

### 一番シンプルな方法

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

  Mat src, dst, gray, back;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;

  cap.read(src);
  cvtColor(src, back, COLOR_BGR2GRAY);

  while (loopflag) {
    if (playing && cap.read(src)) {
      cvtColor(src, gray, COLOR_BGR2GRAY);
      absdiff(gray, back, dst);
      imshow("movie", dst);
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

この例では，whileループに入る前の，
```cpp
cap.read(src);
cvtColor(src, back, COLOR_BGR2GRAY);
```
にて，一度画像を取得し，それをグレースケールに変換したものを背景画像`back`としています．

whileループ内で，取得した画像をグレースケールに変換し`gray`，それと背景画像`back`との差分を，[`absdiff()`](https://docs.opencv.org/4.5.0/d2/de8/group__core__array.html#ga6fef31bc8c4071cbc114a758a2b79c14)で計算し，結果を`dst`に入れています．



### 動的背景差分

上記の方法は，非常にシンプルですが，

- ノイズの影響
- 影（移動物体と一緒に移動する）
- 固定カメラでも照明の変化により背景部分の画像も変化

など，そのままでは，実環境では利用するのは難しいです．

そこで，前もって背景画像を準備するのではなく，逐次的に背景画像を複数枚の画像から生成するといった，動的な背景差分法がいろいろ提案されており，
OpenCVにもライブラリーとして，使えるものが準備されています．

- [https://docs.opencv.org/4.5.0/d7/df6/classcv_1_1BackgroundSubtractor.html](https://docs.opencv.org/4.5.0/d7/df6/classcv_1_1BackgroundSubtractor.html)

ここでは，その一つである[BackgroundSubtractorMOG2](https://docs.opencv.org/4.5.0/d7/d7b/classcv_1_1BackgroundSubtractorMOG2.html)を使ってみます．

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

  Mat src, dst;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;

  int history = 100;
  float varThreshold = 50;
  bool bShadowDetection = false;
  Ptr<BackgroundSubtractor> bgfs =
      createBackgroundSubtractorMOG2(history, varThreshold, bShadowDetection);

  while (loopflag) {
    if (playing && cap.read(src)) {
      bgfs->apply(src, dst);
      imshow("movie", dst);
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

whileループ直前の

```cpp
int history = 100;
float varThreshold = 50;
bool bShadowDetection = false;
Ptr<BackgroundSubtractor> bgfs = createBackgroundSubtractorMOG2(history, varThreshold, bShadowDetection);
```      
にて，動的な背景を生成する準備をしています．`createBackgroundSubtractorMOG2()`の

- 一つ目の引数で`history`で動的な背景を生成するのに何枚の画像を使うか，
- 二つ目の引数で，背景かどうかを判定する閾値
- 三つ目の引数で，影を検出するかどうか

を指定しています．

背景差分を計算しているところは，whileループ中の

```cpp
bgfs->apply(src, dst);
```

です．一つ目の引数に，現在の画像(src)を渡し，二つ目の引数(dst)に背景差分の計算結果が格納されます．

単純な背景差分との差や，`history`,`varThreshold`, `bShadowDetection`を変えたときにどうなるかを確認してみてください．


## しきい値処理
    
背景差分をするときの，```absdiff()```で生成される画像はグレースケールになっています．これを2値化して，マスク画像を作ってみます．

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

  Mat src, mask, dst, gray, back, diff;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;
  double thresh = 30;

  cap.read(src);
  cvtColor(src, back, COLOR_BGR2GRAY);

  while (loopflag) {
    if (playing && cap.read(src)) {
      cvtColor(src, gray, COLOR_BGR2GRAY);
      absdiff(gray, back, diff);
      threshold(diff, mask, thresh, 255, THRESH_BINARY);

      imshow("movie", mask);
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

二値化をしているのは，[`threshold()`](https://docs.opencv.org/4.5.0/d7/d1b/group__imgproc__misc.html#gae8a4a146d1ca78c626a53577199e9c57)です．

引数は
- 一つ目: 入力画像
- 二つ目: 出力画像
- 三つ目: 閾値
- 四つ目: 閾値以上の値を持つ画素に対して割り当てられる値
- 五つ目: [2値化の方法](https://docs.opencv.org/4.5.0/d7/d1b/group__imgproc__misc.html#gaa9e58d2860d4afa658ef70a9b1115576)

です．

第三引数の閾値の値をかえるとマスク画像が変化します．確認してみてください．トラックバーで変更できるようにするとわかりやすくなります．

マスク画像が準備できると，元の画像から動いている部分だけを表示といったこともできます．先ほどをちょっと修正するだけです．

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

  Mat src, mask, gray, back, diff;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;
  double thresh = 30;

  cap.read(src);
  cvtColor(src, back, COLOR_BGR2GRAY);

  while (loopflag) {
    if (playing && cap.read(src)) {
      cvtColor(src, gray, COLOR_BGR2GRAY);
      absdiff(gray, back, diff);
      threshold(diff, mask, thresh, 255, THRESH_BINARY);

      Mat dst;
      src.copyTo(dst, mask);

      imshow("movie", dst);
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

```cpp
Mat dst;
src.copyTo(dst, mask);
```
にて，srcに入っている画像を，[`copyTo()`](https://docs.opencv.org/4.5.0/d3/d63/classcv_1_1Mat.html#a626fe5f96d02525e2604d2ad46dd574f)でdstにコピーしているのですが，
そのときに，マスク画像を二つ目の引数で渡すことで，srcに入っている画像をマスク画像で切り抜いたものをdstに入れるということができます．

    
## 色検出

背景差分とは別な方法として，色を検出（抽出）する方法があります．まずはBGRのままやってみます．


```cpp
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p, void *) { cap.set(CAP_PROP_POS_FRAMES, p); }

double B_MAX = 50;
double B_MIN = 0;
double G_MAX = 50;
double G_MIN = 0;
double R_MAX = 250;
double R_MIN = 100;

int main() {
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;

  while (loopflag) {
    if (playing && cap.read(src)) {
      Mat dst, mask;

      Scalar s_min = Scalar(B_MIN, G_MIN, R_MIN);
      Scalar s_max = Scalar(B_MAX, G_MAX, R_MAX);
      inRange(src, s_min, s_max, mask);
      src.copyTo(dst, mask);

      imshow("movie", dst);
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

ポイントは，
```cpp
Scalar s_min = Scalar(B_MIN, G_MIN, R_MIN);
Scalar s_max = Scalar(B_MAX, G_MAX, R_MAX);
inRange(src, s_min, s_max, mask);
src.copyTo(dst, mask);
```

のあたりです．まず，B,G,Rのそれぞれの値の下限値上限値を準備し（`s_min`, `s_max`），それを使って
['inRange()'](https://docs.opencv.org/4.5.0/d2/de8/group__core__array.html#ga48af0ab51e36436c5d04340e036ce981)で，マスク画像を生成しています．
マスク画像ができたら，上の例と同様に`copyTo()`でマスクをかけた画像を生成しています．

この例では，
```cpp
double B_MAX = 50;
double B_MIN = 0;
double G_MAX = 50;
double G_MIN = 0;
double R_MAX = 250;
double R_MIN = 100;
```
より，B,Gに比べRの範囲を広くしていますので，赤色を抽出するような処理になっています．これらの値を変えた時の変化を確認してみてください．
（ただし，vtest.aviではなく，色がはっきりした他の動画の方が変化がわかりやすいかも知れません）


## 練習問題3

上の例では，カラー画像の色空間をBGRとし，B,G,Rそれぞれの値の範囲を決めて，色の検出を行っていました．
ただ，カラー画像の色空間としては，色相(Hue)，彩度(Saturation)，明度(Value)で決まるHSV空間も存在します．
HSV色空間は人間が色を知覚する方法と類似しているため，BGR(RGB）色空間よりも人がイメージした通りの色になりやすく，
色検出もHSV空間でやる方が良い場合があります．

そこで，

- BGRではなく，HSVで色検出
- trackbarでHSVそれぞれの閾値を調整

するプログラムを作成してください．
