/*
 * drv_ble.h
 * BLEクラス
 * 
 * 
 */
#ifndef drv_ble_h
#define drv_ble_h

#include "Adafruit_Microbit.h"

#define BLE_UART_MAX   19

namespace kirabit {

// クラスの定義
// クラス名・コンストラクタ名・関数名や使用する変数名を定義します。
class Ble
{
  public:
    Ble() ;
    void setup(Adafruit_Microbit_BLESerial* ble);
    void begin(void);
    void end(void);
    bool is_connected(void);
    void poll(void);
    size_t write(uint8_t byte);
    bool is_buffer_full(void);
    void buffer_clear(void);
    const uint8_t * get_buffer(void);

  private:
    Adafruit_Microbit_BLESerial* _ble;
    uint8_t _buff[BLE_UART_MAX];
    uint8_t _index;
};

} // namespace kirabit

extern kirabit::Ble BLE;

#endif  // drv_ble_h
