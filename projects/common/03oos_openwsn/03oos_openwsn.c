/**
\brief This project runs the full OpenWSN stack.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, August 2010
*/

#include "board.h"
#include "crypto_engine.h"
#include "scheduler.h"
#include "openstack.h"
#include "opendefs.h"

int mote_main(void) {
   
   // initialize
   board_init();
   CRYPTO_ENGINE.init();
   scheduler_init();
   openstack_init();

   openserial_printf("Yadhunandana came from temple",29);
   
   // indicate
   
   // start
   scheduler_start();
   return 0; // this line should never be reached
}



void sniffer_setListeningChannel(uint8_t channel){return;}
