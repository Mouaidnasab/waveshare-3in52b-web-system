// EPD_3in52b.cpp

#include "EPD_3in52b.h"
#include "Debug.h"

void EPD_3IN52B_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(200);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(200);
}

void EPD_3IN52B_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

void EPD_3IN52B_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

void EPD_3IN52B_ReadBusy(void)
{
    Debug("e-Paper busy\r\n");
    while(!DEV_Digital_Read(EPD_BUSY_PIN))
    {
        DEV_Delay_ms(20);
    }
    DEV_Delay_ms(200);
    Debug("e-Paper busy release\r\n");
}

static void EPD_3IN52B_TurnOnDisplay(void)
{
    EPD_3IN52B_SendCommand(0x12); // DISPLAY_REFRESH
    // EPD_3IN52B_SendData(0xA5);	
    EPD_3IN52B_ReadBusy();
}

void EPD_3IN52B_Init(void)
{
    EPD_3IN52B_Reset();

    EPD_3IN52B_SendCommand(0x04); //POWER ON
	DEV_Delay_ms(100);
    EPD_3IN52B_ReadBusy();

    EPD_3IN52B_SendCommand(0x00);	 
    EPD_3IN52B_SendData(0x03);	
    EPD_3IN52B_SendData(0x0D);	

    EPD_3IN52B_SendCommand(0x61);	 	
	EPD_3IN52B_SendData(0xF0);	
	EPD_3IN52B_SendData(0x01);	
	EPD_3IN52B_SendData(0x68);
	
	EPD_3IN52B_SendCommand(0x06);	 	
	EPD_3IN52B_SendData(0x2F);	
	EPD_3IN52B_SendData(0x2F);	
    EPD_3IN52B_SendData(0x2E);

    EPD_3IN52B_ReadBusy();

}

void EPD_3IN52B_Display(const UBYTE *blackimage, const UBYTE *ryimage)
{
    UWORD Width, Height;
    Width = (EPD_3IN52B_WIDTH % 8 == 0)? (EPD_3IN52B_WIDTH / 8 ): (EPD_3IN52B_WIDTH / 8 + 1);
    Height = EPD_3IN52B_HEIGHT;

    EPD_3IN52B_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_3IN52B_SendData(~blackimage[i + j * Width]);
        }
    }

    EPD_3IN52B_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_3IN52B_SendData(~ryimage[i + j * Width]);
        }
    }

    EPD_3IN52B_TurnOnDisplay();
}


void EPD_3IN52B_Display_NUM(const UBYTE *image,UBYTE NUM)
{
    UWORD Width, Height;
    Width = (EPD_3IN52B_WIDTH % 8 == 0)? (EPD_3IN52B_WIDTH / 8 ): (EPD_3IN52B_WIDTH / 8 + 1);
    Height = EPD_3IN52B_HEIGHT;

    if (NUM == 0)
    {
        EPD_3IN52B_SendCommand(0x10);
        for (UWORD j = 0; j < Height; j++) {
            for (UWORD i = 0; i < Width; i++) {
                EPD_3IN52B_SendData(~image[i + j * Width]);
            }
        }
    }

    

    if (NUM != 0)
    {
        EPD_3IN52B_SendCommand(0x13);
        for (UWORD j = 0; j < Height; j++) {
            for (UWORD i = 0; i < Width; i++) {
                EPD_3IN52B_SendData(~image[i + j * Width]);
            }
        }
        EPD_3IN52B_TurnOnDisplay();
    }
}

void EPD_3IN52B_Clear(void)
{
    UWORD Width, Height;
    Width = (EPD_3IN52B_WIDTH % 8 == 0)? (EPD_3IN52B_WIDTH / 8 ): (EPD_3IN52B_WIDTH / 8 + 1);
    Height = EPD_3IN52B_HEIGHT;

    EPD_3IN52B_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_3IN52B_SendData(0x00);
        }
    }

    EPD_3IN52B_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_3IN52B_SendData(0x00);
        }
    }

    EPD_3IN52B_TurnOnDisplay();
}

void EPD_3IN52B_sleep(void)
{
    EPD_3IN52B_SendCommand(0X07);  	//deep sleep
    EPD_3IN52B_SendData(0xA5);
}



