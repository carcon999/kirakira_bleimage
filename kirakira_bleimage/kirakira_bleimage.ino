#include <Adafruit_Microbit.h>
#include "kirabit.h"
#include "drv_beep.h"
#include "drv_ledbar.h"
#include "drv_display.h"
#include "drv_ble.h"
#include "drv_key.h"
#include "mode_ble.h"
#include "mode_bright.h"
#include "mode_afterimg.h"

#define MD_IMAGE    (0)
#define MD_BLE      (1)

static int m_Mode = MD_IMAGE;

Adafruit_Microbit microbit;

void setup() 
{
#ifdef DEBUG_SERIAL_ON
  Serial.begin(DEF_SERIAL_SPEED);
#endif

  KEY.setup();

  BEEP.setup(DEF_BEEP_PIN);
  DSP5x5.setup(&microbit.matrix);
  BLE.setup(&microbit.BTLESerial);

  // 明るさ設定モードへ移行（ONのまま電源入れた）
  if(digitalRead(PIN_BUTTON_A) == 0){
    BEEP.tone( 4000,  500 );       // ド
    BEEP.notone();
    MODE_BRIGHT.start();
    while(true){
      MODE_BRIGHT.main();
      if(MODE_BRIGHT.is_mode_change()){
        MODE_BRIGHT.stop();
        m_Mode = MD_IMAGE;
        break;
      }
      // キースキャン処理
      KEY.scan();
    }
  }else{
    LEDBAR.setBright(1);
  }

  MODE_AFTERIMG.setup();
}

static bool mbLock = true;

void loop() 
{
  // 残像表示モード
  if(m_Mode == MD_IMAGE){
    MODE_AFTERIMG.main();

    if (KEY.is_long_pushed(kirabit::BTN::B))
    {
      MODE_BLE.start();
      m_Mode = MD_BLE;
    }
    else if(KEY.is_pushed(kirabit::BTN::A))
    {
      mbLock ^= true;
      MODE_AFTERIMG.imglock(mbLock);
    }
    else if(KEY.is_pushed(kirabit::BTN::B))
    {
      MODE_AFTERIMG.next();
    }
  }
  // BLEモード
  else if(m_Mode == MD_BLE){
    MODE_BLE.main();

    if(MODE_BLE.is_mode_change() || KEY.is_pushed(kirabit::BTN::A))
    {
      MODE_BLE.stop();
      MODE_AFTERIMG.start();
      mbLock = true;
      MODE_AFTERIMG.imglock(mbLock);
      m_Mode = MD_IMAGE;
    }
  }

  // キースキャン処理
  KEY.scan();
}
