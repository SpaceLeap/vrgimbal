# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
BUILDDIRS       += $(BUILD_PATH)/$(d)
BUILDDIRS       += $(BUILD_PATH)/$(d)/usb

LIBRARY_INCLUDES += -I$(d)/usb

# Local flags
CFLAGS_$(d) =

# Local rules and targets
cSRCS_$(d)   := 
cSRCS_$(d)   += adc.c
cSRCS_$(d)   += dma.c
cSRCS_$(d)   += exti.c
cSRCS_$(d)   += flash.c
cSRCS_$(d)   += gpio.c
cSRCS_$(d)   += i2c.c
cSRCS_$(d)   += iwdg.c
cSRCS_$(d)   += pwm_in.c
cSRCS_$(d)   += pwr.c
cSRCS_$(d)   += rtc.c
cSRCS_$(d)   += spi.c
cSRCS_$(d)   += syscalls.c
cSRCS_$(d)   += systick.c
cSRCS_$(d)   += timer.c
cSRCS_$(d)   += usart.c

cSRCS_$(d) += usb/descriptors.c
cSRCS_$(d) += usb/usb_callbacks.c
cSRCS_$(d) += usb/usb_core.c
cSRCS_$(d) += usb/usb_hardware.c
cSRCS_$(d) += usb/usb_init.c
cSRCS_$(d) += usb/usb_int.c
cSRCS_$(d) += usb/usb_mem.c
cSRCS_$(d) += usb/usb_regs.c
cSRCS_$(d) += usb/usb.c



cppSRCS_$(d) := 

sSRCS_$(d)   := 

cFILES_$(d)   := $(cSRCS_$(d):%=$(d)/%)
cppFILES_$(d) := $(cppSRCS_$(d):%=$(d)/%)
sFILES_$(d)   := $(sSRCS_$(d):%=$(d)/%)

OBJS_$(d)	:= $(cFILES_$(d):%.c=$(BUILD_PATH)/%.o)
OBJS_$(d)	+= $(cppFILES_$(d):%.cpp=$(BUILD_PATH)/%.o)
OBJS_$(d)	+= $(sFILES_$(d):%.s=$(BUILD_PATH)/%.o)

DEPS_$(d) 	:= $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))
$(OBJS_$(d)): TGT_ASFLAGS :=

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
