# Standard things
sp := $(sp).x
dirstack_$(sp) := $(d)
d := $(dir)
BUILDDIRS += $(BUILD_PATH)/$(d)

# Local flags
CFLAGS_$(d) := 

# Local rules and targets
cSRCS_$(d) := 

cppSRCS_$(d) :=
cppSRCS_$(d) += BruGi.cpp
cppSRCS_$(d) += BLcontroller.cpp
cppSRCS_$(d) += variables.cpp
cppSRCS_$(d) += fastMathRoutines.cpp
cppSRCS_$(d) += IMU.cpp
cppSRCS_$(d) += IMU_AP.cpp
cppSRCS_$(d) += IMU_EVV.cpp
cppSRCS_$(d) += MPU6050.cpp
cppSRCS_$(d) += CompassHMC5843.cpp
cppSRCS_$(d) += orientationRoutines.cpp
cppSRCS_$(d) += RCdecode.cpp
cppSRCS_$(d) += SerialCom.cpp
cppSRCS_$(d) += SerialCommand.cpp
cppSRCS_$(d) += ManualCmd.cpp
cppSRCS_$(d) += calibrationRoutines.cpp
cppSRCS_$(d) += realtimeStatistics.cpp
cppSRCS_$(d) += calibrationAccel.cpp
cppSRCS_$(d) += utilities.cpp

cppSRCS_$(d) += Parameters.cpp
#cppSRCS_$(d) += setup.cpp
#cppSRCS_$(d) += system.cpp
#cppSRCS_$(d) += test.cpp
#cppSRCS_$(d) += timers.cpp
#cppSRCS_$(d) += utils.cpp
#cppSRCS_$(d) += motors.cpp

cFILES_$(d)   := $(cSRCS_$(d):%=$(d)/%)
cppFILES_$(d) := $(cppSRCS_$(d):%=$(d)/%)

OBJS_$(d) := $(cFILES_$(d):%.c=$(BUILD_PATH)/%.o) \
             $(cppFILES_$(d):%.cpp=$(BUILD_PATH)/%.o)
DEPS_$(d) := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))

TGT_BIN += $(OBJS_$(d))

# Standard things
-include $(DEPS_$(d))
d := $(dirstack_$(sp))
sp := $(basename $(sp))