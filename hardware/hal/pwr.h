#ifndef _PWR_H_
#define _PWR_H_

#include "stm32.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C"{
#endif

void enterSTOPMode(void);
void enterSLEEPMode(void);
void enterSTANDBYMode(void);
void PWR_init();
bool WakeUpFromStanby(void);

void PWR_reset();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
