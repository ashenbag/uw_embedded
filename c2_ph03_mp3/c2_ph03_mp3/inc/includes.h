/*
*********************************************************************************************************
*                                              EXAMPLE CODE
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
*                                           MASTER INCLUDES
*
*                                             NXP LPC2378
*                                                on the
*                                   IAR LPC2378-SK Evaluation Board
*
* Filename      : includes.h
* Version       : V1.00
* Programmer(s) : BAN
*********************************************************************************************************
*/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <math.h>

#include    <lpc2378.h>
#include    <cpu.h>
#include    <lib_def.h>
#include    <lib_mem.h>
#include    <lib_str.h>

#include    <app_cfg.h>
#include    <bsp.h>

#include <ucos_ii.h>

#if (uC_PROBE_OS_PLUGIN > 0)
#include    <os_probe.h>
#endif

#if (uC_PROBE_COM_MODULE > 0)
#include    <probe_com.h>

#if (PROBE_COM_METHOD_RS232 > 0)
#include    <probe_rs232.h>
#endif
#endif




#endif
