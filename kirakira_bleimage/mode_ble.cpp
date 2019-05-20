
#include "arduino.h"
#include "mode_ble.h"
#include "drv_beep.h"
#include "drv_display.h"
#include "drv_ble.h"
#include "drv_ledbar.h"

using namespace kirabit;

// global instance
kirabit::ModeBle MODE_BLE;

static uint8_t ble_status = 0;
static uint16_t img_width;
static uint16_t img_height;
static uint16_t data_len;
static uint16_t receive_size;
static uint8_t img_checksum;
static uint8_t sum;
static unsigned long tm_conn;
static uint8_t conn_pos = 0;

ModeBle::ModeBle()
{
}

void ModeBle::setup(void)
{
  
}

void ModeBle::start(void)
{
  ModeBase::start();
  
#ifdef DEBUG_SERIAL_ON
  Serial.println("* start BLE");
#endif

  BEEP.tone( 659, 107 );       // ソ
  BEEP.tone( 740,  95 );       // ラ
  BEEP.tone( 830,  85 );       // シ
  BEEP.tone( 880,  80 );       // ド
  BEEP.notone();

  BLE.begin();
  BLE.poll();
  
  // Start LED matrix driver after radio (required)
  DSP5x5.begin();
  conn_pos = 0;
  DSP5x5.connect(conn_pos);
  tm_conn = millis();

  ble_status = 0;
  _status = 0;
}

void ModeBle::stop(void)
{
  BEEP.tone( 587, 120 );       // ファ
  BEEP.tone( 554, 128 );       // ミ
  BEEP.tone( 494, 143 );       // レ
  BEEP.tone( 440, 161 );       // ド
  
  BLE.end();
  DSP5x5.end();      
}

void ModeBle::checkdata(const uint8_t* pbuff)
{
  // 初期レコード
  if(ble_status == 0){
#ifdef DEBUG_SERIAL_ON
      Serial.println("status 0");
#endif
      
    // フォーマット確認
    if(*(pbuff+0) == 'K' && *(pbuff+1) == 'I'){
      img_width = *(pbuff+2);
#ifdef DEBUG_SERIAL_ON
      Serial.print("width:");
      Serial.println(img_width);
#endif
      img_height = *(pbuff+3);
#ifdef DEBUG_SERIAL_ON
      Serial.print("height:");
      Serial.println(img_height);
#endif
      data_len = ((uint16_t)*(pbuff+4) << 8) + *(pbuff+5);
#ifdef DEBUG_SERIAL_ON
      Serial.print("len:");
      Serial.println(data_len);
#endif
      img_checksum = *(pbuff+6);
#ifdef DEBUG_SERIAL_ON
      Serial.print("check sum:");
      Serial.println(img_checksum);
#endif

      LEDBAR.startBle(img_width, img_height);
      
      LEDBAR.setColorTable(0, pbuff+7);
      LEDBAR.setColorTable(1, pbuff+10);
      LEDBAR.setColorTable(2, pbuff+13);
      LEDBAR.setColorTable(3, pbuff+16);

      receive_size = 12;
      ble_status = 1;
    }
    
  }else if(ble_status == 1){
    // 色データ２
    receive_size += BLE_UART_MAX;
#ifdef DEBUG_SERIAL_ON
    Serial.println("status 1");
#endif
    ble_status = 2;

    LEDBAR.setColorTable(4, pbuff+0);
    LEDBAR.setColorTable(5, pbuff+3);
    LEDBAR.setColorTable(6, pbuff+6);
    LEDBAR.setColorTable(7, pbuff+9);
    LEDBAR.setColorTable(8, pbuff+12);
    LEDBAR.setColorTable(9, pbuff+15);
    
  }else if(ble_status == 2){
    // 色データ３
    receive_size += BLE_UART_MAX;
#ifdef DEBUG_SERIAL_ON
    Serial.println("status 2");
#endif
    ble_status = 3;
    sum = 0;

    LEDBAR.setColorTable(10, pbuff+0);
    LEDBAR.setColorTable(11, pbuff+3);
    LEDBAR.setColorTable(12, pbuff+6);
    LEDBAR.setColorTable(13, pbuff+9);
    LEDBAR.setColorTable(14, pbuff+12);
    LEDBAR.setColorTable(15, pbuff+15);

    LEDBAR.startBle(img_width, img_height);

  }else if(ble_status == 3){

#ifdef DEBUG_SERIAL_ON
    Serial.println("status 3");
#endif
    receive_size += BLE_UART_MAX;
    
    for(int i = 0 ; i < BLE_UART_MAX ; i++){
      sum += *(pbuff + i);
    }

    if(receive_size <= data_len){
      LEDBAR.putBle(pbuff, BLE_UART_MAX);
    }else{
      LEDBAR.putBle(pbuff, BLE_UART_MAX - (receive_size - data_len));      
    }
    
    if(receive_size >= data_len)
    {
      LEDBAR.endBle();
            
      // sumを書込み通知する。
      BLE.write(sum);

      // 結果表示
      DSP5x5.result(sum == img_checksum);
      delay(1000);

#ifdef DEBUG_SERIAL_ON
      Serial.println("status finished.");
      Serial.print("sum:");
      Serial.println(sum);
#endif

      if(sum == img_checksum){
        ble_status = 4;   // 転送完了
      }else{
        ble_status = 0;
      }
    }
  }
}

void ModeBle::wait_connection(void)
{
  BLE.poll();

  unsigned long tm = millis();

  if(tm > tm_conn + 250){
    tm_conn = tm;
    DSP5x5.connect(++conn_pos);
  }
  
  if(BLE.is_connected())
  {
    DSP5x5.connect(3);
    _status = 1;
  }
}

// BLEデータ受信モード
void ModeBle::receive_data(void)
{
  BLE.poll();
  
  // この条件文は重要！このアクセスが無いと固まる。2018/05/10現在
  if(BLE.is_connected())
  {
    if(BLE.is_buffer_full())
    {
      checkdata(BLE.get_buffer());
      BLE.buffer_clear();
      DSP5x5.progress(receive_size, data_len);
    }

  // 切断された場合は、接続待ちモードへ
  }else{
    // 転送が正常完了後の切断は自動的にモードを戻す。
    if(ble_status == 4){
      m_change = true;
    }else{
      DSP5x5.print("A");
      _status = 0;
    }
  }
}

void ModeBle::main(void)
{
  // ポーリング処理
  BLE.poll();
      
  // 接続待ち
  if(_status == 0)
  {
    wait_connection();
  }
  else
  {
    receive_data();
  }
}
