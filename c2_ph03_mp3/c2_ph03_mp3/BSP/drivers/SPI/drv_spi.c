/*
              drv_spi.c
              LP2378 SPI driver
              Paul Lever
              Dec. 2007

*/

#include "includes.h"  // OS includes

#include "driver.h"
#include "drv_spi.h"
#include "init.h"

  
#define SPI_DEFAULT_CLOCK 1000000    

  // we are supporting two SSP devices
#define SPI_DEVICE_COUNT 2

    // define data for each supported device
typedef struct _SPI_DEVICE {
    INT32U   Clock;         // clock rate
    INT8U    OpenCount;     // track opens handles to this device
    INT16U*  pSSPnCR0;      // SSP register set
    INT8U*   pSSPnCR1;
    INT16U*  pSSPnDR;
    INT8U*   pSSPnSR;
    INT8U*   pSSPnCPSR;
    INT8U*   pSSPnIMSC;
    INT8U*   pSSPnRIS;
    INT8U*   pSSPnMIS;
    INT8U*   pSSPnICR;
    INT16U*  pSSPnDMACR;
    INT32U   PinSck;        // I/O pins
    INT32U   PinMISO;
    INT32U   PinMOSI;
}SPI_DEVICE, *PSPI_DEVICE;


// define a data extension to hold data required by this driver
typedef struct _SPI_EXT{
    SPI_DEVICE   Device[SPI_DEVICE_COUNT]; // each supported device
} SPI_EXT, *PSPI_EXT;


#define SPI0_SCK (1<<15)       /*       -  sck0  P0.15*/
#define SPI0_MISO (1<<17)      /*       -  MISO0 P0.17*/
#define SPI0_MOSI (1<<18)      /*       -  MOSI0 P0.18*/
#define SPI1_SCK (1<<7)        /* UEXT9 -  sck  P0.7*/
#define SPI1_MISO (1<<8)       /* UEXT7 -  MISO P0.8*/
#define SPI1_MOSI (1<<9)       /* UEXT8 -  MOSI P0.9*/
    
//status register 
#define SSP_TNF   0x2        
#define SSP_RNE   0x4   
//control register0
#define SSP_SPIMODE  0x07    
//control register1
#define SSP_ENABLE 0x02
    

// prototypes
INT8U Spi_Open(INT8U DeviceIndex, PVOID pContext, INT8U Flags);
INT8U Spi_Close(INT8U DeviceIndex, PVOID pContext);
INT8U Spi_Write(INT8U DeviceIndex, PVOID pContext, PVOID pBuffer, INT32U* pCount);
INT8U Spi_Ioctl(INT8U DeviceIndex, PVOID pContext, INT8U Control, PVOID pBuffer, INT32U* pCount);
static void SpiSetClock(PSPI_DEVICE pDevice, INT32U ClockRate);
static INT8U SpiInit(PSPI_DEVICE pDevice, INT32U ClockRate);

/* 
    Spi_Init - SPI driver initialization routine
*/
INT8U Spi_Init(POSDRV_DISPATCH pDispatch, PVOID *ppContext, POSDRV_CONFIG pConfig)
{
    PSPI_DEVICE pDevice;
    
      // allocate the data extension for use to save data for this driver
	*ppContext = DRV_MEMALLOC(sizeof(SPI_EXT));
    if (*ppContext == NULL) {
        return OS_DRV_BUFFER_SIZE;
    }
	  // zero the data extension
	memset(*ppContext, 0, sizeof(SPI_EXT));  
	  // initialize the addresses for the supported devices
	pDevice = &((PSPI_EXT)*ppContext)->Device[0];
	pDevice->pSSPnCR0   = (INT16U*)SSP0CR0;
	pDevice->pSSPnCR1   = (INT8U*)SSP0CR1;
	pDevice->pSSPnDR    = (INT16U*)SSP0DR;
	pDevice->pSSPnSR    = (INT8U*)SSP0SR;
	pDevice->pSSPnCPSR  = (INT8U*)SSP0CPSR;
	pDevice->pSSPnIMSC  = (INT8U*)SSP0IMSC;
	pDevice->pSSPnRIS   = (INT8U*)SSP0RIS;
    pDevice->pSSPnMIS   = (INT8U*)SSP0MIS;
	pDevice->pSSPnICR   = (INT8U*)SSP0ICR;
	pDevice->pSSPnDMACR = (INT16U*)SSP0DMACR;
	  //set the pins
	pDevice->PinSck = SPI0_SCK;
    pDevice->PinMISO = SPI0_MISO;
    pDevice->PinMOSI = SPI0_MOSI;
	  // create a name for each device that we support
	OSDRV_AddName(pConfig, "SPI0");

	// now do the second device
    pDevice = &((PSPI_EXT)*ppContext)->Device[1];
	pDevice->pSSPnCR0   = (INT16U*)SSP1CR0;
	pDevice->pSSPnCR1   = (INT8U*)SSP1CR1;
	pDevice->pSSPnDR    = (INT16U*)SSP1DR;
	pDevice->pSSPnSR    = (INT8U*)SSP1SR;
	pDevice->pSSPnCPSR  = (INT8U*)SSP1CPSR;
	pDevice->pSSPnIMSC  = (INT8U*)SSP1IMSC;
	pDevice->pSSPnRIS   = (INT8U*)SSP1RIS;
    pDevice->pSSPnMIS   = (INT8U*)SSP1MIS;
	pDevice->pSSPnICR   = (INT8U*)SSP1ICR;
	pDevice->pSSPnDMACR = (INT16U*)SSP1DMACR;
      //set the pins
    pDevice->PinSck = SPI1_SCK;
    pDevice->PinMISO = SPI1_MISO;
    pDevice->PinMOSI = SPI1_MOSI;
	  // create a name for each device that we support
	OSDRV_AddName(pConfig, "SPI1");

	  // setup the supported entry points for this driver
	pDispatch->pOpen  = Spi_Open;  // required
	pDispatch->pClose = Spi_Close; // required
	pDispatch->pRead  = NULL;      // optional, not currently supported in this driver
	pDispatch->pWrite = Spi_Write; // optional
	pDispatch->pIoctl = Spi_Ioctl; // optional
	
	  // set this driver to accept non-serialized requests on each handle
	  // any synchronization needs to be handled by the driver or caller above us
	pConfig->Exclusive = FALSE;
	
	  // return success
	return OS_DRV_NO_ERR;
}

/*
   Spi_Open - open function for driver
*/
INT8U Spi_Open(INT8U DeviceIndex, PVOID pContext, INT8U Flags)
{
    PSPI_DEVICE pDevice = &((PSPI_EXT)pContext)->Device[DeviceIndex];
    INT8U err;
    
    if (DeviceIndex >= SPI_DEVICE_COUNT) {
            // someone passed us a bogus device
        DEBUGMSG(TRUE, ("SPI_DRV attempt to open bad device index: 0x%X\n\r", DeviceIndex));
        return OS_DRV_NO_DEVICE;
    }
    
        // we only allow one handle to the device at a time in this driver
    if (((PSPI_EXT)pContext)->Device[DeviceIndex].OpenCount > 0) {
        DEBUGMSG(TRUE, ("SPI_DRV attempt to open too many handles\n\r"));
        return OS_DRV_NO_DEVICE;
    }
        //initialize the SPI port
    err = SpiInit(pDevice, SPI_DEFAULT_CLOCK);
    
    if (err != OS_DRV_NO_ERR) {
        return err;
    }
    // count the open handles
	((PSPI_EXT)pContext)->Device[DeviceIndex].OpenCount++;
	
	  // return success
	return OS_DRV_NO_ERR;
}

/*
   Spi_Close - close function for driver
*/
INT8U Spi_Close(INT8U DeviceIndex, PVOID pContext)
{
    if (DeviceIndex >= SPI_DEVICE_COUNT) {
            // someone passed us a bogus device
        DEBUGMSG(TRUE, ("SPI_DRV attempt to close bad device index: 0x%X\n\r", DeviceIndex));
        return OS_DRV_NO_DEVICE;
    }
	  // keep count of the open handles 
	((PSPI_EXT)pContext)->Device[DeviceIndex].OpenCount--;
	  
	  // return success
	return OS_DRV_NO_ERR;
}

/*
   Spi_Write - write function for driver
*/
INT8U Spi_Write(INT8U DeviceIndex, PVOID pContext, PVOID pBuffer, INT32U* pCount)
{
    INT32U length = *pCount;
    INT32U ii;
    PSPI_DEVICE pDevice = &((PSPI_EXT)pContext)->Device[DeviceIndex];
    INT8U tmp;
    
    *pCount = 0;
    
    if (DeviceIndex >= SPI_DEVICE_COUNT) {
            // someone passed us a bogus device
        DEBUGMSG(TRUE, ("SPI_DRV attempt to Write bad device index: 0x%X\n\r", DeviceIndex));
        return OS_DRV_NO_DEVICE;
    }
        //write out the bytes
    for (ii = 0; ii < length; ii++) {
        while(!((tmp = READREG8(pDevice->pSSPnSR)) & SSP_TNF)) {
//??            DEBUGMSG(TRUE, ("SPI_DRV SPSR: 0x%X\n\r", tmp));
        }
        WRITEREG16(pDevice->pSSPnDR, (INT16U)((INT8U*)pBuffer)[ii]);
        while(!((tmp = READREG8(pDevice->pSSPnSR)) & SSP_RNE)) {
//??            DEBUGMSG(TRUE, ("SPI_DRV SPSR: 0x%X\n\r", tmp));
        }
            //read back any input data
        ((INT8U*)pBuffer)[ii] = (INT8U)READREG16(pDevice->pSSPnDR);
    }
    *pCount = length;   
    
    return OS_DRV_NO_ERR;
}

/*
   Spi_Ioctl - Ioctl function for driver
*/
INT8U Spi_Ioctl(INT8U DeviceIndex, PVOID pContext, INT8U Control, PVOID pBuffer, INT32U* pCount)
{
    PSPI_DEVICE pDevice = &((PSPI_EXT)pContext)->Device[DeviceIndex];

    if (DeviceIndex >= SPI_DEVICE_COUNT) {
            // someone passed us a bogus device
        DEBUGMSG(TRUE, ("SPI_DRV attempt to Ioctl bad device index: 0x%X\n\r", DeviceIndex));
        return OS_DRV_NO_DEVICE;
    }
    
	  // determine which Control code is being used
	switch(Control) {
	     // set master SPI clock
      case IOCTL_SPI_SET_CLOCK:
           // make sure we have enough room in the return buffer
         if (*pCount != sizeof(INT32U)) {
            return OS_DRV_BUFFER_SIZE;
         }
         SpiSetClock(pDevice, *((INT32U *)pBuffer));
	       // set the return length equal to the amount of data returned
         if (pCount != NULL) {
             *pCount = 0;
         }
      break;
      
	     // return the current master clock
      case IOCTL_SPI_GET_CLOCK:
           // make sure we have enough room in the return buffer
         if (*pCount != sizeof(INT32U)) {
            return OS_DRV_BUFFER_SIZE;
         }
           // return the current open count for this device
         *((INT32U *)pBuffer) = (long)pDevice->Clock;
	       // set the return length equal to the amount of data returned
         if (pCount != NULL) {
             *pCount = sizeof(INT32U);
         }
      break;
      
      default:
           // we don't know this control code
	       // set the return length equal to the amount of data returned
		 if (pCount != NULL) {
	         *pCount = 0;   
	     }
         return OS_DRV_UNSUPPORTED; 
	}
	
	return OS_DRV_NO_ERR;
}
        
/*
   SpiInit - initialize the SPI port
*/
static INT8U SpiInit(PSPI_DEVICE pDevice, INT32U ClockRate)
{
    
        // pinsel0_P0 must be previously setup for SSP use.
        // set their direction
    WRITEREG32(FIO0DIR, (READREG32(FIO0DIR) | pDevice->PinSck  | pDevice->PinMOSI) & ~pDevice->PinMISO);

        // set master mode, 8-bits, sample on first clock edge, no ints
    WRITEREG16(pDevice->pSSPnCR0, SSP_SPIMODE);
    
    SpiSetClock(pDevice, SPI_DEFAULT_CLOCK);
      //enable
    WRITEREG16(pDevice->pSSPnCR1, SSP_ENABLE);
    
    return OS_DRV_NO_ERR;
}
/*
   SpiSetClock - set the SPI master clock
*/
static void SpiSetClock(PSPI_DEVICE pDevice, INT32U ClockRate)
{
    //assumes SPI PCLK set to divide by 1
    INT32U clk = ((getFcclk()) / ClockRate);
    clk =  (clk < 2) ? 2 : clk;
    WRITEREG8(pDevice->pSSPnCPSR, (clk & 0xFE));
    
    pDevice->Clock = ClockRate;
}

