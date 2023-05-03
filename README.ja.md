# TinyDarius

開発中です。現在ステージAのみ。  
自作ライブラリの[ドッグフーディング](https://ja.wikipedia.org/wiki/%E3%83%89%E3%83%83%E3%82%B0%E3%83%95%E3%83%BC%E3%83%87%E3%82%A3%E3%83%B3%E3%82%B0)も兼ねています。  
<ins>**権利上の問題で、画像や音のリソースデータは含まれておりません。**</ins>  
リソースを独自で作成する際の指針は[こちら](HowToMakeResources.md)


## 概要
アーケードゲーム"ダライアス"からボスラッシュを作りました。  
エミュレーター上での動作ではなく、独自のコードで動作しています。  
その為実際の挙動とは異なります。  
オリジナルの開発者ならびに製作会社に対し敬意を評します。

## 動作ハード
M5Stack Basic,Gray  

### 必要なもの
M5Stack Faces + GB Face  

## ビルド方法
[ArduinoIDE](https://www.arduino.cc/en/software) または [Visual Studio Code](https://code.visualstudio.com/) + [PlatformIO](https://platformio.org/) にてビルド可能です。  
各環境の整備方法などはそれぞれのページを参照してくだい。  

### 必要なライブラリ
* 排他(どちらかを使ってください)  
[M5Stack](https://github.com/m5stack/M5Stack) 0.4.3  
[LovyanGFX](https://github.com/lovyan03/LovyanGFX) 1.1.16  
or  
[M5Unified](https://github.com/m5stack/M5Unified) 0.1.6 (M5GFX を含む)

* 共通  
[SdFat](https://github.com/greiman/SdFat) 2.2.2  
[goblib](https://github.com/GOB52/goblib) 0.2.0  
[goblib_m5s](https://github.com/GOB52/goblib_m5s) 0.3.0

### platform.ini for PlatformIO

#### 設定

|項目 |設定値 |
|:---|:---|
|platform | espressif32@3.5.0 |
|board | m5stack-core-esp32 or m5stack-gray|


#### ビルド種別

|env|説明|備考|
|:---|:---|:---|
|master|マスタービルド (LovyanGFX v0)|デバッグ機能なし|
|master\_v1|マスタービルド (LovyanGFX v1)|デバッグ機能なし|
|release|リリースビルド (LovyanGFX v0)|デバッグ機能あり|
|release\_v1|リリースビルド (LovyanGFX v1)|デバッグ機能あり|
|debug|デバッグビルド (LovyanGFX v0)|デバッグ機能あり|
|debug|デバッグビルド (LovyanGFX v1)|デバッグ機能あり|

master または master\_v1 でのビルドを推奨します。  
私は ArduinoIDE と PlatformIO で M5Stack を除くライブラリを [lib\_extra\_dirs](https://docs.platformio.org/en/latest/projectconf/section_env_library.html#lib-extra-dirs) を介して共有使用しています。(ライブラリのインストールは ArduinoIDE を使用)  
ご自分の環境に合わせて platform.ini を書き換えてください。  

### TinyDarius.ino for ArduinoIDE

TinyDarius.ino 自体は空のファイルです。 setup(),loop() は ./src/main.cpp にあります。  
ビルドは platform.ini の release 相当のものとなります。  
他の env 相当でのビルドにするには platform.txt を書き換える必要があります。  
env の記述を参考にオプションを設定してください。

#### 設定

ボードマネージャー **M5Stack version 1.0.9**  

[Menu] - [Tool]  

|項目|設定値|
|:---|:---|
|Board|M5Stack-Core-ESP32|
|Flash Frequency|80|
|Flash Mode|QIO|
|Partation Scheme|Default|
|Core Debug|any |


## 遊び方

|ボタン|説明|
|:---|:---|
|セレクト|コイン投入|
|スタート|クレジットがあればゲーム開始|
|十字|自機の移動|
|A| 弾発射(ソフトウェアリピート付)|

1. コイン投入(Select押下)
1. スタート押下してゲーム開始
1. 敵が出てくるまで動けません(少々お待ちください)
1. ボスを倒します。(時間切れになるとボスは逃げます)
1. 自機の位置によって次のステージを選びます(障害物に注意)
1. 全てのボスを倒せばゲームクリア

## 実装予定
- ボスの追加
- ラウンドクリア時のタイムボーナス
- 効果音の追加
- スコアランキングの保存と読み込み
- 自機のミサイルとパワーアップは...作るかもしれないし、しないかも。

## 謝辞
**[@KojiSaito](https://twitter.com/kojisaito)** M5Stack 向けプログラミングをしているのを見て私も M5Stack を買ってしまいました。M5Stack への道を開いてくれたことに感謝します。  
**[@Lovyan03](https://github.com/lovyan03)** クールで有用なライブラリを作り続けている御仁。 LovyanGFX があったからこそゲームを作ることができました。  

開発中、様々な助言を頂いた皆様にも感謝申し上げます。  


Have a happy coding :)

