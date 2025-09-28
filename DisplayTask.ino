// DisplayTask.ino

#include "EPD_3in52b.h"

#ifdef ESP32
  #include <freertos/FreeRTOS.h>
  #include <freertos/task.h>
#endif

extern uint8_t uploaded_bw[];
extern uint8_t uploaded_red[];
extern const size_t BUF_SZ;



static uint8_t reverseByte(uint8_t b) {
  b = (b & 0xF0)>>4 | (b & 0x0F)<<4;
  b = (b & 0xCC)>>2 | (b & 0x33)<<2;
  b = (b & 0xAA)>>1 | (b & 0x55)<<1;
  return b;
}



void displayImageTask(void* /*pvParameters*/) {


  // display
  EPD_3IN52B_Init();
  EPD_3IN52B_Display(uploaded_bw, uploaded_red);
  EPD_3IN52B_sleep();
  DEV_Delay_ms(200);




}
