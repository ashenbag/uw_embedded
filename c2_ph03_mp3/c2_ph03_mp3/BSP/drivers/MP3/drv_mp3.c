/*
              drv_mp3.c
              LP2378 MP3-MOD driver
              Paul Lever
              Dec. 2007

*/

#include "includes.h"  // OS includes

#include "driver.h"
#include "drv_spi.h"
#include "drv_mp3.h"

  // we are supportig only one device in this driver
#define MP3_DEVICE_COUNT 1

// define a data extension to hold data required by this driver
typedef struct {
    HANDLE  hSpi;                        // handle to SPI port
    INT8U   OpenCount[MP3_DEVICE_COUNT]; // track opens handles to this device
    BOOLEAN DataMode;                    // in data mode
} MP3_EXT, *PMP3_EXT;


// prototypes
INT8U Mp3_Open(INT8U DeviceIndex, PVOID pContext, INT8U Flags);
INT8U Mp3_Close(INT8U DeviceIndex, PVOID pContext);
INT8U Mp3_Write(INT8U DeviceIndex, PVOID pContext, PVOID pBuffer, INT32U* pCount);
INT8U Mp3_Ioctl(INT8U DeviceIndex, PVOID pContext, INT8U Control, PVOID pBuffer, INT32U* pCount);
static INT8U Mp3Init(PMP3_EXT pContext, HANDLE hSpi);


//init the GPIO pins we use
#define MP3_DREQ_BIT (1<<10)         /* UEXT-3  txd2 P0.10 == data request output */
#define MP3_RESET_BIT (1<<11)        /* UEXT-4  rxd2 P0.11 == data request output */
#define MP3_CS_BIT   (1<<28)         /* UEXT-5  scl0 P0.28 == cs chip select */
#define MP3_EEPROM_BIT (1<<27)       /* UEXT-6  sda0 P0.27 == sw_cs1 */
#define MP3_DCS_BIT  (1<<6)          /* UEXT-10 ssel P0.06 == dcs data chip select */
#define MP3_CS(x) ((x) ? WRITEREG32(FIO0CLR, MP3_CS_BIT) : WRITEREG32(FIO0SET, MP3_CS_BIT))      
#define MP3_DCS(x) ((x) ? WRITEREG32(FIO0CLR, MP3_DCS_BIT) : WRITEREG32(FIO0SET, MP3_DCS_BIT))      
#define MP3_RESET(x) ((x) ? WRITEREG32(FIO0CLR, MP3_RESET_BIT) : WRITEREG32(FIO0SET, MP3_RESET_BIT))
#define MP3_EEPROM(x) ((x) ? WRITEREG32(FIO0CLR, MP3_EEPROM_BIT) : WRITEREG32(FIO0SET, MP3_EEPROM_BIT))
#define MP3_DREQ() (READREG32(FIO0PIN) & MP3_DREQ_BIT)



/* 
    Mp3_Init - SPI driver initialization routine
*/
INT8U Mp3_Init(POSDRV_DISPATCH pDispatch, PVOID *ppContext, POSDRV_CONFIG pConfig)
{
      // allocate the data extension for use to save data for this driver
	*ppContext = DRV_MEMALLOC(sizeof(MP3_EXT));
	  // zero the data extension
	memset(*ppContext, 0, sizeof(MP3_EXT));  
	
	((PMP3_EXT)*ppContext)->hSpi = OS_DRV_INVALID_HANDLE;
	
	  // create a name for each device that we support
	OSDRV_AddName(pConfig, "MP30");
	
	  // setup the supported entry points for this driver
	pDispatch->pOpen  = Mp3_Open;  // required
	pDispatch->pClose = Mp3_Close; // required
	pDispatch->pRead  = NULL;      // optional, not currently supported in this driver
	pDispatch->pWrite = Mp3_Write; // optional
	pDispatch->pIoctl = Mp3_Ioctl; // optional
	
	  // set this driver to accept serialized requests on each handle
	pConfig->Exclusive = TRUE;
	
	  // return success
	return OS_DRV_NO_ERR;
}

/*
   MP3_Open - open function for driver
*/
INT8U Mp3_Open(INT8U DeviceIndex, PVOID pContext, INT8U Flags)
{
    INT8U err;
    
    if (DeviceIndex >= MP3_DEVICE_COUNT) {
            // someone passed us a bogus device
        DEBUGMSG(TRUE, ("MP3_DRV attempt to open bad device index: 0x%X\n\r", DeviceIndex));
        return OS_DRV_NO_DEVICE;
    }
    
        // we only allow one handle to the device at a time in this driver
    if (((PMP3_EXT)pContext)->OpenCount[DeviceIndex] > 0) {
        DEBUGMSG(TRUE, ("MP3_DRV attempt to open too many handles\n\r"));
        return OS_DRV_NO_DEVICE;
    }
    
    if (err != OS_DRV_NO_ERR) {
        return err;
    }
    // count the open handles
	((PMP3_EXT)pContext)->OpenCount[DeviceIndex]++;
	
	  // return success
	return OS_DRV_NO_ERR;
}

/*
   Mp3_Close - close function for driver
*/
INT8U Mp3_Close(INT8U DeviceIndex, PVOID pContext)
{
    if (DeviceIndex >= MP3_DEVICE_COUNT) {
            // someone passed us a bogus device
        DEBUGMSG(TRUE, ("MP3_DRV attempt to close bad device index: 0x%X\n\r", DeviceIndex));
        return OS_DRV_NO_DEVICE;
    }
	  // keep count of the open handles 
	((PMP3_EXT)pContext)->OpenCount[DeviceIndex]--;
	  
	  // return success
	return OS_DRV_NO_ERR;
}

void Delay (INT32U d) {
  volatile INT32U a = d; 
  while (--a!=0);
}

/*
   Mp3_Write - write function for driver
*/
INT8U Mp3_Write(INT8U DeviceIndex, PVOID pContext, PVOID pBuffer, INT32U* pCount)
{
    INT32U length = *pCount;
    INT8U err;
    INT32U pos;
    INT32U len;
#define DREQ_CHUNK 32
    
    *pCount = 0;
    
    if (DeviceIndex >= MP3_DEVICE_COUNT) {
            // someone passed us a bogus device
        DEBUGMSG(TRUE, ("MP3_DRV attempt to Write bad device index: 0x%X\n\r", DeviceIndex));
        return OS_DRV_NO_DEVICE;
    }
    
        //we need to check DREQ every 32-bytes to not overflow the vn1002 chips FIFO
    for (pos = 0; pos < length; pos += len) {
            //send data out SPI port
        if (((PMP3_EXT)pContext)->DataMode) {
                //wait for ready
            while (!MP3_DREQ());
                //program chip
            MP3_DCS(1);
        } else {
            MP3_CS(1);
        }
        len = ((pos + DREQ_CHUNK) > length)? length - pos : DREQ_CHUNK;
        err = Write(((PMP3_EXT)pContext)->hSpi, &(((INT8U*)pBuffer))[pos], &len);
        (((PMP3_EXT)pContext)->DataMode)? MP3_DCS(0) : MP3_CS(0);
        Delay(10); //??
        if (err != OS_DRV_NO_ERR) {
            DEBUGMSG(TRUE, ("MP3_DRV failed SPI Write, err: %d\n\r", err));
            return err;
        }
    }
    *pCount = length;   
    
    return OS_DRV_NO_ERR;
}

/*
   Mp3_Ioctl - Ioctl function for driver
*/
INT8U Mp3_Ioctl(INT8U DeviceIndex, PVOID pContext, INT8U Control, PVOID pBuffer, INT32U* pCount)
{
    INT8U err = OS_DRV_NO_ERR;
    
    if (DeviceIndex >= MP3_DEVICE_COUNT) {
            // someone passed us a bogus device
        DEBUGMSG(TRUE, ("MP3_DRV attempt to Ioctl bad device index: 0x%X\n\r", DeviceIndex));
        return OS_DRV_NO_DEVICE;
    }
    
	  // determine which Control code is being used
	switch(Control) {
	     // set SPI handle and initilaize device
      case IOCTL_MP3_SET_SPI:
           // make sure we have enough room in the input buffer
         if (*pCount != sizeof(HANDLE)) {
            return OS_DRV_BUFFER_SIZE;
         }
         err = Mp3Init((PMP3_EXT)pContext, *((HANDLE *)pBuffer));
         if (err == OS_DRV_NO_ERR) {
             ((PMP3_EXT)pContext)->hSpi = *((HANDLE *)pBuffer);
         }
	         // set the return length equal to the amount of data returned
         *pCount = 0;
      break;
      
      case IOCTL_MP3_SET_DATA_MODE:
          // make sure we have enough room in the input buffer
        if (*pCount != sizeof(INT32U)) {
           return OS_DRV_BUFFER_SIZE;
        }
        (((PMP3_EXT)pContext))->DataMode = (BOOLEAN)*(INT32U *)pBuffer;
            // set the return length equal to the amount of data returned
        *pCount = 0;
        break;
        
      default:
           // we don't know this control code
	       // set the return length equal to the amount of data returned
		 if (pCount != NULL) {
	         *pCount = 0;   
	     }
         return OS_DRV_UNSUPPORTED; 
	}
	
	return err;
}
        
/*
   Mp3Init - initialize the SPI port
*/
static INT8U Mp3Init(PMP3_EXT pContext, HANDLE hSpi)
{
    const INT8U SetMode[] = {0x02, 0x00, 0x08, 0x00};
    const INT8U SetClockF[] = {0x02, 0x03, 0x98, 0x00};
//??    const INT8U SetVol[] = {0x02, 0x0B, 0x20, 0x20};
    const INT8U SetVol[] = {0x02, 0x0B, 0x00, 0x00};
    INT8U buffer[4];
    INT8U err;
    INT32U len;
    
        //setup the GPIO pin directions
    WRITEREG32(FIO0DIR, (READREG32(FIO0DIR) | MP3_CS_BIT | MP3_EEPROM_BIT | MP3_DCS_BIT | MP3_RESET_BIT) & ~MP3_DREQ_BIT);
        //no pullup DCS
    WRITEREG32(PINMODE1, (READREG32(PINMODE1) | (0x3 << 0)));
     
#if  0
#define SCK0_BIT (1<<15)   
#define SCK0(x) ((x) ? WRITEREG32(FIO0CLR, SCK0_BIT) : WRITEREG32(FIO0SET, SCK0_BIT))      
    //test bits
    WRITEREG32(FIO0DIR, 0xFFFFFFFF);
    WRITEREG32(FIO0CLR, 0xFFFFFFFF);
    WRITEREG32(FIO0SET, 0xFFFFFFFF);
    WRITEREG32(FIO0CLR, 0xFFFFFFFF);
    WRITEREG32(FIO0SET, 0xFFFFFFFF);
    WRITEREG32(FIO0CLR, 0xFFFFFFFF);
    for (len = 0; len < 32; len++) {
        WRITEREG32(FIO0SET, (1<<len));
    }
    WRITEREG32(FIO0SET, 0xFFFFFFFF);
    for (len = 0; len < 32; len++) {
        WRITEREG32(FIO0CLR, (1<<len));
    }
    MP3_DCS(1);
    len = len;
    MP3_DCS(0);
    len = len;
    
    SCK0(1);   
    len = len;
    SCK0(0);   
    len = len;

    MP3_CS(1);
    len = len;
    MP3_CS(0);
    len = len;
    MP3_EEPROM(1);
    len = len;
    MP3_EEPROM(0);
    len = len;
    MP3_RESET(1);
    len = len;
    MP3_RESET(0);
    len = len;
#endif
        //reset
    MP3_CS(0);
    MP3_EEPROM(1);
        // initialize the VS1002 chip
    MP3_RESET(1);
    OSTimeDly(2);
    MP3_RESET(0);
        //wait for ready
    while (!MP3_DREQ());
        //program chip
    MP3_CS(1);
    len = sizeof(SetMode);
    memcpy(buffer, SetMode, len);
    err = Write(hSpi, buffer, &len);
    MP3_CS(0);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(TRUE, ("MP3_DRV failed to initilaize mode, err: %d\n\r", err));
        return err;
    }
    OSTimeDly(2);
    MP3_CS(1);
    len = sizeof(SetClockF);
    memcpy(buffer, SetClockF, len);
    err = Write(hSpi, buffer, &len);
    MP3_CS(0);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(TRUE, ("MP3_DRV failed to initilaize ClockF, err: %d\n\r", err));
        return err;
    }
    OSTimeDly(2);
    MP3_CS(1);
    len = sizeof(SetVol);
    memcpy(buffer, SetVol, len);
    err = Write(hSpi, buffer, &len);
    MP3_CS(0);
    if (err != OS_DRV_NO_ERR) {
        DEBUGMSG(TRUE, ("MP3_DRV failed to initilaize SetVol, err: %d\n\r", err));
        return err;
    }
    OSTimeDly(2);
    

    
    return OS_DRV_NO_ERR;
}
