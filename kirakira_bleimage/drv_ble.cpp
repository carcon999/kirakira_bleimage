
#include "arduino.h"
#include "drv_ble.h"
#include "nrf.h"

using namespace kirabit;

// global instance
kirabit::Ble BLE;


// 初期化処理を行う関数
// コンストラクタは　クラス名::クラス名と同じ名前で構成します
Ble::Ble()
{
}

void Ble::setup(Adafruit_Microbit_BLESerial* ble)
{
  _ble = ble;
}

void Ble::begin(void)
{
#ifdef DEBUG_SERIAL_ON
  Serial.println("BLE begin");
#endif
  _ble->setLocalName("kirakirabit");
  _ble->begin();
  buffer_clear();
}

void Ble::end(void)
{
#ifdef DEBUG_SERIAL_ON
  Serial.println("BLE end");
#endif
  _ble->end();
}

bool Ble::is_connected(void)
{
  return *_ble;
}

void Ble::poll(void)
{
  _ble->poll();

  // この条件文は重要！このアクセスが無いと固まる。2018/05/10現在
  if(is_connected())
  {
    // データ読み込み処理
    while(_index < BLE_UART_MAX && _ble->available())
    {
      _buff[_index++] = _ble->read();
    }    
  }
}

size_t Ble::write(uint8_t byte)
{
  _ble->write(byte);
}

bool Ble::is_buffer_full(void)
{
  return (_index >= BLE_UART_MAX);
}

void Ble::buffer_clear(void)
{
  _index = 0;
}

const uint8_t* Ble::get_buffer(void)
{
  return _buff;
}



