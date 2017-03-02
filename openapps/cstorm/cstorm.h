#ifndef __CSTORM_H
#define __CSTORM_H

/**
\addtogroup AppUdp
\{
\addtogroup cstorm
\{
*/

#include "opencoap.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       timerId;
   opentimer_id_t       timerId_newTraffic;
   uint16_t             period;   ///< inter-packet period (in ms)
   uint8_t              packet_per_slotframe;
   bool                 busySending;
   uint8_t              packetId[2];
} cstorm_vars_t;

//=========================== prototypes ======================================

void cstorm_init(void);
uint16_t cstorm_getPeriod();
void cstorm_generateNewTraffic(opentimer_id_t id);
void cstorm_stop();

/**
\}
\}
*/

#endif
