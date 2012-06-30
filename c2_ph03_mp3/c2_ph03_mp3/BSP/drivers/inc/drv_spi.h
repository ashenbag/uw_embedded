/*
              drv_spi.h
              LP2378 SPI driver header file
              This file contains the interface between the driver the application
              Paul Lever
              Dec. 2007

*/

#ifndef __SPI_H__
#define __SPI_H__

    //name of SPI devices
#define SPI_DRV0   "\\dev\\SPI0"
#define SPI_DRV1   "\\dev\\SPI1"

    // define the control codes and data for this driver
    // set the clock rate
    //   input: INT32U - master clcok rate
#define IOCTL_SPI_SET_CLOCK		1
    // get the clock rate
    //   output: INT32U - cuurent master clock rate
#define IOCTL_SPI_GET_CLOCK     2
  
  
INT8U Spi_Init(POSDRV_DISPATCH pDispatch, PVOID *ppContext, POSDRV_CONFIG pConfig);

#endif //__SPI_H__
