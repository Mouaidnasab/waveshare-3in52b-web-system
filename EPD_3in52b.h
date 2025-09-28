// EPD_3in52b.h

#ifndef __EPD_3IN52B_H_
#define __EPD_3IN52B_H_

#include "DEV_Config.h"

#define EPD_3IN52B_WIDTH       240
#define EPD_3IN52B_HEIGHT      360 

#define LUTGC_TEST          //
#define LUTDU_TEST          //

#define EPD_3IN52B_WHITE                         0xFF  // 
#define EPD_3IN52B_BLACK                         0x00  //
#define EPD_3IN52B_Source_Line                   0xAA  //
#define EPD_3IN52B_Gate_Line                     0x55  //
#define EPD_3IN52B_UP_BLACK_DOWN_WHITE           0xF0  //
#define EPD_3IN52B_LEFT_BLACK_RIGHT_WHITE        0x0F  //
#define EPD_3IN52B_Frame                         0x01  // 
#define EPD_3IN52B_Crosstalk                     0x02  //
#define EPD_3IN52B_Chessboard                    0x03  //
#define EPD_3IN52B_Image                         0x04  //


extern unsigned char EPD_3IN52B_Flag;

void EPD_3IN52B_SendCommand(UBYTE Reg);
void EPD_3IN52B_SendData(UBYTE Data);
void EPD_3IN52B_Init(void);
void EPD_3IN52B_Display(const UBYTE *blackimage, const UBYTE *ryimage);
void EPD_3IN52B_Display_NUM(const UBYTE *image,UBYTE NUM);
void EPD_3IN52B_Clear(void);
void EPD_3IN52B_sleep(void);



#endif
