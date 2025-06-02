## カメラの利用について

`VideoCapture`のインスタンスを生成するところで，動画のファイル名ではなく，

```
VideoCapture cap(0);
```

とすれば，PCに繋がったカメラ（最近のノートPCだと，内蔵カメラ）を使うことができます．
ただ，Macならこれだけでよいのですが，WSL on Windowsだと．現状WSL上で動かしているUbuntuのカーネルコンパイルが必要となります．

本演習では，カメラは使わない課題としていますが，Windowsでカメラを使ってみたい人は，下記ページを参考にカーネルコンパイル等の準備を頑張ってみるか，
wsl上ではなく，Windows nativeでの開発環境（Visual StudioとWindows用の[OpenCV](https://opencv.org/releases/)）を準備してみてください．


### 参考
- https://touch-sp.hatenablog.com/entry/2022/01/30/001058
- https://github.com/dorssel/usbipd-win/wiki/WSL-support
