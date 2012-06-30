#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!    
#
##############################################################################################
# 
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#

##############################################################################################
# Start of default section
#

TRGT = arm-elf-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
#AS   = $(TRGT)gcc -x assembler-with-cpp
AS   = $(TRGT)as
BIN  = $(CP) -O ihex 
BIN2 = $(CP) -O binary 

MCU  = arm7tdmi

# List all default C defines here, like -D_DEBUG=1
DDEFS = -DDEBUG=1

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = 

# List all default directories to look for include files here
#DINCDIR = ./  \paull\UW\tools\yagarto\arm-elf\include 
DINCDIR =

# List the default directory to look for the libraries here
##DLIBDIR = C:\paull\UW\tools\newlib\newlib-1.15.0

# List all default libraries here
######DLIBS = -lgcov -lgcc
DLIBS =  -lgcc

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

# Define project name here
PROJECT = MP3

# Define linker script file here
#LDSCRIPT= ./flash.cmd
LDSCRIPT= ./lpc2378_flash.ld

# List all user C define here, like -D_DEBUG=1
UDEFS =  

# Define ASM defines here
UADEFS = 

# List C source files here
SRC  = BSP/init.c BSP/bsp.c BSP/print.c BSP/drivers/base/driver.c BSP/drivers/SPI/drv_spi.c BSP/drivers/MP3/drv_mp3.c BSP/mem.c UCOS/os_core.c UCOS/os_flag.c UCOS/os_mbox.c UCOS/os_mem.c UCOS/os_mutex.c UCOS/os_q.c UCOS/os_sem.c UCOS/os_task.c UCOS/os_time.c UCOS/os_tmr.c ./BSP/os_cpu_c.c app/tasks.c app/main.c 

# List ASM source files here
ASRC = ./BSP/crt.s ./BSP/cpu_a.s ./BSP/os_cpu_a.s 
#ASRC = ./BSP/crt.s_old.s ./BSP/cpu_a.s ./BSP/os_cpu_a.s 

# List all user directories here
UINCDIR = app/ BSP/ inc/ UCOS/ UCOS/lib/ BSP/drivers/inc

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS = 

# Define optimisation level here
OPT = -O0

#
# End of user defines
##############################################################################################


INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
DEFS    = $(DDEFS) $(UDEFS)
ADEFS   = $(DADEFS) $(UADEFS)
OBJS    = $(ASRC:.s=.o) $(SRC:.c=.o)
LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU)

#ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
ASFLAGS = $(MCFLAGS) -gdwarf2 -mapcs-32 -amhls=$(<:.s=.lst) $(ADEFS)
CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb-interwork -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
LDFLAGS = $(MCFLAGS) -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch $(LIBDIR) -v

# Generate dependency information
CPFLAGS += -MD -MP -MF .dep/$(@F).d

#
# makefile rules
#

all: $(OBJS) $(PROJECT).elf $(PROJECT).hex $(PROJECT).bin

#%o : %s
#	$(AS) -c $(ASFLAGS) $< -o $@
%o : %s
	$(AS)  $(ASFLAGS) $< -o $@

%o : %c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@


%elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(BIN) $< $@

%bin: %elf
	$(BIN2) $< $@

clean:
	-rm -f $(OBJS)
	-rm -f $(PROJECT).elf
	-rm -f $(PROJECT).map
	-rm -f $(PROJECT).hex
	-rm -f $(PROJECT).bin
	-rm -f $(SRC:.c=.c.bak)
	-rm -f $(SRC:.c=.lst)
	-rm -f $(ASRC:.s=.s.bak)
	-rm -f $(ASRC:.s=.lst)
	-rm -fR .dep

# 
# Include the dependency files, should be the last of the makefile
#
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***