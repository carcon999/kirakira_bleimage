# プログラムの修正手順

* 前提条件
こちらの[kirabitDemo](https://github.com/carcon999/kirabitDemo/blob/master/README.md)のプログラムが正常に動作した環境であるとします。※そのプログラムが書き込み済みである必要はありません。

1. 公開しているkirakira_bleimageのソースコードをダウンロードして任意の場所に解凍します。
1. [kirabitDemo](https://github.com/carcon999/kirabitDemo/blob/master/README.md)で編集したMMA8653.hとMMA8653.cppを上記解凍先にも配置（コピー）します。
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

7. kirakira_bleimage.inoをArduino IDEで開きます。
7. 念のため、Arduino IDEのToolメニューが以下であることを確認します。赤枠の項目
![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/arduino_env.png)
7. Arduino IDEの「コンパイル・検証」を選択し、エラーが無いことを確認します。エラーが発生する場合は、エラー内容に従って修正してください。
7. Arduino IDEの「Tool」のメニューから「nRF5 Flash SoftDevice」を選択しSoftDeviceを書き込みます。
7. Arduino IDEの「スケッチ」のメニューから「マイコンボードに書き込む」を選択しコンパイル済みのプログラムを書き込みます。
7. 正常に書き込めれば終了です。下記のようなメッセージが出ているかもしれません。
![Image](https://github.com/carcon999/kirakira_bleimage/blob/master/img/wfinish.png)

お疲れさまでした。
