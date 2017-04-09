/**
\brief A timer module with only a single compare value. 

\author Xavier Vilajosana <xvilajosana@eecs.berkeley.edu>
\author Christian Hopfner <christian.hopfner@pcm.endress.com>, September 2016
\author Tengfei Chang     <tengfei.chang@inria.fr> April 2017
*/

#include "board_info.h"
#include "sctimer.h"
#include "sleepmode.h"
#include "debugpins.h"
#include <headers/hw_ints.h>

// ========================== variable ========================================

typedef struct {
    sctimer_cbt sctimer_cb;
} sctimer_vars_t;

sctimer_vars_t sctimer_vars;


// ========================== private =========================================

static void sctimer_isr(void); 

// ========================== protocol =========================================

/**
\brief Initialization sctimer.
*/
void sctimer_init(void){
    memset(&sctimer_vars, 0, sizeof(sctimer_vars_t));
    IntRegister(INT_SMTIM, sctimer_isr);
    IntDisable(INT_SMTIM);
}

void sctimer_set_callback(sctimer_cbt cb){
    sctimer_vars.sctimer_cb = cb;
}

/**
\brief set compare interrupt
*/
void sctimer_setCompare(uint32_t val){
    IntEnable(INT_SMTIM);
    SleepModeTimerCompareSet(val);
}

/**
\brief Return the current value of the timer's counter.

 \returns The current value of the timer's counter.
*/
uint32_t sctimer_readCounter(void){
  return SleepModeTimerCountGet();
}

void sctimer_enable(void){
    IntEnable(INT_SMTIM);
}

void sctimer_disable(void){
    IntDisable(INT_SMTIM);
}

// ========================== private =========================================

void sctimer_isr(void) {
    debugpins_isr_set();
    IntPendClear(INT_SMTIM);
    sctimer_vars.sctimer_cb();
    debugpins_isr_clr();
}