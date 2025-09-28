// DEV_Config.h

#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>

                                                                          
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#if defined(ESP32)

    #define EPD_SCK_PIN   13
    #define EPD_MOSI_PIN  14
    #define EPD_CS_PIN    15
    #define EPD_RST_PIN   26
    #define EPD_DC_PIN    27
    #define EPD_BUSY_PIN  25

    #define D_9PIN  0
    #if D_9PIN
        #define EPD_PWR_PIN 33
    #endif

#elif defined(ESP8266)

    #define EPD_SCK_PIN   14
    #define EPD_MOSI_PIN  13
    #define EPD_CS_PIN    15
    #define EPD_RST_PIN   2
    #define EPD_DC_PIN    4
    #define EPD_BUSY_PIN  5

#else
    #error "Please define either ESP32 or ESP8266 before including DEV_Config.h"
#endif

#define GPIO_PIN_SET    1
#define GPIO_PIN_RESET  0

#define DEV_Digital_Write(_pin, _value)  digitalWrite(_pin, ((_value) == 0) ? LOW : HIGH)
#define DEV_Digital_Read(_pin)           digitalRead(_pin)
#define DEV_Delay_ms(__xms)              delay(__xms)

UBYTE  DEV_Module_Init(void);
void   GPIO_Mode(UWORD GPIO_Pin, UWORD Mode);
void   DEV_SPI_WriteByte(UBYTE data);
UBYTE  DEV_SPI_ReadByte(void);
void   DEV_SPI_Write_nByte(UBYTE *pData, UDOUBLE len);
void   DEV_Module_Exit(void);

#endif 
