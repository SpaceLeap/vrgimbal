# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
BUILDDIRS       += $(BUILD_PATH)/$(d)/Libraries/STM32F10x_StdPeriph_Driver/src

LIBRARY_INCLUDES += -I$(STM32_PATH)/Libraries/STM32F10x_StdPeriph_Driver/inc
LIBRARY_INCLUDES += -I$(STM32_PATH)/Libraries/CMSIS/CM3/CoreSupport
LIBRARY_INCLUDES += -I$(STM32_PATH)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x

# Local flags
CFLAGS_$(d) =

# Local rules and targets
cSRCS_$(d) := 
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/misc.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_can.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_cec.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_crc.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dac.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dbgmcu.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_fsmc.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_iwdg.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rtc.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_sdio.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c
cSRCS_$(d) += Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_wwdg.c

sSRCS_$(d) := 

cFILES_$(d) := $(cSRCS_$(d):%=$(d)/%)
sFILES_$(d) := $(sSRCS_$(d):%=$(d)/%)

OBJS_$(d)	:= $(cFILES_$(d):%.c=$(BUILD_PATH)/%.o)
OBJS_$(d)	+= $(sFILES_$(d):%.s=$(BUILD_PATH)/%.o)

DEPS_$(d) := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))
$(OBJS_$(d)): TGT_ASFLAGS :=

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
