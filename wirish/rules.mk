# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
BUILDDIRS       += $(BUILD_PATH)/$(d)
BUILDDIRS       += $(BUILD_PATH)/$(d)/comm
BUILDDIRS       += $(BUILD_PATH)/$(d)/boards/$(BOARD)

LIBRARY_INCLUDES += -I$(d)/comm -I$(d)/boards/$(BOARD)

# Local flags
CFLAGS_$(d) := 

# Local rules and targets
cSRCS_$(d)   :=  
ifeq ($(MCU_TYPE), STM32F10x)
cSRCS_$(d)   += boards/$(BOARD)/stm32f10x_it.c
cSRCS_$(d)   += boards/$(BOARD)/system_stm32f10x.c
endif
ifeq ($(MCU_TYPE), STM32F4xx)
cSRCS_$(d)   += boards/$(BOARD)/stm32f4xx_it.c
cSRCS_$(d)   += boards/$(BOARD)/system_stm32f4xx.c
endif
ifeq ($(MCU_TYPE), STM32L1xx)
cSRCS_$(d)   += boards/$(BOARD)/stm32l1xx_it.c
cSRCS_$(d)   += boards/$(BOARD)/system_stm32l1xx.c
endif

cppSRCS_$(d) := 
cppSRCS_$(d) += boards/$(BOARD)/$(BOARD).cpp

cppSRCS_$(d) += comm/BetterStream.cpp
#cppSRCS_$(d) += comm/DmaChannel.cpp
cppSRCS_$(d) += comm/FastSerial.cpp
cppSRCS_$(d) += comm/HardwareI2C.cpp
cppSRCS_$(d) += comm/HardwareSPI.cpp
cppSRCS_$(d) += comm/USBSerial.cpp

cppSRCS_$(d) += boards.cpp
cppSRCS_$(d) += cxxabi-compat.cpp
cppSRCS_$(d) += ext_interrupts.cpp
cppSRCS_$(d) += HardwareTimer.cpp
cppSRCS_$(d) += Print.cpp
cppSRCS_$(d) += pwm.cpp
#cppSRCS_$(d) += RCInput.cpp
cppSRCS_$(d) += wirish_analog.cpp
cppSRCS_$(d) += wirish_digital.cpp
cppSRCS_$(d) += wirish_math.cpp
cppSRCS_$(d) += wirish_mem.cpp
cppSRCS_$(d) += wirish_shift.cpp
cppSRCS_$(d) += wirish_time.cpp
cppSRCS_$(d) += wirish_watchdog.cpp

sSRCS_$(d)   := 
sSRCS_$(d)   += boards/$(BOARD)/$(BOARD)_startup.s

cFILES_$(d)   := $(cSRCS_$(d):%=$(d)/%)
cppFILES_$(d) := $(cppSRCS_$(d):%=$(d)/%)
sFILES_$(d)   := $(sSRCS_$(d):%=$(d)/%)

OBJS_$(d)     := $(cFILES_$(d):%.c=$(BUILD_PATH)/%.o)
OBJS_$(d)     += $(cppFILES_$(d):%.cpp=$(BUILD_PATH)/%.o)
OBJS_$(d)     += $(sFILES_$(d):%.s=$(BUILD_PATH)/%.o)

DEPS_$(d)     := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
