
#include "arduino.h"
#include "drv_display.h"
#include "nrf.h"

using namespace kirabit;

// global instance
kirabit::Display DSP5x5;

#define PRGSTAUS_MAX   26

const uint8_t progress_bmp[PRGSTAUS_MAX][5] ={
  {B00000,B00000,B00000,B00000,B00000}, // 0
  {B10000,B00000,B00000,B00000,B00000}, // 1
  {B11000,B00000,B00000,B00000,B00000}, // 2
  {B11100,B00000,B00000,B00000,B00000}, // 3
  {B11110,B00000,B00000,B00000,B00000}, // 4
  {B11111,B00000,B00000,B00000,B00000}, // 5
  {B11111,B10000,B00000,B00000,B00000}, // 6
  {B11111,B11000,B00000,B00000,B00000}, // 7
  {B11111,B11100,B00000,B00000,B00000}, // 8
  {B11111,B11110,B00000,B00000,B00000}, // 9
  {B11111,B11111,B00000,B00000,B00000}, // 10
  {B11111,B11111,B10000,B00000,B00000}, // 11
  {B11111,B11111,B11000,B00000,B00000}, // 12
  {B11111,B11111,B11100,B00000,B00000}, // 13
  {B11111,B11111,B11110,B00000,B00000}, // 14
  {B11111,B11111,B11111,B00000,B00000}, // 15
  {B11111,B11111,B11111,B10000,B00000}, // 16
  {B11111,B11111,B11111,B11000,B00000}, // 17
  {B11111,B11111,B11111,B11100,B00000}, // 18
  {B11111,B11111,B11111,B11110,B00000}, // 19
  {B11111,B11111,B11111,B11111,B00000}, // 20
  {B11111,B11111,B11111,B11111,B10000}, // 21
  {B11111,B11111,B11111,B11111,B11000}, // 22
  {B11111,B11111,B11111,B11111,B11100}, // 23
  {B11111,B11111,B11111,B11111,B11110}, // 24
  {B11111,B11111,B11111,B11111,B11111}, // 25
};

#define LEVEL_MAX      5
const uint8_t level_bmp[LEVEL_MAX][5] ={
  {B00000,B00000,B00000,B00000,B11111}, // 0
  {B00000,B00000,B00000,B11111,B11111}, // 1
  {B00000,B00000,B11111,B11111,B11111}, // 2
  {B00000,B11111,B11111,B11111,B11111}, // 3
  {B11111,B11111,B11111,B11111,B11111}  // 4
};

#define CONNECT_MAX      8
const uint8_t connect_bmp[CONNECT_MAX][5] ={
  {B00000,B00000,B00000,B00000,B00000}, // 0
  {B00000,B10000,B00001,B10000,B00000}, // 1
  {B00000,B11001,B10011,B11001,B00000}, // 2
  {B00000,B11111,B11111,B11111,B00000}, // 3
  {B00000,B11111,B11111,B11111,B00000}, // 4
  {B00000,B11001,B10011,B11001,B00000}, // 5
  {B00000,B10000,B00001,B10000,B00000}, // 6
  {B00000,B00000,B00000,B00000,B00000}, // 7
};

#define LOCK_MAX      2
const uint8_t lock_bmp[LOCK_MAX][5] ={
  {B01110,B01000,B01000,B11111,B11111}, // 0
  {B01110,B01010,B01010,B11111,B11111}, // 1
};

// 初期化処理を行う関数
// コンストラクタは　クラス名::クラス名と同じ名前で構成します
Display::Display()
{
}

void Display::setup(Adafruit_Microbit_Matrix* mtx)
{
  _mtx = mtx;
  _mtx->setRotation(2);   // 上下反転
}

void Display::begin(void)
{
  _mtx->begin();
}

void Display::end(void)
{
  _mtx->print((char*)"");
  delay(200);

  _mtx->end();  
}

void Display::print(const char* str)
{
  _mtx->print((char*)str);
}

void Display::progress(uint16_t val, uint16_t base)
{
  int pos = 0;
  
  if(base != 0){
    pos = PRGSTAUS_MAX * val / base;
  }
  if(pos >= PRGSTAUS_MAX){
    pos = PRGSTAUS_MAX - 1;
  }
  _mtx->show(&progress_bmp[pos][0]);
}

void Display::result(bool res)
{
  if(res){
    _mtx->show(Adafruit_Microbit_Matrix::HEART);
  }else{
    _mtx->show(Adafruit_Microbit_Matrix::NO);
  }
}

void Display::level(uint8_t level)
{
  _mtx->show(&level_bmp[level][0]);
}

void Display::connect(uint8_t pos)
{
  pos &= (CONNECT_MAX - 1);
  _mtx->show(&connect_bmp[pos][0]);
}

void Display::lock(bool value)
{
  _mtx->show(&lock_bmp[value][0]);
}

