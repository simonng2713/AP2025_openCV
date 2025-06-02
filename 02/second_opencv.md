# 動画の表示

## namespace cv

[1. 画像の表示，簡単な画像処理](../01/first_opencv.md)では，OpenCV由来であることがわかるよう`cv::Mat`や`cv::imshow`といったように，
明示的にスコープ解決演算子`::`を用いて，名前空間cvを指定していました．
以降は，コード冒頭に

```
using namespace cv;
```
を記載して，名前空間の指定を省略しています．注意してください．


## 動画の読み込み

[動画(vtest.avi)](vtest.avi)を読み込んで表示してみます．

```cpp
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
```

[VideoCapture](https://docs.opencv.org/4.5.0/d8/dfe/classcv_1_1VideoCapture.html)が動画を扱うclassです．
`VideoCapture cap("vtest.avi");`で，そのインスタンス`cap`を生成し，vtest.aviを読み込んでいます．

whileループのところで
- メソッド`read()`で，1フレーム読み込み，引数であたえたMat型の変数frameに代入
- frameを`imshow()`で表示する
という処理を繰り返しています．

[read()](https://docs.opencv.org/4.5.0/d8/dfe/classcv_1_1VideoCapture.html#a473055e77dd7faa4d26d686226b292c1)
はフレームを読み込めたら`true`，読み込めなかったら`false`を返します．
それを使うことで，whileループにて動画の最後まで読み込むという処理を実現しています


## 動画への画像処理

```cpp
Mat frame;
cap.read(frame)
```
で，読み込まれたframeは，一枚の画像となるので，それに対して[1. 画像の表示，簡単な画像処理](../01/first_opencv.md)にて扱った画像処理をすることができます．

動画にblurをかける例は以下の通りになります．

```cpp
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int b = 2;

void on_tracker(int b_, void *) { b = b_; }

int main() {
  VideoCapture cap("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, dst;
  namedWindow("movie", WINDOW_AUTOSIZE);
  createTrackbar("blur", "movie", nullptr, 10, on_tracker);
  setTrackbarPos("blur", "movie", b);

  while (cap.read(src)) {
    medianBlur(src, dst, b * 2 + 1);
    imshow("movie", dst);
    waitKey(30);
  }
  return 0;
}
```

whileループのところで，
- `read()`で読み込んだフレームを`src`にいれる．
- `src`を`medianBlur()`でblur処理し，結果を`dst`にいれる．
- `dst`を`imshow()`で表示
を繰り返しています．

同様にすれば，contrastやbrightnessの調整も可能です．やってみましょう．

## 色変換（BGR to gray）


画像処理の別な例として，カラー画像をグレースケール画像に変換してみます．`cvtColor()`を使えば簡単です．

```cpp
#include <stdio.h>

#include <opencv2/opencv.hpp>

using namespace cv;

int main() {
  VideoCapture cap("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, gray;
  namedWindow("movie", WINDOW_AUTOSIZE);

  while (cap.read(src)) {
    cvtColor(src, gray, COLOR_BGR2GRAY);
    imshow("movie", gray);
    waitKey(30);
  }

  return 0;
}
```

```
cvtColor(src, gray, COLOR_BGR2GRAY);
```
で，`src`に入っている画像をグレースケールに変換し，その結果を`gray`にいれています．他にどういった変換ができるかは，[ここ](https://docs.opencv.org/4.5.0/d8/d01/group__imgproc__color__conversions.html#ga4e0972be5de079fed4e3a10e24ef5ef0) 参照．
よく使うのは，`COLOR_BGR2GRAY`や`COLOR_BGR2HSV`です．

なお，OpenCVでは，基本的にデータ上の色の並びがB,G,Rの順なので，`COLOR_BGR2GRAY`となってます．OpenCVだけを使っている場合はいいのですが，
OpenGL他，画像を扱う他のライブラリでは，R,G,Bの順でデータが構成されていることがあります．そのため，他ライブラリーと一緒に使うときは，注意が必用です


## フレーム位置を指定

ここまでは，動画を先頭から順に再生（表示）していましたが，再生位置を指定することができます．その例を示します．



```cpp
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p_, void *) {
    cap.set(CAP_PROP_POS_FRAMES, p_); 
}

int main() {
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src;
  namedWindow("movie", WINDOW_AUTOSIZE);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  while (cap.read(src)) {
    imshow("movie", src);
    waitKey(30);
  }
  return 0;
}
```

トラックバーに紐付けたcallback関数`on_tracker`の中で，
```
cap.set(CAP_PROP_POS_FRAMES, p_); 
```
として，動画のフレーム位置（再生位置）を変更しています．`p_`はトラックバーの位置です．他にsetできるものは，[ここ](https://docs.opencv.org/4.5.0/d8/dfe/classcv_1_1VideoCapture.html#a8c6d8c2d37505b5ca61ffd4bb54e9a7c)を参照．


また，
```
cap.get(CAP_PROP_FRAME_COUNT)
```
で，動画の全フレーム数を調べることができます．`createTrackbar()`の第四引数に渡して，トラックバーの最大値が動画の全フレーム数，つまり，動画の最後になるようにしています．


ちなみに，ここでは，`VideoCapture`インスタンス生成の際に動画を読み込むのではなく，
あとで，`open()`メソッドを使って，動画を読み込んでいます．実行時に読み込むファイルを変えたい場合などには，このやり方が便利です．



## キー入力

`waitKey()`で，キー入力を待つことができますが，そのときに入力されたキーに応じた処理をするということができます．


```cpp
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {
  VideoCapture cap("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src;
  namedWindow("movie", WINDOW_AUTOSIZE);

  bool loopflag = true;

  while (loopflag) {
    if (cap.read(src)) {
      imshow("movie", src);
    } else {
      loopflag = false;
    }

    int c = waitKey(10);
    switch (c) {
      case 'e':
        loopflag = false;
      default:
        break;
    }
  }

  return 0;
}
```

```
int c = waitKey(10);
```
と，`waitKey()`の戻り値をchar型の変数に代入してやり，それをif文なりswitch文なりで処理してあげれば，
キー入力に応じた処理をすることができます．
この例では，`e`が入力されたら，whileループから抜けて，プログラムが終了するようになっています．

`waitKey()`では，押されたキーのキーコードを返していますが，
特殊キー（カーソルキーやファンクションキー）も使いたいのであれば，
[`waitKeyEx()`](https://docs.opencv.org/4.5.0/d7/dfc/group__highgui.html#gafa15c0501e0ddd90918f17aa071d3dd0)を使う必要があります．


## 練習問題2

[動画(vtest.avi)](vtest.avi)を読み込み表示させる際，
トラックバーでフレーム位置を指定できるようにしつつ，キー入力でいろいろ操作できるものを作成してください．

たとえば，

- grayかカラー（BGR）を切り替える
- ポーズ
- （ポーズしているとき）1フレーム進む，1フレーム戻る．
- 巻き戻し（先頭へ）
- キー入力で途中終了

などを実装してみてください．



