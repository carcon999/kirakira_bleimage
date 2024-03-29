#include "kirabit.h"
#include "arduino.h"
#include "mode_ble.h"
#include "drv_ledbar.h"
#include "drv_beep.h"
#include "drv_display.h"
#include "drv_key.h"
#include "mode_afterimg.h"
#include "test_image.h"
#include "drv_accel.h"

using namespace kirabit;

// global instance
kirabit::ModeAfterimg MODE_AFTERIMG;

ModeAfterimg::ModeAfterimg()
{
}

void ModeAfterimg::setup(void)
{
  ACCEL.setup();

  LEDBAR.setup(DEF_LED_NUM, DEF_LED_PIN);

  nextimage();
}

void ModeAfterimg::start(void)
{
  ModeBase::start();
  
  _tmout = 0;
}

void ModeAfterimg::stop(void)
{
}

void ModeAfterimg::imglock(bool value)
{
  DSP5x5.begin();
  DSP5x5.lock(value);
  lock = value;
  BEEP.tone( 3000,  500 );
  BEEP.notone();
  DSP5x5.end();
}

void ModeAfterimg::main_accel(void)
{
  int dir = ACCEL.get_dir();
  
  if(dir != 0)
  {
#ifdef DEBUG_SERIAL_ON
    Serial.println(dir);
#endif
    LEDBAR.draw(dir);
    BEEP.tone(1047, 25);
    
    counter++;
    if(counter >= 10){
      if(!lock){
        nextimage();
      }
      counter = 0;
    }
  }
}

void ModeAfterimg::main_continue(void)
{
  unsigned long ms = millis();
  
  if(ms > _tmout){
    counter++;

    if(counter >= 10){
      if(!lock){
        nextimage();
      }
      counter = 0;
    }

    LEDBAR.draw(0);
    BEEP.tone(500, 25);

    _tmout = ms + 225;
  }
}

void ModeAfterimg::main(void)
{
  if(md & 0x01){
    main_continue();
  }else{
    main_accel();
  }
}
void ModeAfterimg::next(void)
{
  counter = 0;
  md++;
}

void ModeAfterimg::nextimage(void)
{
  LEDBAR.setImage(img_table[img_index].tcolor, 16, img_table[img_index].timg, img_table[img_index].width, img_table[img_index].height);
  
  img_index++;
  if(img_index >= IMAGE_NUM){
    img_index = 0;
  }

  //LEDBAR.setImageString(&str0[0][0], STR_WIDTH);
}
