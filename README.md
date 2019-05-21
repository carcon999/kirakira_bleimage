# kirakira_bleimage
You can create an afterimage on the website and send it directly.

![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/helloworld.png)

# 概要
このソフトウェアは、[kirakira:bit](https://www.switch-science.com/catalog/3923/)を利用して、
micro:bitを左右に振ると残像が見えるプログラムです。
ブラウザでお絵かきしたデータを無線（BLE）経由でダイレクトに送ることができます。

![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/overview.png)

# 必要なもの
|名称|説明|備考|
|:---|:---|:---|
|[micro:bit](https://microbit.org/)|マイクロビット|本体|
|[kirakira:bit](https://www.switch-science.com/catalog/3923/)|LEDバー|40個の小型LED搭載|
|電源|PHコネクタ経由で3.7-5V|[リチウムイオンポリマー電池](https://www.switch-science.com/catalog/3118/)がBEST|
|microUSBケーブル|プログラム書き込み用|Arduino IDEを利用します。※MakeCodeでは動きません。|

# プログラムの書き込み
最初にデモ用のプログラムを動作させて、開発環境を整備します。
こちらの[kirabitDemo](https://github.com/carcon999/kirabitDemo/blob/master/README.md)のプログラムを手順に従って環境構築(書き込み)してください。
MakeCodeは利用せずに、Arduino IDEの開発環境が必要です。

ここからは、上記で残像表示ができた状態＋Arduino IDEのビルド環境が整っている状態とします。

1. 公開しているkirakira_bleimageのソースコードをダウンロードして任意の場所に解凍します。
1. kirabitDemoで編集したMMA8653.hとMMA8653.cppを上記解凍先にも配置します。
1. Adafruitのライブラリを一部編集します。編集の目的は、残像を綺麗に表示させるために残像表示中は、BLEの処理を完全に停止させるために停止処理を付け加えます。
1. Adafruit_Microbit.hのヘッダーファイルに３つのメソッドを追加します。ライブラリは、Windowsの場合は、通常"C:\Users\ユーザ名\Documents\Arduino\libraries\Adafruit_microbit_Library"に配置されます。
![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/diff.png)

* Adafruit_Microbit_Matrixクラスに、endメソッドと、stopTimerメソッドの２つ
```c++:Adafruit_Microbit.h
class Adafruit_Microbit_Matrix : public Adafruit_GFX {
 public:
  Adafruit_Microbit_Matrix();
  ~Adafruit_Microbit_Matrix(void);
  boolean   begin(void);
// ★メソッド追加
  void      end(void);      // <- ##### add @carcon999 #####
```

```c++:Adafruit_Microbit.h
 private:
  void startTimer();
// ★メソッド追加
  void stopTimer();      // <- ##### add @carcon999 #####

  uint8_t matrix_buffer[3][9];
};
```
* Adafruit_Microbitクラスに、endメソッドを１つ追加します。
```c++:Adafruit_Microbit.h
class Adafruit_Microbit
{
 public:
  Adafruit_Microbit_Matrix matrix;
  Adafruit_Microbit_BLESerial BTLESerial;

  void   begin(void);
// ★メソッド追加
  void   end(void);      // <- ##### add @carcon999 #####

  uint8_t   getDieTemp(void); 
};
```
5. Adafruit_Microbit.cppのソースファイルを修正します。まず、先ほど追加した３つのメソッドを追加します。
```c++:Adafruit_Microbit.cpp
// ★メソッド追加
void Adafruit_Microbit_Matrix::end(void) {    // <- add @carcon999
  clear();                                    // <- add @carcon999
  stopTimer();                                // <- add @carcon999
}                                             // <- add @carcon999
```

```c++:Adafruit_Microbit.cpp
// ★メソッド追加
void Adafruit_Microbit_Matrix::stopTimer(void)     // <- add @carcon999
{                                                  // <- add @carcon999
  NRF_TIMER2->TASKS_STOP = 1; // タイマストップ     // <- add @carcon999
  NVIC_DisableIRQ(TIMER2_IRQn);                    // <- add @carcon999
}                                                  // <- add @carcon999
```

```c++:Adafruit_Microbit.cpp
// ★メソッド追加
void Adafruit_Microbit::end(void) {     // <- add @carcon999
  matrix.end();                         // <- add @carcon999
}                                       // <- add @carcon999
```

6. Adafruit_Microbit.cppのソースファイルで３つのメソッドを修正します。
```c++:Adafruit_Microbit.cpp
Adafruit_Microbit_BLESerial::Adafruit_Microbit_BLESerial(unsigned char req, unsigned char rdy, unsigned char rst) :
  BLEPeripheral(req, rdy, rst)
{
  this->_txCount = 0;
  this->_rxHead = this->_rxTail = 0;
  this->_flushed = 0;
  Adafruit_Microbit_BLESerial::_instance = this;

  addAttribute(this->_uartService);
  addAttribute(this->_uartNameDescriptor);
  setAdvertisedServiceUuid(this->_uartService.uuid());
  addAttribute(this->_rxCharacteristic);
  addAttribute(this->_rxNameDescriptor);
// ★↓１行コメントアウト
//this->_rxCharacteristic.setEventHandler(BLEWritten, Adafruit_Microbit_BLESerial::_received);  // -> move @carcon999
  addAttribute(this->_txCharacteristic);
  addAttribute(this->_txNameDescriptor);
}
```

```c++:Adafruit_Microbit.cpp
void Adafruit_Microbit_BLESerial::begin(...) {
// ★４行追加
  this->_txCount = 0;                                                                           // <- add @carcon999
  this->_rxHead = this->_rxTail = 0;                                                            // <- add @carcon999
  this->_flushed = 0;                                                                           // <- add @carcon999
  this->_rxCharacteristic.setEventHandler(BLEWritten, Adafruit_Microbit_BLESerial::_received);  // <- add @carcon999
  
  BLEPeripheral::begin();
  #ifdef BLE_SERIAL_DEBUG
    Serial.println(F("Adafruit_Microbit_BLESerial::begin()"));
  #endif
}
```

```c++:Adafruit_Microbit.cpp
void Adafruit_Microbit_BLESerial::end() {
  this->_rxCharacteristic.setEventHandler(BLEWritten, NULL);
  this->_rxHead = this->_rxTail = 0;
  flush();
  BLEPeripheral::disconnect();

// ★１行追加
  BLEPeripheral::end();         // <- add @carcon999
}
```


公開されているソースコードをダウンロードして、ビルドします。
必要なライブラリ等は、先の[kirabitDemo](https://github.com/carcon999/kirabitDemo/blob/master/README.md)でインストール済みです


# Chrome用公開サイト
下記のURLから試すことができます。
Chromeを利用してください。
https://s3-ap-northeast-1.amazonaws.com/microbitble/imagecanvas.html

