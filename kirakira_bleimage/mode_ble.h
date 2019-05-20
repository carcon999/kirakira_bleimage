/*
 * mode_ble.h
 * BLEモードクラス
 * 
 */
#ifndef mode_ble_h
#define mode_ble_h

#include "modebase.h"

namespace kirabit {

// クラスの定義
// クラス名・コンストラクタ名・関数名や使用する変数名を定義します。
class ModeBle : public ModeBase
{
  public:
    ModeBle();

    void setup(void);
    void start(void);
    void stop(void);
    void main(void);
    
  private:
    void wait_connection(void);
    void receive_data(void);
    void checkdata(const uint8_t* pbuff);
  
  private:
    uint8_t _status;
};

} // namespace kirabit

extern kirabit::ModeBle MODE_BLE;

#endif  // mode_ble_h
