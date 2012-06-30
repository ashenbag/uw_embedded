/*
              drv_mps.h
              LP2378 MP3-MOD driver header file
              This file contains the interface between the driver the application
              Paul Lever
              Dec. 2007

*/
#ifndef __MP3_H__
#define __MP3_H__

    //name of SPI device
#define MP3_DRV0   "\\dev\\MP30"

    // define the control codes and data for this driver
    // set the SPI port handle and initialize device
    //   input: HANDLE - open handle to SPI port or OS_DRV_INVALID_HANDLE to disable
#define IOCTL_MP3_SET_SPI		1
    // set for data output
    //   input: INT32   0 - cmd mode, non-zero data mode
#define IOCTL_MP3_SET_DATA_MODE 2

INT8U Mp3_Init(POSDRV_DISPATCH pDispatch, PVOID *ppContext, POSDRV_CONFIG pConfig);

#endif //__MP3__
