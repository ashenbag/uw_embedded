/*
              driver.c
              Main file for driver model 
              This is an example of extending an operating system, in this case UCOS 2.85,
              to include simply driver model.
              Paul Lever
              May 01,2001
              
              Feb. 26,2003 PDL
              Fixed and improved error checking in the Open() function
              Added checks for invalid handle
              Oct. 13, 2003
              Add additional error checks for calls to non-existent functions.
              Placed in with DEBUG for execution speed.
              Oct. 22, 2003 DMI
              Made changes within DEBUG to fix compiler errors: 
              1) OSDRV_pDriverTable index
              2) return values for some functions are INT8U, fixed return values.
              3) commented out #define DEBUG 1 and forgot to uncomment it... reverted to default #define DEBUG 1.
              Aug. 16, 2004 PDL
              Change the way the sempahore is freeded in the Close operation and use #defined memory allocate.
              Dec. 2007  PDL - updated for 2.85    

*/



#include "includes.h"  // OS includes

#include "driver.h"


	/* is driver subsystem initialized? Save the number of drivers in table and the table*/
static INT8U OSDRV_DriverCount = 0;
static POSDRV_DRIVER_ENTRY OSDRV_pDriverTable = NULL;


/*
 OSDRV_SubsysInit - initialize the driver subsystem
    pDriverTable - table of drivers to be initialized
    Count        - number of entries in pDriverTable
    return - status
    
    Notes: OSDRV_Init must be called prior to using any driver
*/
INT8U OSDRV_SubsysInit(POSDRV_DRIVER_ENTRY pDriverTable, INT8U Count)
{
   INT8U status;
   
     // save the tables
   OSDRV_DriverCount = Count;
   OSDRV_pDriverTable = pDriverTable;
  
		// initialize each driver in the system
	for (;Count > 0; Count--) {
		if (pDriverTable[Count - 1].Dispatch.pInit != NULL) {
		     // call the drivers initialization routine
		   status = pDriverTable[Count - 1].Dispatch.pInit(
		                                          &(pDriverTable[Count - 1].Dispatch),
		                                          &(pDriverTable[Count - 1].pContext),
		   										  &(pDriverTable[Count - 1].Config));
		     // save initialization status of driver
		   if (status == OS_DRV_NO_ERR) {
		      pDriverTable[Count - 1].Initialized = TRUE;
		   } else {
		      pDriverTable[Count - 1].Initialized = FALSE;
		   }
	    }
	}

    DEBUGMSG(TRUE, ("OSDRV:SubsysInit Initialized %d driver(s)\n\r", OSDRV_DriverCount));   
	return OS_DRV_NO_ERR; 
}

/*
   Application interface functions
*/
/*
   Open - open a handle to a driver
*/
HANDLE Open(char *pName, INT8U Flags)
{
  INT8U count;
  POSDRV_NAME pNameItem;
  INT8U index;
  INT8U err;
  POSDRV_HANDLE_INTERNAL pHandle;
  
    
		// first find the name of the device by walking the list of
		// drivers and registered names
		// for each driver
	for (count = 0; count < OSDRV_DriverCount; count++)	{
	      // walk the names list of this driver, tracking the index number
	      for(index = 0, pNameItem = &OSDRV_pDriverTable[count].Config.Name;
	          pNameItem != NULL;
	          pNameItem  = pNameItem->pNext, index++) {
			  if (strcmp(pNameItem->pName, pName) == 0) {
			      // we have a match
			      // call the drivers open
#ifdef DEBUG
                if (OSDRV_pDriverTable[count].Dispatch.pOpen == NULL) {
                    // driver has no open
                    DEBUGMSG(TRUE, ("OSDRV:Open open not supported: driver %s, error: %d\n\r", pName, OS_DRV_UNSUPPORTED));   
                  return (HANDLE)OS_DRV_UNSUPPORTED;
                }
#endif //DEBUG
                err = OSDRV_pDriverTable[count].Dispatch.pOpen(
			                                         index,
			                                         OSDRV_pDriverTable[count].pContext,
			   										 Flags);
                if (err != OS_DRV_NO_ERR) {
                    // driver failed open, tell user
                  DEBUGMSG(TRUE, ("OSDRV:Open Failed to open driver %s, error: %d\n\r", pName, err));   
                  return (HANDLE)OS_DRV_INVALID_HANDLE;
                }
                  // allocate the Handle for this open
                pHandle = DRV_MEMALLOC(sizeof(OSDRV_HANDLE_INTERNAL));
                if (pHandle == NULL) {
                  DEBUGMSG(TRUE, ("OSDRV:Open Could not obtain handle for driver %s\n\r", pName));   
                  return (HANDLE)OS_DRV_INVALID_HANDLE;
                }
                  // initialize the handle
                pHandle->DrvIndex = count;
                pHandle->DevIndex = index;
                pHandle->RefCount = 1;
                  // only allocate a semaphore if this device requested exclusive access
                if (OSDRV_pDriverTable[pHandle->DrvIndex].Config.Exclusive) {  
	                pHandle->Event = OSSemCreate(1);
	                if (pHandle->Event == NULL) { 
	                	DEBUGMSG(TRUE, ("OSDRV:Open Could not create semphore for driver %s\n\r", pName));
	                	DRV_MEMFREE(pHandle);
	                	return (HANDLE)OS_DRV_INVALID_HANDLE;
					} 
				} else {
					pHandle->Event = NULL;
				}
                  // give handle to user
                return ((HANDLE)pHandle);  
			  }
	      }
	}
	   // not found
   	DEBUGMSG(TRUE, ("OSDRV:Open Could not find driver %s\n\r", pName));   
	return (HANDLE)OS_DRV_INVALID_HANDLE;   
}


/*
   Close - close a handle to a driver
*/
INT8U  Close(HANDLE H)
{
  INT8U err;
  INT8U errsem;

      // check for invalid handle
   if (H == OS_DRV_INVALID_HANDLE)  {
   	   DEBUGMSG(TRUE, ("OSDRV:Close Invalid handle\n\r"));   
	   return OS_DRV_NO_DEVICE;   
   }
      // check for serialization
   if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Config.Exclusive) {
       // serialize
       OSSemPend(((POSDRV_HANDLE_INTERNAL)H)->Event, 0, &errsem);
   }
#ifdef DEBUG
                if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Dispatch.pClose == NULL) {
                    // driver has no close
                  DEBUGMSG(TRUE, ("OSDRV:Close close not supported, error: %d\n\r", OS_DRV_UNSUPPORTED));   
                  return OS_DRV_UNSUPPORTED;
                }
#endif //DEBUG
      // call the drivers close
   err = OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Dispatch.pClose(
			                     ((POSDRV_HANDLE_INTERNAL)H)->DevIndex,
                                 OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].pContext);
      // undo serialization                                 
   if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Config.Exclusive) {
       /* ?? do you see any holes in the approach below of deleting the sempahore? How would you get around these problems? ??*/
#ifdef DEBUG
		// delete the semaphore if it is not in use
       ((POSDRV_HANDLE_INTERNAL)H)->Event = OSSemDel(((POSDRV_HANDLE_INTERNAL)H)->Event, OS_DEL_NO_PEND, &errsem);
       if (errsem != OS_NO_ERR) {
       		//error deleting sempahore, see if another task is waiting on the semaphore
	       if (errsem == OS_ERR_TASK_WAITING) {
	       		//an application tried to close the device while one of its threads was waiting on the semaphore
                DEBUGMSG(TRUE, ("OSDRV:Close attemp to close device while task is waiting on I/O request serialization.\n\r"));
	       }
			// delete the semaphore. If anyone is waiting on it, they will be readied.
	       ((POSDRV_HANDLE_INTERNAL)H)->Event = OSSemDel(((POSDRV_HANDLE_INTERNAL)H)->Event, OS_DEL_ALWAYS, &errsem);
       }
#else  //DEBUG
		// delete the semaphore. If anyone is waiting on it, they will be readied.
       ((POSDRV_HANDLE_INTERNAL)H)->Event = OSSemDel(((POSDRV_HANDLE_INTERNAL)H)->Event, OS_DEL_ALWAYS, &errsem);
#endif //DEBUG
   }
      // free up the handle
   DRV_MEMFREE((POSDRV_HANDLE_INTERNAL)H);
   
   return err;
}

/*
   Read - read a buffer from the driver
*/
INT8U  Read(HANDLE H, PVOID pBuffer, INT32U* pLength) 
{
  INT8U err;
  INT8U errsem;

      // check for invalid handle
   if (H == OS_DRV_INVALID_HANDLE)  {
   	   DEBUGMSG(TRUE, ("OSDRV:Read Invalid handle\n\r"));   
	   return OS_DRV_NO_DEVICE;   
   }
      // check for serialization
   if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Config.Exclusive) {
       // serialize
       OSSemPend(((POSDRV_HANDLE_INTERNAL)H)->Event, 0, &errsem);
   }
#ifdef DEBUG
    if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Dispatch.pRead == NULL) {
        // driver has no close
        DEBUGMSG(TRUE, ("OSDRV:Read read not supported, error: %d\n\r", OS_DRV_UNSUPPORTED));   
        return OS_DRV_UNSUPPORTED;
    }
#endif //DEBUG
      // call the drivers close
   err = OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Dispatch.pRead(
			                     ((POSDRV_HANDLE_INTERNAL)H)->DevIndex,
                                 OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].pContext,
                                 pBuffer,
                                 pLength);
      // undo serialization                                 
   if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Config.Exclusive) {
       // serialize
       errsem = OSSemPost(((POSDRV_HANDLE_INTERNAL)H)->Event);
   }
   
   return err;
}

/*
   Write - wead a buffer to the driver
*/
INT8U  Write(HANDLE H, PVOID pBuffer, INT32U* pLength) 
{
  INT8U err;
  INT8U errsem;

      // check for invalid handle
   if (H == OS_DRV_INVALID_HANDLE)  {
   	   DEBUGMSG(TRUE, ("OSDRV:Write Invalid handle\n\r"));   
	   return OS_DRV_NO_DEVICE;   
   }
      // check for serialization
   if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Config.Exclusive) {
       // serialize
       OSSemPend(((POSDRV_HANDLE_INTERNAL)H)->Event, 0, &errsem);
   }
#ifdef DEBUG
    if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Dispatch.pWrite == NULL) {
        // driver has no write
        DEBUGMSG(TRUE, ("OSDRV:Write write not supported, error: %d\n\r", OS_DRV_UNSUPPORTED));   
        return OS_DRV_UNSUPPORTED;
    }
#endif //DEBUG

       // call the drivers write
   err = OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Dispatch.pWrite(
			                     ((POSDRV_HANDLE_INTERNAL)H)->DevIndex,
                                 OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].pContext,
                                 pBuffer,
                                 pLength);
      // undo serialization                                 
   if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Config.Exclusive) {
       // serialize
       errsem = OSSemPost(((POSDRV_HANDLE_INTERNAL)H)->Event);
   }
   
   return err;
}

/*
   Ioctl - IOCTL to the driver
*/
INT8U  Ioctl(HANDLE H, INT8U Control, PVOID pBuffer, INT32U* pLength)
{
  INT8U err;
  INT8U errsem;

      // check for invalid handle
   if (H == OS_DRV_INVALID_HANDLE)  {
   	   DEBUGMSG(TRUE, ("OSDRV:Ioctl Invalid handle\n\r"));   
	   return OS_DRV_NO_DEVICE;   
   }
      // check for serialization
   if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Config.Exclusive) {
       // serialize
       OSSemPend(((POSDRV_HANDLE_INTERNAL)H)->Event, 0, &errsem);
   }
#ifdef DEBUG
    if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Dispatch.pIoctl == NULL) {
        // driver has no close
        DEBUGMSG(TRUE, ("OSDRV:Ioctl ioctl not supported, error: %d\n\r", OS_DRV_UNSUPPORTED));   
        return OS_DRV_UNSUPPORTED;
    }
#endif //DEBUG

      // call the drivers close
   err = OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Dispatch.pIoctl(
			                     ((POSDRV_HANDLE_INTERNAL)H)->DevIndex,
                                 OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].pContext,
                                 Control,
                                 pBuffer,
                                 pLength);
      // undo serialization                                 
   if (OSDRV_pDriverTable[((POSDRV_HANDLE_INTERNAL)H)->DrvIndex].Config.Exclusive) {
       // serialize
       errsem = OSSemPost(((POSDRV_HANDLE_INTERNAL)H)->Event);
   }
   
   return err;
}



/*
     Utility Functions
*/
/*
   OSDRV_AddName - utility function to add device name to devices supported list of devices
*/
INT8U OSDRV_AddName(POSDRV_CONFIG pConfig, char * pName)
{
  POSDRV_NAME pNextItem;
  
	// find first empty list entry
  for(pNextItem = &pConfig->Name; 
      (pNextItem->pNext != NULL) && (pNextItem->pName != NULL); 
      pNextItem = pNextItem->pNext) {
    ; // nothing to do
  }
  
  if ((pNextItem->pNext == NULL) &&
      (pNextItem->pName != NULL)) {
      // name in use, allocate the next buffer
      // allocate a buffer the next entry and initialize
     pNextItem->pNext = DRV_MEMALLOC(sizeof(OSDRV_CONFIG));
     pNextItem = pNextItem->pNext;
     if (pNextItem == NULL) {
        return OS_DRV_BUFFER_SIZE; // no memory available
     }
     pNextItem->pNext = NULL;
  }
  
    // allocate a buffer for the name, saving space for our prefix
  pNextItem->pName = DRV_MEMALLOC(strlen(pName) + strlen("\\dev\\") + 1);
  
    // copy and concatinate the name
    // the name ends up as \dev\devname
  strcpy(pNextItem->pName, "\\dev\\");  
  strcat(pNextItem->pName, pName);
  
  return OS_DRV_NO_ERR;
}


