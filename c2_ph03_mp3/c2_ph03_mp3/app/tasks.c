/*++
Copyright (c) 2001-2007  University of Washington Extension.  

Module Name:

    tasks.c

Module Description:

    The tasks that are executed by the test application.
    
Revision History:

--*/


#include "includes.h"  // OS includes
#include "print.h"
#include "uarts.h"
#include "init.h"
#include "driver.h"
#include "drv_spi.h"
#include "drv_mp3.h"

#include "mp3/train_crossing.h"

#define PLAY_SINE_TEST 1
#define CONTINUOUS     1

// allocate the stacks for each task
static OS_STK   TaskPlayStk[APP_TASK_DEFAULT_STK_SIZE];
static OS_STK   TaskMonStk[APP_TASK_DEFAULT_STK_SIZE];

#define MP3BUFFERSIZE   256
INT8U buffer[MP3BUFFERSIZE];

// task prototypes
void PlayTask(void* pdata);
void MonTask(void* pdata);
static void SetLED(BOOLEAN On);
static INT32U GetMP3(INT8U* pBuffer, INT32U Length, BOOLEAN Start);

//state variable to track CPU usage
static INT32U State = 0;
OS_EVENT * SemPrint;
//get external refrence to the print buffer
PRINT_BUFFER();

// create the driver table. Only need to set the drivers Init functions
OSDRV_DRIVER_ENTRY DriverTable[] = {
// Init                                   Config, Context Initialized, RefCount
{ {Spi_Init, NULL, NULL, NULL, NULL, NULL}, {0},    NULL,    FALSE,      0},
{ {Mp3_Init, NULL, NULL, NULL, NULL, NULL}, {0},    NULL,    FALSE,      0}
// add additional drivers here
};  
#define DRIVER_COUNT (sizeof(DriverTable)/sizeof(OSDRV_DRIVER_ENTRY)) 

// this task is the initial task running, started by main(). It begins the system tick timer
// and creates all the other task. Then it deletes itself.
void StartupTask(void* pdata)
{
    INT8U err;
    
    DEBUGMSG(1,("StartupTask: begin\n\r"));
    DEBUGMSG(1,("StartupTask: tick timer\n\r"));
    // Initialize BSP functions   
    BSP_Init();                                                 
    // re-init the UART so we can use the serial port
    initUART0(38400, UART_8N1, UART_FIFO_OFF, getFcclk());
    // initialize the driver sub-system
    err = OSDRV_SubsysInit(DriverTable, DRIVER_COUNT);
    if (err != OS_DRV_NO_ERR) {
        RETAILMSG(1,("StartupTask: Failed to initialize driver subsystem: %d\n\r", err));
        //park here
        while(TRUE);
    }
    SemPrint = OSSemCreate(1);
	  // create the the test tasks
      // we have OS_STK_GROWTH set to 1, so the stack grows from high to low
    DEBUGMSG(1,("StartupTask: Creating the tasks...\n\r"));
    OSTaskCreate(PlayTask, (void*)0, (void*)&TaskPlayStk[APP_TASK_DEFAULT_STK_SIZE-1], APP_TASK_PLAY_PRIO);

#ifdef OS_TASK_STAT_EN
      //create a CPU monitor task 
    OSStatInit ();
//??    OSTaskCreate(MonTask, (void*)0, (void*)&TaskMonStk[APP_TASK_DEFAULT_STK_SIZE-1], APP_TASK_MON_PRIO);
#endif //OS_TASK_STAT_EN
    
      // delete ourselves, letting the work be done in the new tasks.
    DEBUGMSG(1,("StartupTask: deleting self\n\r"));
	OSTaskDel(OS_PRIO_SELF);  
}


// PlayTask - output some sounds
void PlayTask(void* pdata)
{
    INT8U err;
    HANDLE hSpi;        //handle to SPI driver
    HANDLE hMp3;        //handle to MP3 driver
    INT32U length;
    const INT8U SineWave[] = {0x53, 0xEF, 0x6E, 0x44, 0x00, 0x00, 0x00, 0x00};
    const INT8U Deact[]    = {0x45, 0x78, 0x69, 0x74, 0x00, 0x00, 0x00, 0x00};
    const INT8U ModeTest[] = {0x02, 0x00, 0x08, 0x20};
    const INT8U ModePlay[] = {0x02, 0x00, 0x08, 0x00};
    const INT8U SoftReset[]   = {0x02, 0x00, 0x08, 0x04};
    INT32U DataMode = 1;
    INT32U ii;
    BOOLEAN start;
    
    OSSemPend(SemPrint, 0, &err);
    DEBUGMSG(1,("PlayTask: begin\n\r"));
    OSSemPost(SemPrint);
        //set states as we go along so we track CPU uasge better
    State = 1;        
        //get a handles to the SPI and MP3 drivers
    hSpi = Open(SPI_DRV1, OSDRV_WRITE |OSDRV_EXCLUSIVE);
    if (hSpi == OS_DRV_INVALID_HANDLE) {
        DEBUGMSG(1,("PlayTask: failed to open SPI driver\n\r"));
        OSTaskDel(OS_PRIO_SELF);  
    }
    hMp3 = Open(MP3_DRV0, OSDRV_WRITE |OSDRV_EXCLUSIVE);
    if (hMp3 == OS_DRV_INVALID_HANDLE) {
        DEBUGMSG(1,("PlayTask: failed to open MP3 driver\n\r"));
        OSTaskDel(OS_PRIO_SELF);  
    }
        //tell the MP3 driver which SPI port driver to use
    length = sizeof(HANDLE);
    err = Ioctl(hMp3, IOCTL_MP3_SET_SPI, &hSpi, &length);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(1,("PlayTask: failed to set SPI port to open MP3 driver, err: %d\n\r", err));
        OSTaskDel(OS_PRIO_SELF);
    }
    //reset the MP3 chip
    length = sizeof(SoftReset);
    memcpy(buffer, SoftReset, length);
    err = Write(hMp3, buffer, &length);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(1,("PlayTask: failed write, err: %d\n\r", err));
        OSTaskDel(OS_PRIO_SELF);
    }

#if PLAY_SINE_TEST    
    //allow tests
    length = sizeof(ModeTest);
    memcpy(buffer, ModeTest, length);
    err = Write(hMp3, buffer, &length);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(1,("PlayTask: failed write, err: %d\n\r", err));
        OSTaskDel(OS_PRIO_SELF);
    }
    
        //set to data mode
    DataMode = 1;
    length = sizeof(DataMode);
    err = Ioctl(hMp3, IOCTL_MP3_SET_DATA_MODE, &DataMode, &length);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(1,("PlayTask: failed to set data mode, err: %d\n\r", err));
        OSTaskDel(OS_PRIO_SELF);
    }
    State = 2;        
    OSSemPend(SemPrint, 0, &err);
    DEBUGMSG(1,("PlayTask: playing\n\r"));
    OSSemPost(SemPrint);
    for(ii = 0; ii < 3; ii++) {    
        State = 3;        
            //play test sound (note, first time through the chip may not play)
        length = sizeof(SineWave);
        memcpy(buffer, SineWave, length);
        err = Write(hMp3, buffer, &length);
        if (err != OS_DRV_NO_ERR) {
            DEBUGMSG(1,("PlayTask: failed write, err: %d\n\r", err));
            OSTaskDel(OS_PRIO_SELF);
        }
        SetLED(0);
        OSTimeDlyHMSM(0, 0, 10, 000);
    OSSemPend(SemPrint, 0, &err);
        DEBUGMSG(1,("PlayTask: stopping\n\r"));
    OSSemPost(SemPrint);
            //stop
        length = sizeof(Deact);
        memcpy(buffer, Deact, length);
        err = Write(hMp3, buffer, &length);
        if (err != OS_DRV_NO_ERR) {
            DEBUGMSG(1,("PlayTask: failed write, err: %d\n\r", err));
            OSTaskDel(OS_PRIO_SELF);
        }
        SetLED(1);
        OSTimeDlyHMSM(0, 0, 4, 000);
    }    
#endif //PLAY_SINE_TEST
    State = 4;        

        //set to cmd mode
    DataMode = 0;
    length = sizeof(DataMode);
    err = Ioctl(hMp3, IOCTL_MP3_SET_DATA_MODE, &DataMode, &length);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(1,("PlayTask: failed to set data mode, err: %d\n\r", err));
        OSTaskDel(OS_PRIO_SELF);
    }
    //set to play mode
    length = sizeof(ModePlay);
    memcpy(buffer, ModePlay, length);
    err = Write(hMp3, buffer, &length);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(1,("PlayTask: failed write, err: %d\n\r", err));
        OSTaskDel(OS_PRIO_SELF);
    }
    //set to data mode
    DataMode = 1;
    length = sizeof(DataMode);
    err = Ioctl(hMp3, IOCTL_MP3_SET_DATA_MODE, &DataMode, &length);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(1,("PlayTask: failed to set data mode, err: %d\n\r", err));
        OSTaskDel(OS_PRIO_SELF);
    }
    //loop though the data until done
    start  = TRUE;
    while(TRUE) { 
        INT32U len;
        State = 5;        
        len = GetMP3(buffer, MP3BUFFERSIZE, start);
        start = FALSE;
        SetLED(1);
        if (len > 0) {
            length = len;
            err = Write(hMp3, buffer, &length);
            if (err != OS_DRV_NO_ERR) {
                DEBUGMSG(1,("PlayTask: failed write, err: %d\n\r", err));
                while(TRUE); //park here on error
            }
        }
        SetLED(0);
        if (len < MP3BUFFERSIZE) {
            //we are done with file
#ifdef CONTINUOUS
            start = TRUE;
    OSSemPend(SemPrint, 0, &err);
            DEBUGMSG(1,("PlayTask: restarting\n\r"));
    OSSemPost(SemPrint);
#else  //CONTINUOUS
            break;
#endif //CONTINUOUS
        }
    }
    State = 6;        
    OSSemPend(SemPrint, 0, &err);
    DEBUGMSG(1,("PlayTask: exiting\n\r"));
    OSSemPost(SemPrint);
    OSTaskDel(OS_PRIO_SELF);
}

/*
 * GetMP3 - get MP3 buffers
 *          pBuffer = buffer to fill
 *          Length - size to fill
 *          Start - TRUE start at begining of file
 *          return - number of bytes returned. If less than Length, then end of file.
*/
INT32U GetMP3(INT8U* pBuffer, INT32U Length, BOOLEAN Start)
{
    static INT32U current = 0;
    static INT32U count = 0;
    INT32U len;
    INT32U* ptmp = &current;
    
    if (Start) {
        current = 0;
    }
    count++;
    len = (Length + current) < sizeof(Wave) ? Length : sizeof(Wave) - current;
    if (((current) >= (sizeof(Wave)-100)) || (len > 256)) {
        INT32U tmp;
        tmp = len;
    }
    memcpy(pBuffer, &Wave[current], len);
    if (((current) >= (sizeof(Wave)-100)) || (len > 256)) {
        INT32U tmp;
        tmp = len;
    }
    current += len;
    return len;
}

// MonTask - monitor the CPU usage
void MonTask(void* pdata)
{
    INT8U err;
    //give system time to get initial values
    OSTimeDlyHMSM(0, 0, 1, 000);

    while(TRUE) {
    OSSemPend(SemPrint, 0, &err);
        DEBUGMSG(1,("MonTask: cpu %%: %d, state: %d\n\r", OSCPUUsage, State));
    OSSemPost(SemPrint);
        OSTimeDlyHMSM(0, 0, 1, 000);
    }
}

// toggle the LED next to the display (labeled SD)
static void SetLED(BOOLEAN On)
{
    if (On) {
        WRITEREG32(FIO0CLR, SD_LED_BIT);
    } else {
        WRITEREG32(FIO0SET, SD_LED_BIT);
    }
}
/*
*********************************************************************************************************
*********************************************************************************************************
**                                         uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskCreateHook(ptcb);
#endif
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                        TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif
}
#endif
#endif
