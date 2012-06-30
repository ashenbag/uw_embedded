/*++
Copyright (c) 2001  University of Washington Extension.  All rights reserved.

Module Name:

    main.c

Module Description:

    Test appilcation for UCOS-II port.
  Ported from the UCOS test app.

Revision History:

--*/



#include "includes.h"  // OS includes
#include "init.h"
#include "print.h"

// prototype for startup task
void StartupTask(void* pdata);

// allocate a stack for the startup task
static OS_STK  StartupStk[APP_TASK_START_STK_SIZE];

// allocate the print buffer
PRINT_DEFINEBUFFER();




/*++

Routine Description:

    Standard program main entry point.

Arguments:

    None.

Return Value:

    Returns 0, which indicates nothing.

--*/
int main(void)
{
    INT8U err;
    
    scb2300_t SCBParams = {
        /* Initial SCB Parameters */
        .PLL_M_Mul = 12,           /* PLL Multiplier. Valid values 6 through 512*/
        .PLL_N_Div = 1,            /* PLL Divider. Valid values 1 through 32 */
        .PLL_Fcco = 288000000,     /* Frequency (Hz) of PLL output */
        .CCLK_Div = 6,            /* CPU Clock divider, cclk */
        .MAMMode = MAMCR_PARTIAL,  /* MAM mode Partial is the preferred setting for Rev -,A parts */
        .MAMTim = MAMTIM_AUTOCFG,  /* Let initMAM calculate the optimal MAM timing */
    };


    /* pre-initialize so we can use the serial port, etc.*/
    initHardware(&SCBParams);

    RETAILMSG(1, (
              "main: MP3-DEMO Built %s %s.\n\r\n\r",
              __DATE__,
              __TIME__));


    // initialize the OS
    DEBUGMSG(1, ("main: Running OSInit()...\n\r"));
    OSInit();

      // create the startup task
    snprintf(stringbuffer, 128, "TEST \n\r");
    printString(stringbuffer); 
    DEBUGMSG(1, ("main: Creating start up task\n\r"));
    err = OSTaskCreate(StartupTask, (void*)0,
                 (void*)&StartupStk[APP_TASK_START_STK_SIZE-1], APP_TASK_START_PRIO);
    if (err != OS_NO_ERR) {
        DEBUGMSG(1, ("main: failed creating start up task\n\r"));
        while(TRUE);  //park on error
    }

    DEBUGMSG(1, ("Starting UCOS...\n\r"));


    // start the OS
    OSStart();

    // We never get here.
    RETAILMSG(1, (
              "main: MP3-DEMO: Exiting.\n\r\n\r"));

    return 0;
}

