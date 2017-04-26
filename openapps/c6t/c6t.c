/**
\brief CoAP 6top application.

\author Xavi Vilajosana <xvilajosana@eecs.berkeley.edu>, February 2013.
\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, July 2014
*/

#include "opendefs.h"
#include "c6t.h"
#include "sixtop.h"
#include "idmanager.h"
#include "openqueue.h"
#include "neighbors.h"
#include "cbor.h"
#include "byteorder.h"
#include "radio.h"

//=========================== defines =========================================

const uint8_t c6t_path0[] = "6t";

//=========================== variables =======================================

c6t_vars_t c6t_vars;

//=========================== prototypes ======================================

owerror_t c6t_receive(
   OpenQueueEntry_t* msg,
   coap_header_iht*  coap_header,
   coap_option_iht*  coap_options
);
void    c6t_sendDone(
   OpenQueueEntry_t* msg,
   owerror_t         error
);

//=========================== public ==========================================

void c6t_init() {
   if(idmanager_getIsDAGroot()==TRUE) return;

   //Yadhu added starts here
   unsigned char data[1024] = {0};
   cbor_stream_t stream;
   memset(data,0x00,1024);
   cbor_init(&stream, data, sizeof(data));

   cbor_serialize_int(&stream,30);
   //Yadhu added ends here

   openserial_printf("This is c6t app",15);

   // prepare the resource descriptor for the /6t path
   c6t_vars.desc.path0len            = sizeof(c6t_path0)-1;
   c6t_vars.desc.path0val            = (uint8_t*)(&c6t_path0);
   c6t_vars.desc.path1len            = 0;
   c6t_vars.desc.path1val            = NULL;
   c6t_vars.desc.componentID         = COMPONENT_C6T;
   c6t_vars.desc.discoverable        = TRUE;
   c6t_vars.desc.callbackRx          = &c6t_receive;
   c6t_vars.desc.callbackSendDone    = &c6t_sendDone;

   opencoap_register(&c6t_vars.desc);
}

//=========================== private =========================================

/**
\brief Receives a command and a list of items to be used by the command.

\param[in] msg          The received message. CoAP header and options already
   parsed.
\param[in] coap_header  The CoAP header contained in the message.
\param[in] coap_options The CoAP options contained in the message.

\return Whether the response is prepared successfully.
*/
owerror_t c6t_receive(
      OpenQueueEntry_t* msg,
      coap_header_iht*  coap_header,
      coap_option_iht*  coap_options
   ) {

   owerror_t            outcome;
   open_addr_t          neighbor;
   bool                 foundNeighbor;
   openserial_printf("c6t_receive",11);

   switch (coap_header->Code) {

      case COAP_CODE_REQ_PUT:
         // add a slot

         openserial_printf(&(msg->payload[0]),1);

         uint8_t neighbor_count = neighbors_getNumNeighbors();
         char msg_local[40];

         sprintf(msg_local, "neighbor_count %d", neighbor_count);
         openserial_printf(msg_local,16);
         memset(msg_local,0,40);

         neighborRow_t local_neighbor;

         if(FALSE == neighbors_getNeighborInfo(msg->payload[0]-'0',&local_neighbor))
             openserial_printf("Failed to get neighbor",22);
          else {
             sprintf(msg_local,"neighbor address: ");
             memcpy(msg_local+18,local_neighbor.addr_64b.addr_128b,16);
             openserial_printf(msg_local,27);
          }

         // reset packet payload
         msg->payload                  = &(msg->packet[127]);
         msg->length                   = 0;

         // get preferred parent
         foundNeighbor = icmpv6rpl_getPreferredParentEui64(&neighbor);

         if (foundNeighbor==FALSE) {
            outcome                    = E_FAIL;
            coap_header->Code          = COAP_CODE_RESP_PRECONDFAILED;
            break;
         }

         if (sixtop_setHandler(SIX_HANDLER_SF0)==FALSE){
            // one sixtop transcation is happening, only one instance at one time

            // set the CoAP header
            coap_header->Code             = COAP_CODE_RESP_CHANGED;

            outcome                       = E_FAIL;
            break;
         }
         // call sixtop
         sixtop_request(
            IANA_6TOP_CMD_ADD,
            &neighbor,
            1
         );

         // set the CoAP header
         coap_header->Code             = COAP_CODE_RESP_CHANGED;

         outcome                       = E_SUCCESS;
         break;

      case COAP_CODE_REQ_DELETE:
         // delete a slot

         // reset packet payload
         msg->payload                  = &(msg->packet[127]);
         msg->length                   = 0;

         // get preferred parent
         foundNeighbor = icmpv6rpl_getPreferredParentEui64(&neighbor);
         if (foundNeighbor==FALSE) {
            outcome                    = E_FAIL;
            coap_header->Code          = COAP_CODE_RESP_PRECONDFAILED;
            break;
         }

         if (sixtop_setHandler(SIX_HANDLER_SF0)==FALSE){
            // one sixtop transcation is happening, only one instance at one time

            // set the CoAP header
            coap_header->Code             = COAP_CODE_RESP_CHANGED;

            outcome                       = E_FAIL;
            break;
         }
         // call sixtop
         sixtop_request(
            IANA_6TOP_CMD_DELETE,
            &neighbor,
            1
         );

         // set the CoAP header
         coap_header->Code             = COAP_CODE_RESP_CHANGED;

         outcome                       = E_SUCCESS;
         break;

      default:
         outcome = E_FAIL;
         break;
   }

   return outcome;
}

void c6t_sendDone(OpenQueueEntry_t* msg, owerror_t error) {
   openqueue_freePacketBuffer(msg);
}
