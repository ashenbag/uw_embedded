#ifndef __PRINT_H__
#define __PRINT_H__

void printHex(INT32U u32);
void print_uint32(INT32U u);
void printString(char *ptr);
void printStringHex(char *ptr, INT32U u32Hex);
void printChar(const INT8U letter);

//
// Macros for printing debug messages.
//
#define _DBG_PRINTX_ARG(arg...) arg /* unroll the parens around the var args*/
#define RETAILMSG(x,y) \
    ((x) ? (snprintf(stringbuffer, PRINTBUFMAX, _DBG_PRINTX_ARG y), printString(stringbuffer)) : 0)
#define PRINTBUFMAX 128
#define PRINT_DEFINEBUFFER() char stringbuffer[PRINTBUFMAX]
#define PRINT_BUFFER() extern char stringbuffer[]

#ifdef DEBUG
#define DEBUGMSG(x,y) \
    ((x) ? (snprintf(stringbuffer, PRINTBUFMAX, _DBG_PRINTX_ARG y), printString(stringbuffer)) : 0)
#else // DEBUG
    #define DEBUGMSG(x,y) (0)
#endif // DEBUG

#endif /* __PRINT_H__ */
