/***************************************************************************
 **
 **    This file defines the board specific definition
 **
 **    Used with ARM IAR C/C++ Compiler and Assembler.
 **
 **    (c) Copyright IAR Systems 2005
 **
 **    $Revision: 1.4 $
 **
 ***************************************************************************/
#include "arm_comm.h"

#ifndef __BOARD_H
#define __BOARD_H

#define I_RC_OSC_FREQ   (4 MHZ)
#define MAIN_OSC_FREQ   (12 MHZ)
#define RTC_OSC_FREQ    (32768UL)

// USB Link LED
#define USB_LINK_LED_MASK   (1UL<<13)
#define USB_LINK_LED_DIR    IO0DIR
#define USB_LINK_LED_FDIR   FIO0DIR
#define USB_LINK_LED_SET    IO0SET
#define USB_LINK_LED_FSET   FIO0SET
#define USB_LINK_LED_CLR    IO0CLR
#define USB_LINK_LED_FCLR   FIO0CLR
#define USB_LINK_LED_IO     IO0PIN
#define USB_LINK_LED_FIO    FIO0PIN

// LCD Reset
#define LCD_RST_MASK        (1UL<<25)
#define LCD_RST_FDIR        FIO3DIR
#define LCD_RST_FSET        FIO3SET
#define LCD_RST_FCLR        FIO3CLR
#define LCD_RST_FIO         FIO3PIN

// LCD CS
#define LCD_CS_MASK         (1UL<<21)
#define LCD_CS_DIR          IO1DIR
#define LCD_CS_FDIR         FIO1DIR
#define LCD_CS_SET          IO1SET
#define LCD_CS_FSET         FIO1SET
#define LCD_CS_CLR          IO1CLR
#define LCD_CS_FCLR         FIO1CLR
#define LCD_CS_IO           IO1PIN
#define LCD_CS_FIO          FIO1PIN

// PCLK offset
#define WDT_PCLK_OFFSET     0
#define TIMER0_PCLK_OFFSET  2
#define TIMER1_PCLK_OFFSET  4
#define UART0_PCLK_OFFSET   6
#define UART1_PCLK_OFFSET   8
#define PWM0_PCLK_OFFSET    10
#define PWM1_PCLK_OFFSET    12
#define I2C0_PCLK_OFFSET    14
#define SPI_PCLK_OFFSET     16
#define RTC_PCLK_OFFSET     18
#define SSP1_PCLK_OFFSET    20
#define DAC_PCLK_OFFSET     22
#define ADC_PCLK_OFFSET     24
#define CAN1_PCLK_OFFSET    26
#define CAN2_PCLK_OFFSET    28
#define ACF_PCLK_OFFSET     30
#define BAT_RAM_PCLK_OFFSET 32
#define GPIO_PCLK_OFFSET    34
#define PCB_PCLK_OFFSET     36
#define I2C1_PCLK_OFFSET    38
//#define                   40
#define SSP0_PCLK_OFFSET    42
#define TIMER2_PCLK_OFFSET  44
#define TIMER3_PCLK_OFFSET  46
#define UART2_PCLK_OFFSET   48
#define UART3_PCLK_OFFSET   50
#define I2C2_PCLK_OFFSET    52
#define I2S_PCLK_OFFSET     54
#define MCI_PCLK_OFFSET     56
//#define                   58
#define PCLK_PCLK_OFFSET    60
//#define                   62




#endif /* __BOARD_H */
