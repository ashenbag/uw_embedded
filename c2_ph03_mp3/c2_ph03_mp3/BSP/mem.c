/*
 *      mem.c - temporary memory allocation support
 *   UNTESTED  pdl
 */

#include "includes.h"
#include "mem.h"

struct _MEM_ALLOC;          //forward reference
typedef struct _MEM_HEADER {
    INT32U   Size;        // alloacted size, plus upper bit set for valid
    struct _MEM_ALLOC * pNext;   // next in list
}MEM_HEADER, *PMEM_HEADER;
typedef struct _MEM_ALLOC {
    MEM_HEADER Header;      // header of alloaction
    INT8U    pData;         // allocated buffer    
}MEM_ALLOC, *PMEM_ALLOC;
//in use tag. If size 0, then iniial value
#define MEM_TAG  (1<<31)

//???extern char end asm ("end"); // Defined by the linker
//use a fixed heap size for now
static char heap[MEM_HEAPSIZE];
static char *end = &heap[MEM_HEAPSIZE-1];

static OS_EVENT * SemMemory; //semaphore to protect heap list

/* tmemalloc - allocate memory 
 *             can only be used after UCOS begins
 */
void * tmemalloc(INT32 Size)
{
    static BOOLEAN initialized = FALSE; // set for one time initialization
    static OS_EVENT * SemMemory;        // semaphore to protect from reentrancy
    INT8U  err;
    void * pMem = NULL;
    
    if (!initialized) {
        SemMemory = OSSemCreate(1);
        if (SemMemory == NULL) {
            // just fail
            return NULL;
        }
        initialized = TRUE;
    }
    // protect call with semaphore
    OSSemPend(SemMemory, 0, &err);
    if (err != OS_NO_ERR) {
        // just fail
        return NULL;
    }
    // call real function
    pMem =  _tmemalloc(Size);
    //unprotect
    err = OSSemPost(SemMemory);
    if (err != OS_NO_ERR) {
        // ignore error
    }
    memset(pMem, 0, Size);
    return pMem;
}

/* _tmemalloc - allocate memory 
 *             
 */
void * _tmemalloc(INT32 Size)
{
    static BOOLEAN initialized = FALSE; // set for one time initialization
    PMEM_ALLOC pCurrent;                // current free memory pointer
    static void *pHead = NULL; // head of list
    PMEM_ALLOC pAlloc = NULL;
    INT32U tsize;
    
    if (Size & MEM_TAG) {
        return NULL;                    // too big, like in way too big
    }
    // round the size up to 4-byte aligned
    Size = (Size + 3) & 0xFFFFFFFC;
    
    if (!initialized) {
        pHead = (void *)heap; // head of list
        pAlloc = (PMEM_ALLOC)(pHead);
        pAlloc->Header.Size = MEM_TAG;
        pAlloc->Header.pNext = NULL; 
        initialized = TRUE;
    }
    // we have no checking at this time for an overflow!
    // walk the list looking for a fit or 
    // get the lowest address
    for ( pCurrent = pHead;
         (pCurrent->Header.Size & MEM_TAG);
         pCurrent = pCurrent->Header.pNext) {
        if (!(pCurrent->Header.Size & ~MEM_TAG)) {
            // size has not been set, this is the initial condition case of chunk
            pAlloc = (PMEM_ALLOC)(pCurrent);
            tsize = sizeof(MEM_HEADER) + Size;
            if (((INT32U)pAlloc + tsize) > (INT32U)end) {
                //overflow
                pAlloc = NULL;
                break;
            }
            pAlloc->Header.Size = tsize | MEM_TAG;
            pAlloc->Header.pNext = (PMEM_ALLOC)((INT32U)pAlloc + sizeof(MEM_HEADER) + tsize);
            //initialize next to unknown size
            (pAlloc->Header.pNext)->Header.pNext = NULL;
            (pAlloc->Header.pNext)->Header.Size = MEM_TAG;
            break;
        } else {
            if (!(pCurrent->Header.Size & MEM_TAG)) {
                //free chunk
                tsize = (pCurrent->Header.Size & ~MEM_TAG);
                if (Size <= (tsize + sizeof(MEM_HEADER))) {
                    //fits
                    //use the whole chunk
                    pAlloc = (PMEM_ALLOC)(pCurrent);
                    pAlloc->Header.Size |= MEM_TAG;
                    break;
                }
            }
        }
    }
    return (pAlloc != NULL) ? &pAlloc->pData : NULL; 
}

/* tmemfree - deallocate memory 
 *             can only be used after UCOS begins
 */
void tmemfree(void *pData)
{
    INT8U err;
    
    // protect call with semaphore
    OSSemPend(SemMemory, 0, &err);
    if (err != OS_NO_ERR) {
        // just fail
        return;
    }
    // call real function
    _tmemfree(pData);
    //unprotect
    err = OSSemPost(SemMemory);
    if (err != OS_NO_ERR) {
        // ignore error
    }
}

/* _tmemfree - deallocate memory 
 */
void _tmemfree(void *pData)
{
    PMEM_ALLOC pAlloc = pData - sizeof(MEM_HEADER);
    
    if (pAlloc->Header.Size & MEM_TAG) {
        //clear the in-use flag
        pAlloc->Header.Size &= ~MEM_TAG;
    } else {
        DEBUGMSG(1,("tmemfree: corrupt heap\n\r"));
    }
}
