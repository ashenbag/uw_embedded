/*
*********************************************************************************************************
*                                               uC/CPU
*                                    CPU CONFIGURATION & PORT LAYER
*
*                          (c) Copyright 2004-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/CPU is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/CPU in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to
*               help you experience uC/CPU.  The fact that the source code is provided
*               does NOT mean that you can use it without paying a licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            CPU PORT FILE
*
*                                                 ARM
*                                            IAR C Compiler
*  
* Filename      : cpu.h
* Version       : V1.17
* Programmer(s) : ITJ
*                 JJL
*                 JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef  CPU_CFG_MODULE_PRESENT
#define  CPU_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*                                          CPU INCLUDE FILES
*
* Note(s) : (1) The following CPU files are located in the following directories :
*
*               (a) \<CPU-Compiler Directory>\cpu_def.h
*
*               (b) \<CPU-Compiler Directory>\<cpu>\<compiler>\cpu*.*
*
*                       where
*                               <CPU-Compiler Directory>    directory path for common   CPU-compiler software
*                               <cpu>                       directory name for specific CPU
*                               <compiler>                  directory name for specific compiler
*
*           (2) Compiler MUST be configured to include the '\<CPU-Compiler Directory>\' directory & the
*               specific CPU-compiler directory as additional include path directories.
*********************************************************************************************************
*/

#include  <cpu_def.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                    CONFIGURE STANDARD DATA TYPES
*
* Note(s) : (1) Configure standard data types according to CPU-/compiler-specifications.
*
*           (2) (a) (1) 'CPU_FNCT_VOID' data type defined to replace the commonly-used function pointer
*                       data type of a pointer to a function which returns void & has no arguments.
*
*                   (2) Example function pointer usage :
*
*                           CPU_FNCT_VOID  FnctName;
*
*                           FnctName();
*
*               (b) (1) 'CPU_FNCT_PTR'  data type defined to replace the commonly-used function pointer
*                       data type of a pointer to a function which returns void & has a single void 
*                       pointer argument.
*
*                   (2) Example function pointer usage :
*
*                           CPU_FNCT_PTR   FnctName;
*                           void          *pobj
*
*                           FnctName(pobj);
*********************************************************************************************************
*/

typedef            void      CPU_VOID;
typedef  unsigned  char      CPU_CHAR;                          /*  8-bit character                                     */
typedef  unsigned  char      CPU_BOOLEAN;                       /*  8-bit boolean or logical                            */
typedef  unsigned  char      CPU_INT08U;                        /*  8-bit unsigned integer                              */
typedef    signed  char      CPU_INT08S;                        /*  8-bit   signed integer                              */
typedef  unsigned  short     CPU_INT16U;                        /* 16-bit unsigned integer                              */
typedef    signed  short     CPU_INT16S;                        /* 16-bit   signed integer                              */
typedef  unsigned  int       CPU_INT32U;                        /* 32-bit unsigned integer                              */
typedef    signed  int       CPU_INT32S;                        /* 32-bit   signed integer                              */
typedef            float     CPU_FP32;                          /* 32-bit floating point                                */
typedef            double    CPU_FP64;                          /* 64-bit floating point                                */


//??
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef signed   char  INT8;                     /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef signed   short INT16;                    /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef signed   int   INT32;                    /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */
#ifndef FALSE
#define FALSE          0
#endif
#ifndef TRUE
#define TRUE          1
#endif
//??

typedef            void    (*CPU_FNCT_VOID)(void);              /* See Note #2a.                                        */
typedef            void    (*CPU_FNCT_PTR )(void *);            /* See Note #2b.                                        */


/*$PAGE*/
/*
*********************************************************************************************************
*                                       CPU WORD CONFIGURATION
*
* Note(s) : (1) Configure CPU_CFG_ADDR_SIZE & CPU_CFG_DATA_SIZE with CPU's word sizes :
*
*                   CPU_WORD_SIZE_08             8-bit word size
*                   CPU_WORD_SIZE_16            16-bit word size
*                   CPU_WORD_SIZE_32            32-bit word size
*                   CPU_WORD_SIZE_64            64-bit word size            See Note #1a
*
*               (a) 64-bit word size NOT currently supported.
*
*           (2) Configure CPU_CFG_ENDIAN_TYPE with CPU's data-word-memory order :
*
*                   CPU_ENDIAN_TYPE_BIG         Big-   endian word order (CPU words' most  significant
*                                                                         octet @ lowest memory address)
*                   CPU_ENDIAN_TYPE_LITTLE      Little-endian word order (CPU words' least significant
*                                                                         octet @ lowest memory address)
*********************************************************************************************************
*/

                                                                /* Define  CPU         word sizes (see Note #1) :       */
#define  CPU_CFG_ADDR_SIZE              CPU_WORD_SIZE_32        /* Defines CPU address word size.                       */

#define  CPU_CFG_DATA_SIZE              CPU_WORD_SIZE_32        /* Defines CPU data    word size.                       */
#define  CPU_CFG_ENDIAN_TYPE            CPU_ENDIAN_TYPE_LITTLE  /* Defines CPU data    word-memory order.               */


/*
*********************************************************************************************************
*                                 CONFIGURE CPU ADDRESS & DATA TYPES
*********************************************************************************************************
*/

                                                                /* CPU address type based on address bus size.          */
#if     (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_32)
typedef  CPU_INT32U  CPU_ADDR;
#elif   (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_16)
typedef  CPU_INT16U  CPU_ADDR;
#else
typedef  CPU_INT08U  CPU_ADDR;
#endif

                                                                /* CPU data    type based on data    bus size.          */
#if     (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_32)
typedef  CPU_INT32U  CPU_DATA;
#elif   (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_16)
typedef  CPU_INT16U  CPU_DATA;
#else
typedef  CPU_INT08U  CPU_DATA;
#endif


typedef  CPU_DATA    CPU_ALIGN;                                 /* Defines CPU data-word-alignment size.                */
typedef  CPU_DATA    CPU_SIZE_T;                                /* Defines CPU standard 'size_t'   size.                */


/*$PAGE*/
/*
*********************************************************************************************************
*                                   CRITICAL SECTION CONFIGURATION
*
* Note(s) : (1) Configure CPU_CFG_CRITICAL_METHOD with CPU's/compiler's critical section method :
*
*                                                       Enter/Exit critical sections by ...
*
*                   CPU_CRITICAL_METHOD_INT_DIS_EN      Disable/Enable interrupts
*                   CPU_CRITICAL_METHOD_STATUS_STK      Push/Pop       interrupt status onto stack
*                   CPU_CRITICAL_METHOD_STATUS_LOCAL    Save/Restore   interrupt status to local variable
*
*               (a) CPU_CRITICAL_METHOD_INT_DIS_EN  is NOT a preferred method since it does NOT support 
*                   multiple levels of interrupts.  However, with some CPUs/compilers, this is the only 
*                   available method.
*
*               (b) CPU_CRITICAL_METHOD_STATUS_STK    is one preferred method since it DOES support multiple 
*                   levels of interrupts.  However, this method assumes that the compiler allows in-line 
*                   assembly AND will correctly modify the local stack pointer when interrupt status is 
*                   pushed/popped onto the stack.
*
*               (c) CPU_CRITICAL_METHOD_STATUS_LOCAL  is one preferred method since it DOES support multiple 
*                   levels of interrupts.  However, this method assumes that the compiler provides C-level 
*                   &/or assembly-level functionality for the following :
*
*                     ENTER CRITICAL SECTION :
*                       (a) Save    interrupt status into a local variable
*                       (b) Disable interrupts
*
*                     EXIT  CRITICAL SECTION :
*                       (c) Restore interrupt status from a local variable
*
*           (2) Critical section macro's most likely require inline assembly.  If the compiler does NOT 
*               allow inline assembly in C source files, critical section macro's MUST call an assembly 
*               subroutine defined in a 'cpu_a.asm' file located in the following software directory :
*
*                   \<CPU-Compiler Directory>\<cpu>\<compiler>\
*
*                       where
*                               <CPU-Compiler Directory>    directory path for common   CPU-compiler software
*                               <cpu>                       directory name for specific CPU
*                               <compiler>                  directory name for specific compiler
*
*           (3) To save/restore interrupt status, a local variable 'cpu_sr' of type 'CPU_SR' MAY need to
*               be declared (e.g. if 'CPU_CRITICAL_METHOD_STATUS_LOCAL' method is configured).  Configure
*               'CPU_SR' data type with the appropriate-sized CPU data type large enough to completely 
*               store the CPU's/compiler's status word.
*********************************************************************************************************
*/

typedef  CPU_INT32U  CPU_SR;                                    /* Defines   CPU status register size (see Note #3).    */

                                                                /* Configure CPU critical method      (see Note #1) :   */
#define  CPU_CFG_CRITICAL_METHOD        CPU_CRITICAL_METHOD_STATUS_LOCAL
#define  OS_CRITICAL_METHOD    3
//??#define  CPU_CRITICAL_ENTER()           { cpu_sr = CPU_SR_Save(); }
//??#define  CPU_CRITICAL_EXIT()            { CPU_SR_Restore(cpu_sr); }

#if      OS_CPU_INT_DIS_MEAS_EN > 0

#define  OS_ENTER_CRITICAL()  {cpu_sr = OCPU_SR_Save();  \
                               OS_CPU_IntDisMeasStart();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_IntDisMeasStop();   \
                               CPU_SR_Restore(cpu_sr);}

#else

#define  OS_ENTER_CRITICAL()  {cpu_sr = CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {CPU_SR_Restore(cpu_sr);}

#endif


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_SR  CPU_SR_Save   (void);
void    CPU_SR_Restore(CPU_SR  cpu_sr);


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef   CPU_CFG_ADDR_SIZE
#error   "CPU_CFG_ADDR_SIZE              not #define'd in 'cpu.h'               "
#error   "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error   "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error   "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"

#elif   ((CPU_CFG_ADDR_SIZE != CPU_WORD_SIZE_08) && \
         (CPU_CFG_ADDR_SIZE != CPU_WORD_SIZE_16) && \
         (CPU_CFG_ADDR_SIZE != CPU_WORD_SIZE_32))
#error   "CPU_CFG_ADDR_SIZE        illegally #define'd in 'cpu.h'               "
#error   "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error   "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error   "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"
#endif


#ifndef   CPU_CFG_DATA_SIZE
#error   "CPU_CFG_DATA_SIZE              not #define'd in 'cpu.h'               "
#error   "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error   "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error   "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"

#elif   ((CPU_CFG_DATA_SIZE != CPU_WORD_SIZE_08) && \
         (CPU_CFG_DATA_SIZE != CPU_WORD_SIZE_16) && \
         (CPU_CFG_DATA_SIZE != CPU_WORD_SIZE_32))
#error   "CPU_CFG_DATA_SIZE        illegally #define'd in 'cpu.h'               "
#error   "                         [MUST be  CPU_WORD_SIZE_08   8-bit alignment]"
#error   "                         [     ||  CPU_WORD_SIZE_16  16-bit alignment]"
#error   "                         [     ||  CPU_WORD_SIZE_32  32-bit alignment]"
#endif



#ifndef   CPU_CFG_ENDIAN_TYPE
#error   "CPU_CFG_ENDIAN_TYPE            not #define'd in 'cpu.h'   "
#error   "                         [MUST be  CPU_ENDIAN_TYPE_BIG   ]"
#error   "                         [     ||  CPU_ENDIAN_TYPE_LITTLE]"

#elif   ((CPU_CFG_ENDIAN_TYPE != CPU_ENDIAN_TYPE_BIG   ) && \
         (CPU_CFG_ENDIAN_TYPE != CPU_ENDIAN_TYPE_LITTLE))
#error   "CPU_CFG_ENDIAN_TYPE      illegally #define'd in 'cpu.h'   "
#error   "                         [MUST be  CPU_ENDIAN_TYPE_BIG   ]"
#error   "                         [     ||  CPU_ENDIAN_TYPE_LITTLE]"
#endif




#ifndef   CPU_CFG_CRITICAL_METHOD
#error   "CPU_CFG_CRITICAL_METHOD        not #define'd in 'cpu.h'             "
#error   "                         [MUST be  CPU_CRITICAL_METHOD_INT_DIS_EN  ]"
#error   "                         [     ||  CPU_CRITICAL_METHOD_STATUS_STK  ]"
#error   "                         [     ||  CPU_CRITICAL_METHOD_STATUS_LOCAL]"

#elif   ((CPU_CFG_CRITICAL_METHOD != CPU_CRITICAL_METHOD_INT_DIS_EN  ) && \
         (CPU_CFG_CRITICAL_METHOD != CPU_CRITICAL_METHOD_STATUS_STK  ) && \
         (CPU_CFG_CRITICAL_METHOD != CPU_CRITICAL_METHOD_STATUS_LOCAL))
#error   "CPU_CFG_CRITICAL_METHOD  illegally #define'd in 'cpu.h'             "
#error   "                         [MUST be  CPU_CRITICAL_METHOD_INT_DIS_EN  ]"
#error   "                         [     ||  CPU_CRITICAL_METHOD_STATUS_STK  ]"
#error   "                         [     ||  CPU_CRITICAL_METHOD_STATUS_LOCAL]"
#endif


/* ARM exception IDs                                    */
#define  OS_CPU_ARM_EXCEPT_RESET                                                                    0x00
#define  OS_CPU_ARM_EXCEPT_UNDEF_INSTR                                                              0x01
#define  OS_CPU_ARM_EXCEPT_SWI                                                                      0x02
#define  OS_CPU_ARM_EXCEPT_PREFETCH_ABORT                                                           0x03
#define  OS_CPU_ARM_EXCEPT_DATA_ABORT                                                               0x04
#define  OS_CPU_ARM_EXCEPT_ADDR_ABORT                                                               0x05
#define  OS_CPU_ARM_EXCEPT_IRQ                                                                      0x06
#define  OS_CPU_ARM_EXCEPT_FIQ                                                                      0x07
#define  OS_CPU_ARM_EXCEPT_NBR                                                                      0x08

/* ARM exception vectors addresses                      */
#define  OS_CPU_ARM_EXCEPT_RESET_VECT_ADDR              (OS_CPU_ARM_EXCEPT_RESET          * 0x04 + 0x00)
#define  OS_CPU_ARM_EXCEPT_UNDEF_INSTR_VECT_ADDR        (OS_CPU_ARM_EXCEPT_UNDEF_INSTR    * 0x04 + 0x00)
#define  OS_CPU_ARM_EXCEPT_SWI_VECT_ADDR                (OS_CPU_ARM_EXCEPT_SWI            * 0x04 + 0x00)
#define  OS_CPU_ARM_EXCEPT_PREFETCH_ABORT_VECT_ADDR     (OS_CPU_ARM_EXCEPT_PREFETCH_ABORT * 0x04 + 0x00)
#define  OS_CPU_ARM_EXCEPT_DATA_ABORT_VECT_ADDR         (OS_CPU_ARM_EXCEPT_DATA_ABORT     * 0x04 + 0x00)
#define  OS_CPU_ARM_EXCEPT_ADDR_ABORT_VECT_ADDR         (OS_CPU_ARM_EXCEPT_ADDR_ABORT     * 0x04 + 0x00)
#define  OS_CPU_ARM_EXCEPT_IRQ_VECT_ADDR                (OS_CPU_ARM_EXCEPT_IRQ            * 0x04 + 0x00)
#define  OS_CPU_ARM_EXCEPT_FIQ_VECT_ADDR                (OS_CPU_ARM_EXCEPT_FIQ            * 0x04 + 0x00)

                                                                /* ARM exception handlers addresses                     */
#define  OS_CPU_ARM_EXCEPT_RESET_HANDLER_ADDR           (OS_CPU_ARM_EXCEPT_RESET          * 0x04 + 0x20)
#define  OS_CPU_ARM_EXCEPT_UNDEF_INSTR_HANDLER_ADDR     (OS_CPU_ARM_EXCEPT_UNDEF_INSTR    * 0x04 + 0x20)
#define  OS_CPU_ARM_EXCEPT_SWI_HANDLER_ADDR             (OS_CPU_ARM_EXCEPT_SWI            * 0x04 + 0x20)
#define  OS_CPU_ARM_EXCEPT_PREFETCH_ABORT_HANDLER_ADDR  (OS_CPU_ARM_EXCEPT_PREFETCH_ABORT * 0x04 + 0x20)
#define  OS_CPU_ARM_EXCEPT_DATA_ABORT_HANDLER_ADDR      (OS_CPU_ARM_EXCEPT_DATA_ABORT     * 0x04 + 0x20)
#define  OS_CPU_ARM_EXCEPT_ADDR_ABORT_HANDLER_ADDR      (OS_CPU_ARM_EXCEPT_ADDR_ABORT     * 0x04 + 0x20)
#define  OS_CPU_ARM_EXCEPT_IRQ_HANDLER_ADDR             (OS_CPU_ARM_EXCEPT_IRQ            * 0x04 + 0x20)
#define  OS_CPU_ARM_EXCEPT_FIQ_HANDLER_ADDR             (OS_CPU_ARM_EXCEPT_FIQ            * 0x04 + 0x20)

                                                                /* ARM "Jump To Self" assembled instruction             */
#define  OS_CPU_ARM_INSTR_JUMP_TO_SELF                   0xEAFFFFFE
                                                                /* ARM "Jump To Exception Handler" assembled instruction*/
#define  OS_CPU_ARM_INSTR_JUMP_TO_HANDLER                0xE59FF018


#define  OS_STK_GROWTH        1                   /* Stack grows from HIGH to LOW memory on ARM        */

#define  OS_TASK_SW()         OSCtxSw()

/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of CPU cfg module inclusion.                     */

