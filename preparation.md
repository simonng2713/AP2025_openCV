# 準備

## Windows 

wslを起動し，OpenCVをインストールしてください．

```
sudo apt update
sudo apt install libopencv-dev libopencv-contrib-dev
```

次に，wslのGUIアプリを起動するためのGWSLをインストールします．

1. Microsoft store で「GWSL」と検索してインストール．
2. インストールが完了したら「GWSL」を起動

参考：　https://qiita.com/1024xx4/items/45a5cce13eafa17ca6f1

ただし，windows11だと，wslがGUIアプリを起動するための機能が統合されたwslg(Windows Subsystem for Linux GUI)となっているかもしれません．
その場合は，GWSLのインストールは不要です．

要不要の確認方法は，まず，GWSLをインストールしていない状態で，
```
sudo apt install x11-apps
```
としたあとに，
```
xeyes
```
して，何か起動すれば，GWSLのインストールは不要です．

## Mac

ターミナルにて，homebrewを使ってインストール．

```
brew update 
brew install opencv
```

## 共通

インストール作業後，ターミナルにて

```
opencv_verson
```
としたときに，version番号が表示されば，問題なくインストールされています． （2024/6/5現在），Windows(wsl)だと4.8.0，Macだと4.8.1あたりになります．

