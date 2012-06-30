/*
              driver.h
              Header file for driver model example
              Paul Lever

*/

#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "mem.h"

	// define a handle for users
typedef void * PVOID;
typedef PVOID HANDLE;


	// define a handle for internal use
	// (normally we would hide this from users)
typedef struct {
	INT8U    DrvIndex;	// index of driver list for this device
	INT8U    DevIndex;	// index of device within driver
	OS_EVENT *Event;  	// Handle's semaphore
	INT8U	 RefCount;  // reference count for Handle	
} OSDRV_HANDLE_INTERNAL, *POSDRV_HANDLE_INTERNAL;
	
	// device name link list
struct _OSDRV_NAME;	
typedef struct _OSDRV_NAME {
	char 	*pName;		// name of device
	struct _OSDRV_NAME *pNext;	// link to next name
} OSDRV_NAME, *POSDRV_NAME;
	
	// Configuration table for driver initialization
    //   exclusive means that the driver model will only give the driver one request at a time
    //   per handle. A driver that only wants to handle one request per device at a ti8me should
    //   set Exclusive when it registers and only allow one Open per device
typedef struct {
	INT8U	Exclusive;	// TRUE, device accepts only one request at a time
	OSDRV_NAME Name;	// device name, linked list
} OSDRV_CONFIG, *POSDRV_CONFIG;	
	
	// forward reference
struct _OSDRV_DISPATCH;
	// Driver initialization function prototype
typedef INT8U(*POSDRV_INIT)(struct _OSDRV_DISPATCH* pDispatch, PVOID *ppContext, POSDRV_CONFIG pConfig);
	// Driver Open function prototype
typedef INT8U(*POSDRV_OPEN)(INT8U DeviceIndex, PVOID pContext, INT8U Flags);
	// Driver Close function prototype
typedef INT8U(*POSDRV_CLOSE)(INT8U DeviceIndex, PVOID pContext);
	// Driver Read function prototype
typedef INT8U(*POSDRV_READ)(INT8U DeviceIndex, PVOID pContext, PVOID pBuffer, INT32U* pCount);
	// Driver Write function prototype
typedef INT8U(*POSDRV_WRITE)(INT8U DeviceIndex, PVOID pContext, PVOID pBuffer, INT32U* pCount);
	// Driver Ioctl function prototype
typedef INT8U(*POSDRV_IOCTL)(INT8U DeviceIndex, PVOID pContext, INT8U Control, PVOID pBuffer, INT32U* pCount);

	// Dispatch function entry points for a driver
typedef struct _OSDRV_DISPATCH {
  POSDRV_INIT	pInit;		// Initialization function
  POSDRV_OPEN	pOpen;		// Open function
  POSDRV_CLOSE	pClose;		// Close function
  POSDRV_READ	pRead;		// Read function
  POSDRV_WRITE	pWrite;		// Write function
  POSDRV_IOCTL	pIoctl;		// Ioctl function
} OSDRV_DISPATCH, *POSDRV_DISPATCH;


	// the table of drivers to be supported in this build
	// create this table, initializing only the Dispatch.pInit item
	// and pass to OSDRV_Init() on startup
typedef struct {
	OSDRV_DISPATCH Dispatch; // dispatch table for driver
	OSDRV_CONFIG   Config;	 // configuration of device
	PVOID    pContext;       // context for driver, by Handle
	INT8U    Initialized;    // is device initialized
	INT8U	 RefCount;  	 // reference count for driver	
} OSDRV_DRIVER_ENTRY, *POSDRV_DRIVER_ENTRY;


	// define driver specific errors status
	//
	// returned when hanlde was not created
#define OS_DRV_INVALID_HANDLE  ((HANDLE)0xFFFF)
	// returned when device not found or not available
	// (should be unique with ucos_ii.h OS_ errors)
#define OS_DRV_NO_ERR	    OS_NO_ERR	
#define OS_DRV_NO_DEVICE	200
	// driver subsystem not initialized.
#define OS_DRV_NOT_INIT		201
    // request not supported on this device
#define OS_DRV_UNSUPPORTED	202
    // insufficient buffer size
#define OS_DRV_BUFFER_SIZE	203
    // error, device is busy
#define OS_DRV_BUSY			204
    // error, bad data
#define OS_DRV_DATA			205
    // error, device error
#define OS_DRV_DEVICE		206


	// Open Flags
	// open for read
#define OSDRV_READ		0x01
	// open for write
#define OSDRV_WRITE		0x02
	// open for exclsuive
#define OSDRV_EXCLUSIVE	0x04
	
	//driver model memory allocation routines
    // only usable after OS starts
#define DRV_MEMALLOC(x) tmemalloc(x)
#define DRV_MEMFREE(x)  tmemfree(x)

	// function prototypes
	// driver subsystem initialization function
INT8U OSDRV_SubsysInit(POSDRV_DRIVER_ENTRY pDriverTable, INT8U Count);
    // driver utility function prototypes
INT8U OSDRV_AddName(POSDRV_CONFIG pConfig, char * pName);
    

    // prototypes for applications to use
	// Open device
HANDLE Open(char *pName, INT8U Flags);	
INT8U  Close(HANDLE H);	
INT8U  Read(HANDLE H, PVOID pBuffer, INT32U* pLength);	
INT8U  Write(HANDLE H, PVOID pBuffer, INT32U* pLength);	
INT8U  Ioctl(HANDLE H, INT8U Control, PVOID pBuffer, INT32U* pLength);

#endif //__DRIVER__
