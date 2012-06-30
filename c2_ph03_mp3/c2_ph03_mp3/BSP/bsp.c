/*
*********************************************************************************************************
*                                     MICRIUM BOARD SUPPORT SUPPORT
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                             NXP LPC2378
*                                                on the
*                                   IAR LPC2378-SK Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : BAN
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GLOBALS
#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

                                                                /* ---------------------- GPIO0 Pins ---------------------- */
#define  GPIO0_RD1              DEF_BIT_00
#define  GPIO0_TD1              DEF_BIT_01
#define  GPIO0_TXD0             DEF_BIT_02
#define  GPIO0_RXD0             DEF_BIT_03
#define  GPIO0_RD2              DEF_BIT_04
#define  GPIO0_TD2              DEF_BIT_05
#define  GPIO0_SSEL1            DEF_BIT_06
#define  GPIO0_SCK1             DEF_BIT_07
#define  GPIO0_MISO1            DEF_BIT_08
#define  GPIO0_MOSI1            DEF_BIT_09
#define  GPIO0_TXD2             DEF_BIT_10
#define  GPIO0_RXD2             DEF_BIT_11
#define  GPIO0_Z_OUT            DEF_BIT_12
#define  GPIO0_USB_LINK         DEF_BIT_13
#define  GPIO0_USB_CONNECT      DEF_BIT_14
#define  GPIO0_TXD1             DEF_BIT_15
#define  GPIO0_RXD1             DEF_BIT_16
#define  GPIO0_CP               DEF_BIT_17
#define  GPIO0_BUT2             DEF_BIT_18
#define  GPIO0_MCICLK           DEF_BIT_19
#define  GPIO0_MCICMD           DEF_BIT_20
#define  GPIO0_MCIPWR           DEF_BIT_21
#define  GPIO0_MCIDAT0          DEF_BIT_22
#define  GPIO0_Y_OUT            DEF_BIT_23
#define  GPIO0_X_OUT            DEF_BIT_24
#define  GPIO0_MIC_IN           DEF_BIT_25
#define  GPIO0_AOUT             DEF_BIT_26
#define  GPIO0_SDA0             DEF_BIT_27
#define  GPIO0_SCL0             DEF_BIT_28
#define  GPIO0_BUT1             DEF_BIT_29
#define  GPIO0_PHY_INT          DEF_BIT_30
#define  GPIO0_U2DP             DEF_BIT_31

                                                                /* ---------------------- GPIO1 Pins ---------------------- */
#define  GPIO1_E_TXD0           DEF_BIT_00
#define  GPIO1_E_TXD1           DEF_BIT_01
#define  GPIO1_E_TX_EN          DEF_BIT_04
#define  GPIO1_E_CRS0           DEF_BIT_08
#define  GPIO1_E_RXD0           DEF_BIT_09
#define  GPIO1_E_RXD1           DEF_BIT_10
#define  GPIO1_E_RX_ER          DEF_BIT_14
#define  GPIO1_E_RX_CLK         DEF_BIT_15
#define  GPIO1_E_MDC            DEF_BIT_16
#define  GPIO1_E_MDIO           DEF_BIT_17
#define  GPIO1_UP               DEF_BIT_18
#define  GPIO1_DOWN             DEF_BIT_19
#define  GPIO1_SCK0             DEF_BIT_20
#define  GPIO1_SSEL0            DEF_BIT_21
#define  GPIO1_RIGHT            DEF_BIT_22
#define  GPIO1_MISO0            DEF_BIT_23
#define  GPIO1_MOSI0            DEF_BIT_24
#define  GPIO1_CENTER           DEF_BIT_25
#define  GPIO1_LCD_BL           DEF_BIT_26
#define  GPIO1_LEFT             DEF_BIT_27
#define  GPIO1_G_SEL2           DEF_BIT_28
#define  GPIO1_WP               DEF_BIT_29
#define  GPIO1_VBUS             DEF_BIT_30
#define  GPIO1_AIN5             DEF_BIT_31

                                                                /* ---------------------- GPIO2 Pins ---------------------- */
#define  GPIO2_ISP_E            DEF_BIT_10
#define  GPIO2_MCIDAT1          DEF_BIT_11
#define  GPIO2_MCIDAT2          DEF_BIT_12
#define  GPIO2_MCIDAT3          DEF_BIT_13
#define  GPIO2_LCD_RST          DEF_BIT_26


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

        CPU_INT32U  VIC_SpuriousInt;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  PLL_Init         (void);
static  void  MAM_Init         (void);
static  void  GPIO_Init        (void);
static  void  VIC_Init         (void);
static  void  ADC_Init         (void);
static  void  LCD_LightInit    (void);

static  void  Tmr_TickInit     (void);


static  void  VIC_Dummy        (void);                          /* Prototypes for dummy interrupt handlers                  */
static  void  VIC_DummyWDT     (void);
static  void  VIC_DummySW      (void);
static  void  VIC_DummyDEBUGRX (void);
static  void  VIC_DummyDEBUGTX (void);
static  void  VIC_DummyTIMER0  (void);
static  void  VIC_DummyTIMER1  (void);
static  void  VIC_DummyUART0   (void);
static  void  VIC_DummyUART1   (void);
static  void  VIC_DummyPWM01   (void);
static  void  VIC_DummyI2C0    (void);
static  void  VIC_DummySPI     (void);
static  void  VIC_DummySSP1    (void);
static  void  VIC_DummyPLL     (void);
static  void  VIC_DummyRTC     (void);
static  void  VIC_DummyEINT0   (void);
static  void  VIC_DummyEINT1   (void);
static  void  VIC_DummyEINT2   (void);
static  void  VIC_DummyEINT3   (void);
static  void  VIC_DummyAD0     (void);
static  void  VIC_DummyI2C1    (void);
static  void  VIC_DummyBOD     (void);
static  void  VIC_DummyETHERNET(void);
static  void  VIC_DummyUSB     (void);
static  void  VIC_DummyCAN01   (void);
static  void  VIC_DummyMMC     (void);
static  void  VIC_DummyGP_DMA  (void);
static  void  VIC_DummyTIMER2  (void);
static  void  VIC_DummyTIMER3  (void);
static  void  VIC_DummyUART2   (void);
static  void  VIC_DummyUART3   (void);
static  void  VIC_DummyI2C2    (void);
static  void  VIC_DummyI2S     (void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    PLL_Init();                                                 /* Initialize the PLL                                       */
    MAM_Init();                                                 /* Initialize the Memory Acceleration Module                */
//??leave them as initially setup    GPIO_Init();                                                /* Initialize the board's I/Os                              */
    ADC_Init();                                                 /* Initialize the board's ADC                               */
    LCD_LightInit();                                            /* Initialize the PWM for the LCD backlight                 */
    VIC_Init();                                                 /* Initialize the Vectored Interrupt Controller             */

    Tmr_TickInit();                                             /* Initialize the uC/OS-II tick interrupt                   */
}


/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Get the CPU clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    CPU_INT32U  msel;
    CPU_INT32U  nsel;
    CPU_INT32U  fin;
    CPU_INT32U  pll_clk_feq;                                    /* When the PLL is enabled, this is Fcco                    */
    CPU_INT32U  clk_div;
    CPU_INT32U  clk_freq;


    switch (SCB_READREG8(CLKSRCSEL) & 0x03) {                                 /* Determine the current clock source                       */
        case 0:
             fin = IRC_OSC_FRQ;
             break;

        case 1:
             fin = MAIN_OSC_FRQ;
             break;

        case 2:
             fin = RTC_OSC_FRQ;
             break;

        default:
             fin = IRC_OSC_FRQ;
             break;
    }

    if ((SCB_READREG32(PLLSTAT) & (1 << 25)) > 0) {                                /* If the PLL is currently enabled and connected        */
        msel        = (CPU_INT32U)(SCB_READREG32(PLLSTAT) & 0x3FFF) + 1;           /* Obtain the PLL multiplier                            */
        nsel        = (CPU_INT32U)((SCB_READREG32(PLLSTAT) >>   16) & 0x0F) + 1;   /* Obtain the PLL divider                               */
        pll_clk_feq = (2 * msel * (fin / nsel));                    /* Compute the PLL output frequency                     */
    } else {
        pll_clk_feq = (fin);                                        /* The PLL is bypassed                                  */
    }

    clk_div         = (CPU_INT32U)(SCB_READREG32(CCLKCFG) & 0xFF) + 1;             /* Obtain the CPU core clock divider                    */
    clk_freq        = (CPU_INT32U)(pll_clk_feq / clk_div);          /* Compute the ARM Core clock frequency                 */

    return (clk_freq);
}

/*
*********************************************************************************************************
*                                            BSP_CPU_PclkFreq()
*
* Description : Get the peripheral clock frequency for a specific peripheral.
*
* Argument(s) : pclk        The peripheral clock ID, one of PCLK_??? defined in bsp.h.
*
* Return(s)   : The peripheral's clock in Hz
*********************************************************************************************************
*/


CPU_INT32U  BSP_CPU_PclkFreq (CPU_INT08U  pclk)
{
    CPU_INT32U  clk_freq;
    CPU_INT32U  selection;


    clk_freq = BSP_CPU_ClkFreq();

    switch (pclk) {
        case PCLKINDX_WDT:
        case PCLKINDX_TIMER0:
        case PCLKINDX_TIMER1:
        case PCLKINDX_UART0:
        case PCLKINDX_UART1:
        case PCLKINDX_PWM0:
        case PCLKINDX_PWM1:
        case PCLKINDX_I2C0:
        case PCLKINDX_SPI:
        case PCLKINDX_RTC:
        case PCLKINDX_SSP1:
        case PCLKINDX_DAC:
        case PCLKINDX_ADC:
        case PCLKINDX_CAN1:
        case PCLKINDX_CAN2:
        case PCLKINDX_ACF:
             selection = ((SCB_READREG32(PCLKSEL0) >> (pclk * 2)) & 0x03);
             if (selection == 0) {
                 return (clk_freq / 4);
             } else if (selection == 1) {
                 return (clk_freq);
             } else if (selection == 2) {
                 return (clk_freq / 2);
             } else {
                 return (clk_freq / 8);
             }

        case PCLKINDX_BAT_RAM:
        case PCLKINDX_GPIO:
        case PCLKINDX_PCB:
        case PCLKINDX_I2C1:
        case PCLKINDX_SSP0:
        case PCLKINDX_TIMER2:
        case PCLKINDX_TIMER3:
        case PCLKINDX_UART2:
        case PCLKINDX_UART3:
        case PCLKINDX_I2C2:
        case PCLKINDX_MCI:
        case PCLKINDX_SYSCON:
             selection = ((SCB_READREG32(PCLKSEL1) >> ((pclk - 16) * 2)) & 0x03);
             if (selection == 0) {
                 return (clk_freq / 4);
             } else if (selection == 1) {
                 return (clk_freq);
             } else if (selection == 2) {
                 return (clk_freq / 2);
             } else {
                 return (clk_freq / 8);
             }

        default:
             return (0);
    }
}


/*
*********************************************************************************************************
*                                          OS_CPU_ExceptHndlr()
*
* Description : Handle any exceptions.
*
* Argument(s) : except_id     ARM exception type:
*
*                                  OS_CPU_ARM_EXCEPT_RESET             0x00
*                                  OS_CPU_ARM_EXCEPT_UNDEF_INSTR       0x01
*                                  OS_CPU_ARM_EXCEPT_SWI               0x02
*                                  OS_CPU_ARM_EXCEPT_PREFETCH_ABORT    0x03
*                                  OS_CPU_ARM_EXCEPT_DATA_ABORT        0x04
*                                  OS_CPU_ARM_EXCEPT_ADDR_ABORT        0x05
*                                  OS_CPU_ARM_EXCEPT_IRQ               0x06
*                                  OS_CPU_ARM_EXCEPT_FIQ               0x07
*
* Return(s)   : none.
*
* Caller(s)   : OS_CPU_ARM_EXCEPT_HANDLER(), which is declared in os_cpu_a.s.
*********************************************************************************************************
*/

void  OS_CPU_ExceptHndlr (CPU_DATA except_id)
{
    CPU_FNCT_VOID  pfnct;
    CPU_INT32U    *sp;

                                                                        /* If this exception is either an IRQ or FIQ        */
    if ((except_id == OS_CPU_ARM_EXCEPT_IRQ) || (except_id == OS_CPU_ARM_EXCEPT_FIQ)) {
        pfnct = (CPU_FNCT_VOID)READREG32(VICADDRESS);                              /* Read the interrupt vector from the VIC           */
        if (pfnct != (CPU_FNCT_VOID)0) {                                /* Make sure we don't have a NULL pointer           */
          (*pfnct)();                                                   /* Execute the ISR for the interrupting device      */
            WRITEREG32(VICADDRESS, 1);                                             /* Acknowlege the VIC interrupt                     */
        }
    } else {
        sp = (CPU_INT32U *)OSTCBCur->OSTCBStkPtr;
        APP_TRACE_INFO(("\nCPU_ARM_EXCEPTION #%d trapped.\n", except_id));
        APP_TRACE_INFO(("R0  : 0x%08x\n", *(sp + 0x01)));
        APP_TRACE_INFO(("R1  : 0x%08x\n", *(sp + 0x02)));
        APP_TRACE_INFO(("R2  : 0x%08x\n", *(sp + 0x03)));
        APP_TRACE_INFO(("R3  : 0x%08x\n", *(sp + 0x04)));
        APP_TRACE_INFO(("R4  : 0x%08x\n", *(sp + 0x05)));
        APP_TRACE_INFO(("R5  : 0x%08x\n", *(sp + 0x06)));
        APP_TRACE_INFO(("R6  : 0x%08x\n", *(sp + 0x07)));
        APP_TRACE_INFO(("R7  : 0x%08x\n", *(sp + 0x08)));
        APP_TRACE_INFO(("R8  : 0x%08x\n", *(sp + 0x09)));
        APP_TRACE_INFO(("R9  : 0x%08x\n", *(sp + 0x0A)));
        APP_TRACE_INFO(("R10 : 0x%08x\n", *(sp + 0x0B)));
        APP_TRACE_INFO(("R11 : 0x%08x\n", *(sp + 0x0C)));
        APP_TRACE_INFO(("R12 : 0x%08x\n", *(sp + 0x0D)));
        APP_TRACE_INFO(("SP  : 0x%08x\n",   (unsigned int)sp));
        APP_TRACE_INFO(("LR  : 0x%08x\n", *(sp + 0x0E)));
        APP_TRACE_INFO(("PC  : 0x%08x\n", *(sp + 0x0F)));
        APP_TRACE_INFO(("CPSR: 0x%08x\n", *(sp + 0x00)));

                                                                /* Infinite loop on other exceptions.                       */
                                                                /* Should be replaced by other behavior (reboot, etc.)      */
        while (TRUE) {
            ;
        }
    }
}


/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    WRITEREG32(VICINTENCLEAR, 0xFFFFFFFFL);                                /* Disable ALL interrupts                                   */
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                       PB, JOYSTICK AND ADC FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           PB_GetStatus()
*
* Description : Get the status of a push button on the board.
*
* Argument(s) : pb      The ID of the push button to probe
*
*                       1    probe the push button B1
*                       2    probe the push button B2
*
* Return(s)   : DEF_FALSE   if the push button is pressed
*               DEF_TRUE    if the push button is not pressed
*********************************************************************************************************
*/

CPU_BOOLEAN  PB_GetStatus (CPU_INT08U pb)
{
    CPU_BOOLEAN  status;


    status = FALSE;

    switch (pb) {
        case 1:
             if ((READREG32(FIO0PIN) & GPIO0_BUT1) == 0) {
                 status = DEF_TRUE;
             }
             break;

        case 2:
             if ((READREG32(FIO0PIN) & GPIO0_BUT2) == 0) {
                 status = DEF_TRUE;
             }
             break;

        default:
             break;
    }

    return (status);
}

/*
*********************************************************************************************************
*                                         ADC_Init()
*
* Description : Initialize the ADC.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static  void  ADC_Init (void)
{
    CPU_INT08U  div;


    WRITEREG32(ADINTEN, 0x0000000000);                          /* Disable all ADC interrupts                               */
    WRITEREG32(AD0CR, READREG32(AD0CR) & (~(1 << 21)));                                      /* Power down the ADC                                       */
    SCB_WRITEREG32(PCONP, SCB_READREG32(PCONP) | (1 << 12));                                      /* Enable     the ADC clock                                 */

                                                                /* Configure P1.31 for ADC potentiometer input              */
    WRITEREG32(PINSEL3, READREG32(PINSEL3) | (CPU_INT32U)(0x03 << 30));
    WRITEREG32(FIO1DIR, READREG32(FIO1DIR) & (~(GPIO1_AIN5)));


    div      = (BSP_CPU_PclkFreq(PCLKINDX_ADC) / 4500000) + 1;      /* Calculate divider                                        */

                                                                /* Configure ADC ...                                        */
    WRITEREG32(AD0CR,  (0x20 <<  0)                                     /*  ... Sample/convert pin AD0.5 ...                        */
             | (div  <<  8)                                     /*  ... Set divider so sample freq. < 4.5 MHz ...           */
             | (   1 << 16)                                     /*  ... Use burst mode for continuous conversion ...        */
             | (0x04 << 17)                                     /*  ... Use 11 clocks per conversion for 10-bit accuracy ...*/
             | (   1 << 21)                                     /*  ... Power up the ADC.                                   */
             | (0x00 << 24));                                    /*  ... No start                                            */


    WRITEREG32(AD0CR, READREG32(AD0CR) | (0x01 << 24));                                    /* Start conversion                                         */
}

/*
*********************************************************************************************************
*                                         ADC_GetStatus()
*
* Description : Get the status of any ADC on the board.
*
* Argument(s) : adc     The ID of the ADC to probe
*
*                       1    probe the potentiometer
*
* Return(s)   : The 10-bit numerator of a binary fraction, where the numerator is 0x03FF.  This is equal
*               to the intput voltage divided reference voltage.
*********************************************************************************************************
*/

CPU_INT16U  ADC_GetStatus (CPU_INT08U adc)
{
    CPU_INT16U  status;


    status = 0;

    switch (adc) {
        case 1:
             status = ((READREG32(ADDR5) >> 6) & 0x03FF);
             break;

        default:
             break;
    }

    return (status);
}


/*
*********************************************************************************************************
*                                         Joystick_GetStatus()
*
* Description : Get the status of the joystick on the board.
*
* Argument(s) : none.
*
* Return(s)   : A bit-mapped indication of the direction(s) the joystick is toggled, or if the joystick
*               is pushed in.  The return value will be the bitwise OR of zero or more of the following:
*
*               JOYSTICK_CENTER     if the joystick is being pressed.
*               JOYSTICK_LEFT       if the joystick is toggled left.
*               JOYSTICK_RIGHT      if the joystick is toggled right.
*               JOYSTICK_UP         if the joystick is toggled up.
*               JOYSTICK_DOWN       if the joystick is toggled down.
*********************************************************************************************************
*/

CPU_INT08U  Joystick_GetStatus (void)
{
    CPU_INT32U   pins;
    CPU_BOOLEAN  status;

    pins   = READREG32(FIO1PIN);
    status = 0;

    if ((pins & GPIO1_LEFT  ) == 0) {
        status |=  JOYSTICK_LEFT;
    }

    if ((pins & GPIO1_RIGHT ) == 0) {
        status |=  JOYSTICK_RIGHT;
    }

    if ((pins & GPIO1_UP    ) == 0) {
        status |=  JOYSTICK_UP;
    }

    if ((pins & GPIO1_DOWN  ) == 0) {
        status |=  JOYSTICK_DOWN;
    }

    if ((pins & GPIO1_CENTER) == 0) {
        status |=  JOYSTICK_CENTER;
    }

    return (status);
}


/*
*********************************************************************************************************
*                                        LCD_LightInit()
*
* Description : Initialize the PWM used for the LCD backlight.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static  void  LCD_LightInit (void)
{
                                                                /* Configure P1.26 for PWM1                                 */
    WRITEREG32(PINSEL3, READREG32(PINSEL3) | (0x02 << 20));
    WRITEREG32(PINSEL3, READREG32(PINSEL3) &~(0x01 << 20));

    SCB_WRITEREG32(PCONP, SCB_READREG32(PCONP) | (DEF_BIT_06));                                   /* Enable PWM1 clock                                        */
    WRITEREG32(PWM1TCR, (   0 <<  3)                                    /* Disable PWM                                              */
             |  (   0 <<  0)                                    /* Disable counter                                          */
             |  (DEF_BIT_01));                                   /* Reset PWM                                                */
    WRITEREG32(PWM1CTCR, (0x00 <<  0));                                   /* Counter uses prescaler                                   */
    WRITEREG32(PWM1MCR, 2);                                              /* Reset on PWMMR0                                          */
    WRITEREG32(PWM1PCR,    (   0 <<  6)                                    /* Single-edged mode for PWM1.6                             */
             |  (DEF_BIT_14));                                   /* Enable PWM1.6 output                                     */
    WRITEREG32(PWM1PR,  0);
    WRITEREG32(PWM1MR0, 0xFF);                                           /* 8-bit resolution                                         */
    WRITEREG32(PWM1LER, (DEF_BIT_01));
    WRITEREG32(PWM1MR6, 0);
    WRITEREG32(PWM1LER, READREG32(PWM1LER) |  (DEF_BIT_06));
    WRITEREG32(PWM1TCR, READREG32(PWM1TCR) |  (DEF_BIT_03));                                   /* Enable PWM                                               */
    WRITEREG32(PWM1TCR, READREG32(PWM1TCR) & (~(DEF_BIT_01)));                                   /* Release reset                                            */
    WRITEREG32(PWM1TCR, READREG32(PWM1TCR) |  DEF_BIT_00);                                    /* Enable counting                                          */
}

/*
*********************************************************************************************************
*                                          LCD_LightSet()
*
* Description : Set the LCD backlight.
*
* Argument(s) : light       The 8-bit value that specifies the brightness of the LCD backlight.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  LCD_LightSet (CPU_INT08U  light)
{
    WRITEREG32(PWM1MR6, light);
    WRITEREG32(PWM1LER, READREG32(PWM1LER) | DEF_BIT_06);
}

/*
*********************************************************************************************************
*********************************************************************************************************
**                                          uC/OS-View FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              OSView_TmrInit()
*
* Description : Select & initialize a timer for use with uC/OS-View.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

#if (OS_VIEW_MODULE > 0)
void  OSView_TmrInit (void)
{
    T1PR  = 0;
    T1TCR = 0x00000001;                                         /* Enable the timer                                         */

}
#endif


/*
*********************************************************************************************************
*                                         OSView_TmrRd()
*
* Description : Read the current counts of a 32-bit free running timer.
*
* Argument(s) : none.
*
* Return(s)   : The 32 bit counts of the timer.
*********************************************************************************************************
*/

#if (OS_VIEW_MODULE > 0)
CPU_INT32U  OSView_TmrRd (void)
{
    return ((CPU_INT32U)T1TC);
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                             uC/Probe PLUG-IN FOR uC/OS-II FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       OSProbe_TmrInit()
*
* Description : Select & initialize a timer for use with the uC/Probe Plug-In for uC/OS-II.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN == 1)
void  OSProbe_TmrInit (void)
{
    WRITEREG32(T1PR, 0);
    WRITEREG32(T1TCR, 0x00000001);                                         /* Enable the timer                                         */

}
#endif


/*
*********************************************************************************************************
*                                        OSProbe_TmrRd()
*
* Description : Read the current counts of a 32-bit free running timer.
*
* Argument(s) : none.
*
* Return(s)   : The 32bit counts of the timer.
*********************************************************************************************************
*/

#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN == 1)
CPU_INT32U  OSProbe_TmrRd (void)
{
    return (READREG32(T1TC));
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
**                                     uC/OS-II TIMER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            Tmr_TickInit()
*
* Description : Initialize uC/OS-II's tick source.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static  void  Tmr_TickInit (void)
{
    CPU_INT32U  pclk_freq;
    CPU_INT32U  rld_cnts;

                                                                /* VIC timer #0 Initialization                              */
    WRITEREG32(VICINTSELECT, READREG32(VICINTSELECT) & ~(1 << VIC_TIMER0));                         /* Configure the timer interrupt as an IRQ source           */
    WRITEREG32(VICVECTADDR4, (CPU_INT32U)Tmr_TickISR_Handler);           /* Set the vector address                                   */
    WRITEREG32(VICINTENABLE,  (1 << VIC_TIMER0));                         /* Enable the timer interrupt source                        */

    pclk_freq     =   BSP_CPU_PclkFreq(PCLKINDX_TIMER0);        /* Get the peripheral clock frequency                       */

    rld_cnts      =   pclk_freq / OS_TICKS_PER_SEC;             /* Calculate the # of counts necessary for the OS ticker    */

    WRITEREG32(T0TCR, (1 << 1));                                  /* Disable and reset counter 0 and the prescale counter 0   */
    WRITEREG32(T0TCR, 0);                                        /* Clear the reset bit                                      */
    WRITEREG32(T0PC,  0);                                        /* Prescaler is set to no division                          */

    WRITEREG32(T0MR0, rld_cnts);
    WRITEREG32(T0MCR, 3);                                        /* Interrupt on MR0 (reset TC), stop TC                     */

    WRITEREG32(T0CCR, 0);                                        /* Capture is disabled.                                     */
    WRITEREG32(T0EMR, 0);                                        /* No external match output.                                */
    WRITEREG32(T0TCR, 1);                                        /* Enable timer 0                                           */
}


/*
*********************************************************************************************************
*                                       Tmr_TickISR_Handler()
*
* Description : Handle the timer interrupt that is used to generate TICKs for uC/OS-II.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  Tmr_TickISR_Handler (void)
{
    WRITEREG32(T0IR, 0xFF);                                                /* Clear timer #0 interrupt                                 */

    OSTimeTick();                                               /* Call uC/OS-II's OSTimeTick()                             */
}


/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                Static Board Support Initialization Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      Set the CPU Clock Frequency
*
* Description: This function sets up and activates the PLL
*
* Arguements  : None
*
* Returns     : None
*
* Notes      : 1) The PLL output frequency is calculated by the following formula:
*                     Fcco = 2 * Fin * m / n, where Fin is the PLL input clock. In
*                     this particular case, Fin is set to the Main Oscillator
*                     whose frequency is #define'd in bsp.h. M is the PLL
*                     clock multiplier. M must be written to the PLLCFG register
*                     as the desired multiplier - 1. N is the PLL clock divider
*                     and must be written to PLLCFG as the desired divider - 1.
*
*              2) Fcco must be between 250 and 550 MHz. The ARM Core clock
*                 must never exceed 72 MHz. Use cClkDiv to divide Fcco accordingly.
*
*              3) When using the USB device, you must choose Fcco as a multiple
*                 of 96 MHz, and then use usbClkDiv to divide Fcco to exactly
*                 48 MHz.
*
*              4) In this example, Fin = 12MHz, M = 12, N = 1, cClkDiv = 6 and usbClkDiv = 6.
*                 Therefore, Fcco = 2 * Fin * M / N = (2 * 12 * 12 / 1) = 288MHz.
*                 The processor clock = (Fcco / cClkDiv) = (288MHz / 6) =  48MHz.
*                 Finally, the USB clock = (Fcco / usbClkDib) = (288MHz / 6) = 48MHz.
*
*              5) Early revisions of the part have a PLL errata preventing Fcco from
*                 being greater than 288MHz.
*
*              6) For later revisions, M = 20, cCLKDiv = 8, and usbClkDiv = 10 yield
*                 60MHz for the processor clock and 48MHz for the USB clock.
*********************************************************************************************************
*/
/*
 * NAME
 * 
 *  initPLL
 * 
 * DESCRIPTION
 * 
 *  This init is specific to the 2300 family of devices.
 * 
 *  Initialize PLL (Configured for a 12.000 MHz crystal) to
 *  boost processor clock to 72.000 MHz
 * 
 * PARAMETER
 * 
 *  none...
 * 
 * EXAMPLE
 * 
 *  initPLL(); 
 *
 * NOTES
 *
 *  none...
 *
 */


static  void  PLL_Init (void)
{
#if CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL /* Allocate storage for CPU status register                 */
    CPU_SR  cpu_sr = 0;
    CPU_SR  cpu_sr0 = 0;
    CPU_SR  cpu_sr1 = 0;
    CPU_SR  cpu_sr2 = 0;
    CPU_SR  cpu_sr3 = 0;
    CPU_SR  cpu_sr4 = 0;
    CPU_SR  cpu_sr5 = 0;
#endif

    CPU_INT32U  m;
    CPU_INT32U  n;
    CPU_INT32U  clk_div;
    CPU_INT32U  clk_div_usb;


    m           = 11;                                           /* PLL Multiplier = 20, MSEL bits = 12 - 1 = 11             */
    n           =  0;                                           /* PLL Divider    =  1, NSEL bits =  1 - 1 =  0             */
    clk_div     =  5;                                           /* Configure the  ARM Core clock div to 6. CCLKSEL =  6 - 1 */
    clk_div_usb =  5;                                           /* Configure the USB clock divider to 6, USBSEL  = 6 - 1    */

    if ((SCB_READREG32(PLLSTAT) & DEF_BIT_25)) {                           /* If the PLL is already running                            */
        OS_ENTER_CRITICAL();
        cpu_sr0 = cpu_sr;
        SCB_WRITEREG32(PLLCON, DEF_BIT_01);                                 /* Disconnect the PLL                                       */
        SCB_WRITEREG32(PLLFEED, 0xAA);                                       /* PLL register update sequence, 0xAA, 0x55                 */
        SCB_WRITEREG32(PLLFEED, 0x55);
        OS_EXIT_CRITICAL();
    }

//??    CPU_CRITICAL_ENTER();
    OS_ENTER_CRITICAL();
    cpu_sr1 = cpu_sr;
    SCB_WRITEREG32(PLLCON, DEF_BIT_00);                                    /* Disable the PLL                                          */
    SCB_WRITEREG32(PLLFEED, 0xAA);                                          /* PLL register update sequence, 0xAA, 0x55                 */
    SCB_WRITEREG32(PLLFEED,  0x55);
//??    CPU_CRITICAL_EXIT();
    OS_EXIT_CRITICAL();
    SCB_WRITEREG32(SCS, SCB_READREG32(SCS) & (~DEF_BIT_04));                                    /* OSCRANGE = 0, Main OSC is between 1 and 20 Mhz           */
    SCB_WRITEREG32(SCS, SCB_READREG32(SCS) | DEF_BIT_05);                                    /* OSCEN = 1, Enable the main oscillator                    */

    while ((SCB_READREG32(SCS) &  DEF_BIT_06) == 0) {                          /* Wait until OSCSTAT is set (Main OSC ready to be used)    */
        ;
    }

    SCB_WRITEREG32(CLKSRCSEL, DEF_BIT_00);                                     /* Select main OSC, 12MHz, as the PLL clock source          */

    //??    CPU_CRITICAL_ENTER();
    OS_ENTER_CRITICAL();
    cpu_sr2 = cpu_sr;
    SCB_WRITEREG32(PLLCFG, (m << 0) | (n << 16));                           /* Configure the PLL multiplier and divider                 */
    SCB_WRITEREG32(PLLFEED, 0xAA);                                           /* PLL register update sequence, 0xAA, 0x55                 */
    SCB_WRITEREG32(PLLFEED, 0x55);
    //??    CPU_CRITICAL_EXIT();
    OS_EXIT_CRITICAL();

    //??    CPU_CRITICAL_ENTER();
    OS_ENTER_CRITICAL();
    cpu_sr3 = cpu_sr;
    SCB_WRITEREG32(PLLCON, DEF_BIT_00);                                     /* Enable the PLL                                           */
    SCB_WRITEREG32(PLLFEED, 0xAA);                                           /* PLL register update sequence, 0xAA, 0x55                 */
    SCB_WRITEREG32(PLLFEED, 0x55);
    //??    CPU_CRITICAL_EXIT();
    OS_EXIT_CRITICAL();

    SCB_WRITEREG32(CCLKCFG, clk_div);                                        /* Configure the ARM Core Processor clock divider           */
    SCB_WRITEREG32(USBCLKCFG, clk_div_usb);                                    /* Configure the USB clock divider                          */

    while ((SCB_READREG32(PLLSTAT) & DEF_BIT_26) == 0) {                       /* Wait for PLOCK to become set                             */
        ;
    }

    SCB_WRITEREG32(PCLKSEL0, 0xAAAAAAAA);                                     /* Set peripheral clocks to be half of main clock           */
    SCB_WRITEREG32(PCLKSEL1, 0x22AAA8AA);

    //??    CPU_CRITICAL_ENTER();
    OS_ENTER_CRITICAL();
//??    WRITEREG32(PLLCON, DEF_BIT_01);                                     /* Connect the PLL. The PLL is now the active clock source  */
    cpu_sr4 = cpu_sr;
    SCB_WRITEREG32(PLLCON, DEF_BIT_01 | DEF_BIT_00);                                     /* Connect the PLL. The PLL is now the active clock source  */
    SCB_WRITEREG32(PLLFEED, 0xAA);                                           /* PLL register update sequence, 0xAA, 0x55                 */
    SCB_WRITEREG32(PLLFEED, 0x55);
    //??    CPU_CRITICAL_EXIT();
    OS_EXIT_CRITICAL();

    while ((SCB_READREG32(PLLSTAT) & DEF_BIT_25) == 0) {                       /* Wait PLLC, the PLL connect status bit to become set      */
        ;
    }
}


/*
*********************************************************************************************************
*                                       MAM_Init()
*
* Description : Initialize the Memory Acceleration Module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static  void  MAM_Init (void)
{
    CPU_INT32U  clk_freq;


    clk_freq = BSP_CPU_ClkFreq();                               /* Get the current core clock frequency                     */

    SCB_WRITEREG32(MAMCR,  0);                                               /* Disable MAM functionality                                */

    if (clk_freq < 20000000) {                                  /* Compare current clock frequency with MAM modes           */
        SCB_WRITEREG32(MAMTIM, 1);                                             /* Set MAM fetch cycles to 1 processor clock in duration    */
    }

    if (clk_freq < 40000000) {
        SCB_WRITEREG32(MAMTIM, 2);                                             /* Set MAM fetch cycles to 2 processor clock in duration    */
    }

    if (clk_freq >= 40000000) {
        SCB_WRITEREG32(MAMTIM, 3);                                             /* Set MAM fetch cycles to 3 processor clock in duration    */
    }

    SCB_WRITEREG32(MAMCR, 2);                                                  /* Enable full MAM functionality                            */
}

/*
*********************************************************************************************************
*                                          GPIO_Init()
*
* Description : Initializes the GPIO pins.  All the I/O pins are initialized in this function
*               so you don't have to look at multiple places for I/O initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) Refer to the LPC2378 User Manual, Chapter 9 for a detailed Pin Assignment
*********************************************************************************************************
*/

static  void  GPIO_Init (void)
{
    SCB_WRITEREG32(SCS, SCB_READREG32(SCS) | 1);                                           /* Enable high-speed GPIO on ports 0 and 1                  */

    WRITEREG32(IO0DIR, 0);
    WRITEREG32(IO1DIR, 0);
    WRITEREG32(FIO0DIR, 0);
    WRITEREG32(FIO1DIR, 0);
    WRITEREG32(FIO2DIR, 0);
    WRITEREG32(FIO3DIR, 0);
    WRITEREG32(FIO4DIR, 0);

    WRITEREG32(FIO0MASK, 0);
    WRITEREG32(FIO1MASK, 0);
    WRITEREG32(FIO2MASK, 0);
    WRITEREG32(FIO3MASK, 0);
    WRITEREG32(FIO4MASK, 0);

    WRITEREG32(PINSEL0, 0);
    WRITEREG32(PINSEL1, 0);
    WRITEREG32(PINSEL2, 0);
    WRITEREG32(PINSEL3, 0);
    WRITEREG32(PINSEL4, 0);
    WRITEREG32(PINSEL5, 0);
    WRITEREG32(PINSEL6, 0);
    WRITEREG32(PINSEL7, 0);
    WRITEREG32(PINSEL8, 0);
    WRITEREG32(PINSEL9, 0);
    WRITEREG32(PINSEL10, 0);

                                                                /* Configure P0.18 for push button B1                       */
    WRITEREG32(PINSEL1, READREG32(PINSEL1) & (~(0x03 <<  4)));
    WRITEREG32(FIO0DIR, READREG32(FIO0DIR) & (~(GPIO0_BUT1)));

                                                                /* Configure P0.29 for push button B2                       */
    WRITEREG32(PINSEL1, READREG32(PINSEL1) & (~(0x03 << 26)));
    WRITEREG32(FIO0DIR, READREG32(FIO0DIR) & (~(GPIO0_BUT2)));

                                                                /* Configure P1.18 for joystick UP                          */
    WRITEREG32(PINSEL1, READREG32(PINSEL1) & (~(0x03 <<  4)));
    WRITEREG32(FIO0DIR, READREG32(FIO0DIR) & (~(GPIO1_UP)));

                                                                /* Configure P1.19 for joystick DOWN                        */
    WRITEREG32(PINSEL1, READREG32(PINSEL1) & (~(0x03 <<  6)));
    WRITEREG32(FIO0DIR, READREG32(FIO0DIR) & (~(GPIO1_DOWN)));

                                                                /* Configure P1.22 for joystick RIGHT                       */
    WRITEREG32(PINSEL1, READREG32(PINSEL1) & (~(0x03 << 12)));
    WRITEREG32(FIO0DIR, READREG32(FIO0DIR) & (~(GPIO1_RIGHT)));

                                                                /* Configure P1.25 for joystick CENTER                      */
    WRITEREG32(PINSEL1, READREG32(PINSEL1) & (~(0x03 << 18)));
    WRITEREG32(FIO0DIR, READREG32(FIO0DIR) & (~(GPIO1_CENTER)));

                                                                /* Configure P1.27 for joystick LEFT                        */
    WRITEREG32(PINSEL1, READREG32(PINSEL1) & (~(0x03 << 22)));
    WRITEREG32(FIO0DIR, READREG32(FIO0DIR) &= (~(GPIO1_LEFT)));
}

/*
*********************************************************************************************************
*                                     VIC_Init()
*
* Description : Initialize the Vectored Interrupt Controller
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static  void  VIC_Init (void)
{
    WRITEREG32(VICINTENCLEAR, 0xFFFFFFFF);                                /* Disable ALL interrupts                                   */
    WRITEREG32(VICADDRESS, 0);                                         /* Acknowlege any pending VIC interrupt                     */
    WRITEREG32(VICPROTECTION, 0);                                         /* Allow VIC register access in User of Priviledged modes   */

    WRITEREG32(VICVECTADDR0, (CPU_INT32U)VIC_DummyWDT);                   /* Set the vector address                                   */
    WRITEREG32(VICVECTADDR1, (CPU_INT32U)VIC_DummySW);
    WRITEREG32(VICVECTADDR2, (CPU_INT32U)VIC_DummyDEBUGRX);
    WRITEREG32(VICVECTADDR3, (CPU_INT32U)VIC_DummyDEBUGTX);
    WRITEREG32(VICVECTADDR4, (CPU_INT32U)VIC_DummyTIMER0);
    WRITEREG32(VICVECTADDR5, (CPU_INT32U)VIC_DummyTIMER1);
    WRITEREG32(VICVECTADDR6, (CPU_INT32U)VIC_DummyUART0);
    WRITEREG32(VICVECTADDR7, (CPU_INT32U)VIC_DummyUART1);
    WRITEREG32(VICVECTADDR8, (CPU_INT32U)VIC_DummyPWM01);
    WRITEREG32(VICVECTADDR9, (CPU_INT32U)VIC_DummyI2C0);
    WRITEREG32(VICVECTADDR10, (CPU_INT32U)VIC_DummySPI);
    WRITEREG32(VICVECTADDR11, (CPU_INT32U)VIC_DummySSP1);
    WRITEREG32(VICVECTADDR12, (CPU_INT32U)VIC_DummyPLL);
    WRITEREG32(VICVECTADDR13, (CPU_INT32U)VIC_DummyRTC);
    WRITEREG32(VICVECTADDR14, (CPU_INT32U)VIC_DummyEINT0);
    WRITEREG32(VICVECTADDR15, (CPU_INT32U)VIC_DummyEINT1);
    WRITEREG32(VICVECTADDR16, (CPU_INT32U)VIC_DummyEINT2);
    WRITEREG32(VICVECTADDR17, (CPU_INT32U)VIC_DummyEINT3);
    WRITEREG32(VICVECTADDR18, (CPU_INT32U)VIC_DummyAD0);
    WRITEREG32(VICVECTADDR19, (CPU_INT32U)VIC_DummyI2C1);
    WRITEREG32(VICVECTADDR20, (CPU_INT32U)VIC_DummyBOD);
    WRITEREG32(VICVECTADDR21, (CPU_INT32U)VIC_DummyETHERNET);
    WRITEREG32(VICVECTADDR22, (CPU_INT32U)VIC_DummyUSB);
    WRITEREG32(VICVECTADDR23, (CPU_INT32U)VIC_DummyCAN01);
    WRITEREG32(VICVECTADDR24, (CPU_INT32U)VIC_DummyMMC);
    WRITEREG32(VICVECTADDR25, (CPU_INT32U)VIC_DummyGP_DMA);
    WRITEREG32(VICVECTADDR26, (CPU_INT32U)VIC_DummyTIMER2);
    WRITEREG32(VICVECTADDR27, (CPU_INT32U)VIC_DummyTIMER3);
    WRITEREG32(VICVECTADDR28, (CPU_INT32U)VIC_DummyUART2);
    WRITEREG32(VICVECTADDR29, (CPU_INT32U)VIC_DummyUART3);
    WRITEREG32(VICVECTADDR30, (CPU_INT32U)VIC_DummyI2C2);
    WRITEREG32(VICVECTADDR31, (CPU_INT32U)VIC_DummyI2S);
    

}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                       DUMMY INTERRUPT HANDLERS
*********************************************************************************************************
*********************************************************************************************************
*/

static  void  VIC_Dummy (void)
{
    while (DEF_TRUE) {
        ;
    }
}

static  void  VIC_DummyWDT (void)
{
    VIC_SpuriousInt = VIC_WDT;
    VIC_Dummy();
}

static  void  VIC_DummySW (void)
{
    VIC_SpuriousInt = VIC_SW;
    VIC_Dummy();
}

static  void  VIC_DummyDEBUGRX (void)
{
    VIC_SpuriousInt = VIC_DEBUGRX;
    VIC_Dummy();
}

static  void  VIC_DummyDEBUGTX (void)
{
    VIC_SpuriousInt = VIC_DEBUGTX;
    VIC_Dummy();
}

static  void  VIC_DummyTIMER0 (void)
{
    VIC_SpuriousInt = VIC_TIMER0;
    VIC_Dummy();
}

static  void  VIC_DummyTIMER1 (void)
{
    VIC_SpuriousInt = VIC_TIMER1;
    VIC_Dummy();
}

static  void  VIC_DummyUART0 (void)
{
    VIC_SpuriousInt = VIC_UART0;
    VIC_Dummy();
}

static  void  VIC_DummyUART1 (void)
{
    VIC_SpuriousInt = VIC_UART1;
    VIC_Dummy();
}

static  void  VIC_DummyPWM01 (void)
{
    VIC_SpuriousInt = VIC_PWM01;
    VIC_Dummy();
}

static  void  VIC_DummyI2C0 (void)
{
    VIC_SpuriousInt = VIC_I2C0;
    VIC_Dummy();
}

static  void  VIC_DummySPI (void)
{
    VIC_SpuriousInt = VIC_SPI;
    VIC_Dummy();
}

static  void  VIC_DummySSP1 (void)
{
    VIC_SpuriousInt = VIC_SSP1;
    VIC_Dummy();
}

static  void  VIC_DummyPLL (void)
{
    VIC_SpuriousInt = VIC_PLL;
    VIC_Dummy();
}

static  void  VIC_DummyRTC (void)
{
    VIC_SpuriousInt = VIC_RTC;
    VIC_Dummy();
}

static  void  VIC_DummyEINT0 (void)
{
    VIC_SpuriousInt = VIC_EINT0;
    VIC_Dummy();
}

static  void  VIC_DummyEINT1 (void)
{
    VIC_SpuriousInt = VIC_EINT1;
    VIC_Dummy();
}

static  void  VIC_DummyEINT2 (void)
{
    VIC_SpuriousInt = VIC_EINT2;
    VIC_Dummy();
}

static  void  VIC_DummyEINT3 (void)
{
    VIC_SpuriousInt = VIC_EINT3;
    VIC_Dummy();
}

static  void  VIC_DummyAD0 (void)
{
    VIC_SpuriousInt = VIC_AD0;
    VIC_Dummy();
}

static  void  VIC_DummyI2C1 (void)
{
    VIC_SpuriousInt = VIC_I2C1;
    VIC_Dummy();
}

static  void  VIC_DummyBOD (void)
{
    VIC_SpuriousInt = VIC_BOD;
    VIC_Dummy();
}

static  void  VIC_DummyETHERNET (void)
{
    VIC_SpuriousInt = VIC_ETHERNET;
    VIC_Dummy();
}

static  void  VIC_DummyUSB (void)
{
    VIC_SpuriousInt = VIC_USB;
    VIC_Dummy();
}

static  void  VIC_DummyCAN01 (void)
{
    VIC_SpuriousInt = VIC_CAN12;
    VIC_Dummy();
}

static  void  VIC_DummyMMC (void)
{
    VIC_SpuriousInt = VIC_MMC;
    VIC_Dummy();
}

static  void  VIC_DummyGP_DMA (void)
{
    VIC_SpuriousInt = VIC_GP_DMA;
    VIC_Dummy();
}

static  void  VIC_DummyTIMER2 (void)
{
    VIC_SpuriousInt = VIC_TIMER2;
    VIC_Dummy();
}

static  void  VIC_DummyTIMER3 (void)
{
    VIC_SpuriousInt = VIC_TIMER3;
    VIC_Dummy();
}

static  void  VIC_DummyUART2 (void)
{
    VIC_SpuriousInt = VIC_UART2;
    VIC_Dummy();
}

static  void  VIC_DummyUART3 (void)
{
    VIC_SpuriousInt = VIC_UART3;
    VIC_Dummy();
}

static  void  VIC_DummyI2C2 (void)
{
    VIC_SpuriousInt = VIC_I2C2;
    VIC_Dummy();
}

static  void  VIC_DummyI2S (void)
{
    VIC_SpuriousInt = VIC_I2S;
    VIC_Dummy();
}
