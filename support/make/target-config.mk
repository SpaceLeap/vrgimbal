# Board-specific configuration values.  Flash and SRAM sizes in bytes.

ifeq ($(BOARD), laserlab_VR_GIMBAL_F1)
   MCU            := STM32F103RC
   MCU_TYPE       := STM32F10x
   MCU_DENSITY    := STM32F10X_HD
   DENSITY        := STM32_HIGH_DENSITY
   FLASH_SIZE     := 262144
   SRAM_SIZE      := 49152
endif


# Memory target-specific configuration values

ifeq ($(MEMORY_TARGET), ram)
   LDSCRIPT := $(BOARD)/ram.ld
   VECT_BASE_ADDR := VECT_TAB_RAM
endif
ifeq ($(MEMORY_TARGET), flash)
   LDSCRIPT := $(BOARD)/flash.ld
   VECT_BASE_ADDR := VECT_TAB_FLASH
endif
ifeq ($(MEMORY_TARGET), jtag)
   LDSCRIPT := $(BOARD)/jtag.ld
   VECT_BASE_ADDR := VECT_TAB_BASE
endif
ifeq ($(MEMORY_TARGET), dfu)
   LDSCRIPT := $(BOARD)/dfu.ld
   VECT_BASE_ADDR := VECT_TAB_FLASH
endif