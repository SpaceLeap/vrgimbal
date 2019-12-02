# Try "make help" first

.DEFAULT_GOAL := all

SRCROOT := .


IMU_TYPE := EVV # AP
CONFIG := VR_Gimbal_1_13

ifeq ($(CONFIG), VR_Gimbal_1_13)
BOARD            := laserlab_VR_GIMBAL_F1
PROJECT          := VRGimbal
endif
FIRMWARE_PATH    := $(SRCROOT)/Firmware/$(PROJECT)

#MEMORY_TARGET    ?= jtag
MEMORY_TARGET    ?= dfu

##
## Useful paths, constants, etc.
##

BUILD_PATH       := $(SRCROOT)/$(FIRMWARE_PATH)/build
# Support files for linker and Makefile
SUPPORT_PATH     := $(SRCROOT)/support
# Support files for linker
LDDIR            := $(SUPPORT_PATH)/ld
# Support files for this Makefile
MAKEDIR          := $(SUPPORT_PATH)/make

# USB ID for DFU upload
VENDOR_ID  := 1EAF
PRODUCT_ID := 0003

# $(BOARD)- and $(MEMORY_TARGET)-specific configuration
include $(MAKEDIR)/target-config.mk

# Path of source code
HARDWARE_PATH    := $(SRCROOT)/hardware
HAL_PATH         := $(HARDWARE_PATH)/hal
ifeq ($(MCU_TYPE), STM32F10x)
STM32_PATH       := $(HARDWARE_PATH)/STM32F10x_StdPeriph_Lib_V3.5.0
cpu_flags        := -mcpu=cortex-m3 -march=armv7-m
endif
ifeq ($(MCU_TYPE), STM32F4xx)
STM32_PATH       := $(HARDWARE_PATH)/STM32F4xx_DSP_StdPeriph_Lib_V1.0.1
cpu_flags        := -mcpu=cortex-m4
endif
ifeq ($(MCU_TYPE), STM32L1xx)
STM32_PATH       := $(HARDWARE_PATH)/STM32L1xx_StdPeriph_Lib_V1.1.1
cpu_flags        := -mcpu=cortex-m3 -march=armv7-m
endif

WIRISH_PATH      := $(SRCROOT)/wirish
LIBRARIES_PATH   := $(SRCROOT)/Libraries

LIBRARY_INCLUDES := 

##
## Compilation flags
##

GLOBAL_FLAGS    := -DBOARD_$(BOARD)
GLOBAL_FLAGS    += -DBOARDCONFIG_$(CONFIG)
GLOBAL_FLAGS    += -DMCU_$(MCU)
GLOBAL_FLAGS    += -DMCU_TYPE_$(MCU_TYPE)
GLOBAL_FLAGS    += -D$(MCU_DENSITY)
GLOBAL_FLAGS    += -D$(DENSITY)
GLOBAL_FLAGS    += -D$(VECT_BASE_ADDR)
GLOBAL_FLAGS    += -DUSE_STDPERIPH_DRIVER
#GLOBAL_FLAGS    += -DHSE_VALUE=8000000
# GLOBAL_CFLAGS -----------------------------------------------------------------------------------
GLOBAL_CFLAGS   := $(cpu_flags)
GLOBAL_CFLAGS   += -mthumb                #Generate code for the Thumb instruction set
GLOBAL_CFLAGS   += -Wall                  #This enables all the warnings about constructions that some users consider questionable, and that are easy to avoid (or modify to prevent the warning), even in conjunction with macros
#GLOBAL_CFLAGS   += -ggdb                  #Produce debugging information in the operating system’s native format
#GLOBAL_CFLAGS   +=  -O2					#Compilazione con ottimizzazione per velocità di esecuzione
GLOBAL_CFLAGS   += -ffunction-sections
GLOBAL_CFLAGS   += -fdata-sections
GLOBAL_CFLAGS   += $(GLOBAL_FLAGS)
# GLOBAL_CXXFLAGS ---------------------------------------------------------------------------------
GLOBAL_CXXFLAGS := -fpermissive           #Downgrade some diagnostics about nonconformant code from errors to warnings. Thus, using ‘-fpermissive’ will allow some nonconforming code to compile.
GLOBAL_CXXFLAGS += -Wno-psabi
GLOBAL_CXXFLAGS += $(GLOBAL_FLAGS)
# GLOBAL_ASFLAGS ----------------------------------------------------------------------------------
GLOBAL_ASFLAGS  := $(cpu_flags)
GLOBAL_ASFLAGS  += -mthumb
GLOBAL_ASFLAGS  += -x assembler-with-cpp
GLOBAL_ASFLAGS  += $(GLOBAL_FLAGS)
# GLOBAL_LDFLAGS ----------------------------------------------------------------------------------
LDFLAGS         := $(cpu_flags)
LDFLAGS         += -mthumb
LDFLAGS         += -static
LDFLAGS         += -Wall
LDFLAGS         += -Wl,--gc-sections
LDFLAGS         += -Wl,--cref
LDFLAGS         += -Wl,-Map,$(BUILD_PATH)/$(PROJECT).map
LDFLAGS         += -T$(LDDIR)/$(LDSCRIPT)
LDFLAGS         += -L$(LDDIR)

##
## Build rules and useful templates
##

include $(SUPPORT_PATH)/make/build-rules.mk
include $(SUPPORT_PATH)/make/build-templates.mk

##
## Set all submodules here
##

# Try to keep LIBMAPLE_MODULES a simply-expanded variable
# Official libraries:
LIBRARY_MODULES := 
LIBRARY_MODULES := $(STM32_PATH)
LIBRARY_MODULES += $(HAL_PATH)
LIBRARY_MODULES += $(WIRISH_PATH)

# Official libraries:
ifeq ($(IMU_TYPE), AP)
LIBRARY_MODULES += $(LIBRARIES_PATH)/AC_PID
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_ADC
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_AHRS
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_AnalogSource
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Airspeed
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Baro
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Buffer
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Common
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Compass
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Declination
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_GPS
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Camera
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Limits
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_GSM
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_InertialSensor
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_InertialNav
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_LANC
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Math
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Menu
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Motors
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Mount
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Navigation
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_OpticalFlow
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_PeriodicProcess
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_PerfMon
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Progmem
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_RangeFinder
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Relay
#LIBRARY_MODULES += $(LIBRARIES_PATH)/APM_PI
LIBRARY_MODULES += $(LIBRARIES_PATH)/APM_RC
LIBRARY_MODULES += $(LIBRARIES_PATH)/Arduino_Mega_ISR_Registry
#LIBRARY_MODULES += $(LIBRARIES_PATH)/DataFlash
LIBRARY_MODULES += $(LIBRARIES_PATH)/EEPROM
LIBRARY_MODULES += $(LIBRARIES_PATH)/Filter
#LIBRARY_MODULES += $(LIBRARIES_PATH)/GCS_Mavlink
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_LeadFilter
#LIBRARY_MODULES += $(LIBRARIES_PATH)/GPS_IMU
#LIBRARY_MODULES += $(LIBRARIES_PATH)/mapleSDfat
#LIBRARY_MODULES += $(LIBRARIES_PATH)/ME_Armed
#LIBRARY_MODULES += $(LIBRARIES_PATH)/memcheck
#LIBRARY_MODULES += $(LIBRARIES_PATH)/MP32_WNK
#LIBRARY_MODULES += $(LIBRARIES_PATH)/PID
#LIBRARY_MODULES += $(LIBRARIES_PATH)/RC_Channel
#LIBRARY_MODULES += $(LIBRARIES_PATH)/Trig_LUT
#LIBRARY_MODULES += $(LIBRARIES_PATH)/Waypoints
#LIBRARY_MODULES += $(LIBRARIES_PATH)/I2C
else
#IMU_TYPE NOT AP

LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Common
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Compass
#LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Declination
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Math
LIBRARY_MODULES += $(LIBRARIES_PATH)/AP_Progmem
LIBRARY_MODULES += $(LIBRARIES_PATH)/APM_RC
LIBRARY_MODULES += $(LIBRARIES_PATH)/Arduino_Mega_ISR_Registry
LIBRARY_MODULES += $(LIBRARIES_PATH)/EEPROM
endif

# Firmware libraries:
LIBRARY_MODULES += $(FIRMWARE_PATH)
# //TEO  aggiunta cartella file comuni per tutte le board
#LIBRARY_MODULES += $(SRCROOT)/Firmware/Common
#LIBRARY_MODULES += $(SRCROOT)/Firmware/Drivers
#LIBRARY_MODULES += $(SRCROOT)/Firmware/BruGi

# Call each module's rules.mk:
$(foreach m,$(LIBRARY_MODULES),$(eval $(call LIBRARY_MODULE_template,$(m))))

##
## Targets
##

# main target
include $(SRCROOT)/build-targets.mk

.PHONY: install all clean

# Target upload commands
UPLOAD_ram   := $(SUPPORT_PATH)/scripts/reset.py && sleep 1 && $(DFU) -a0 -d $(VENDOR_ID):$(PRODUCT_ID) -D $(BUILD_PATH)/$(PROJECT).bin -R
UPLOAD_flash := $(DFU) -a1 -d $(VENDOR_ID):$(PRODUCT_ID) -D $(BUILD_PATH)/$(PROJECT).bin -R
UPLOAD_dfu   := $(DFU) -a1 -d $(VENDOR_ID):$(PRODUCT_ID) -D $(BUILD_PATH)/$(PROJECT).bin -R
UPLOAD_jtag  := $(OPENOCD_WRAPPER) flash

# Conditionally upload to whatever the last build was
install: INSTALL_TARGET = $(shell cat $(BUILD_PATH)/build-type 2>/dev/null)
install: all
	@echo "Install target:" $(INSTALL_TARGET)
	$(UPLOAD_$(INSTALL_TARGET))

# Force a rebuild if the target changed
PREV_BUILD_TYPE = $(shell cat $(BUILD_PATH)/build-type 2>/dev/null)
build-check:
ifneq ($(PREV_BUILD_TYPE), $(MEMORY_TARGET))
    $(shell rm -rf $(BUILD_PATH))
endif

all: build-check MSG_INFO $(BUILD_PATH)/$(PROJECT).bin

clean:
	rm -rf $(BUILD_PATH)

