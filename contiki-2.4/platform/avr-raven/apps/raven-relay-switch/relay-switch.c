/*   Copyright (c) 2010 Mathias Dalheimer
 *  All rights reserved.
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of the copyright holders nor the names of
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include "raven-relay.h"
#include <string.h>
#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "webserver-nogui.h"
#include "httpd-cgi.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

#define UDP_DATA_LEN 120
#define UDP_IP_BUF   ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

static struct uip_udp_conn *udpconn;
static struct etimer udp_periodic_timer;
static uint8_t RELAY_STATE = ON;


PROCESS(raven_relay_process, "Raven Relay Process");
AUTOSTART_PROCESSES(&raven_relay_process);

/*---------------------------------------------------------------------------*/

static void
pollhandler(void) {
  printf("----Relay: Process polled\r\n");
}

static void
exithandler(void) {
  printf("----Relay: Process exits.\r\n");
}

/*---------------------------------------------------------------------------*/

static void
udphandler(process_event_t ev, process_data_t data)
{
  static int seq_id;
  char buf[UDP_DATA_LEN];
  struct switchmsg_t relaycmd;

  PRINTF("----Relay: Entering UDP handler.\r\n");
  if (ev == tcpip_event) {
    if(uip_newdata()) {
      // Extract data
      //((char *)uip_appdata)[uip_datalen()] = 0;
      //PRINTF("Server received: '%s' from ", (char *)uip_appdata);
      PRINTF("----- Relay received: '%d' bytes from ", uip_datalen());
      PRINT6ADDR(&UDP_IP_BUF->srcipaddr);
      PRINTF("\r\n");
      if (uip_datalen() == sizeof(relaycmd)) {
        memcpy(&relaycmd, uip_appdata, sizeof(relaycmd));
        relaycmd.command = ntohs(relaycmd.command);
        switch(relaycmd.command) {
          case ON:
            PRINTF("----Relay: Switching ON.\r\n");
            send_relay_on();
            RELAY_STATE = ON;
            break;
          case OFF:
            PRINTF("----Relay: Switching OFF.\r\n");
            send_relay_off();
            RELAY_STATE = OFF;
            break;
          case TOGGLE:
            if (RELAY_STATE == ON) {
              PRINTF("----Relay: Toggling to OFF.\r\n");
              send_relay_off();
              RELAY_STATE = OFF;
            } else {
              PRINTF("----Relay: Toggling to ON.\r\n");
              send_relay_on();
              RELAY_STATE = ON;
            }
            break;
          case STATUS:
            PRINTF("----Relay: Sending status.\r\n");
            uip_ipaddr_copy(&udpconn->ripaddr, &UDP_IP_BUF->srcipaddr);
            udpconn->rport = UDP_IP_BUF->srcport;
            PRINTF("Responding with message: ");
            sprintf(buf, "Hello from the server! (%d)\r\n", ++seq_id);
            PRINTF("%s\n", buf);
            uip_udp_packet_send(udpconn, buf, strlen(buf));
            break;
          default:
            PRINTF("Unknown command received, ignoring.\r\n");
            break;
        }
      } else {
        PRINTF("Received %d bytes, but switchcommand is %d bytes big. Discarding.\r\n",
            uip_datalen(), sizeof(relaycmd));
      }
      /* Restore server connection to allow data from any node */
      memset(&udpconn->ripaddr, 0, sizeof(udpconn->ripaddr));
      udpconn->rport = 0;
    }
  } else {


    printf("----Relay: sending hearbeat to: ");
    PRINT6ADDR(&udpconn->ripaddr);
    printf("\r\n");
    uip_udp_packet_send(udpconn, "heartbeart!", strlen("heartbeart!"));

    etimer_set(&udp_periodic_timer, 60*CLOCK_SECOND);
  }
}

static void print_local_addresses(void) {
  int i;
  uip_netif_state state;

  PRINTF("Current IPv6 addresses: \r\n");
  for(i = 0; i < UIP_CONF_NETIF_MAX_ADDRESSES; i++) {
    state = uip_netif_physical_if.addresses[i].state;
    if(state  != NOT_USED) { //== TENTATIVE || state == PREFERRED) {
      PRINT6ADDR(&uip_netif_physical_if.addresses[i].ipaddr);
      PRINTF("\n\r");
    }
    }
  }

  /*---------------------------------------------------------------------------*/

  PROCESS_THREAD(raven_relay_process, ev, data) {
    uip_ipaddr_t ipaddr;
    PROCESS_POLLHANDLER(pollhandler());
    PROCESS_EXITHANDLER(exithandler());

    // see: http://senstools.gforge.inria.fr/doku.php?id=contiki:examples
    PROCESS_BEGIN();
    PRINTF("Relay process startup.\r\n");
    // wait 3 second, in order to have the IP addresses well configured
    etimer_set(&udp_periodic_timer, CLOCK_CONF_SECOND*3);
    // wait until the timer has expired
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    // Define Address of the server that receives our heartbeats.
    // TODO: Make this dynamic
#ifdef UDP_ADDR_A
    uip_ip6addr(&ipaddr,
        UDP_ADDR_A,UDP_ADDR_B,UDP_ADDR_C,UDP_ADDR_D,
        UDP_ADDR_E,UDP_ADDR_F,UDP_ADDR_G,UDP_ADDR_H);
#else /* UDP_ADDR_A */
    uip_ip6addr(&ipaddr,0xbbbb,0,0,0,0xd69a,0x20ff,0xfe07,0x7664);
#endif /* UDP_ADDR_A */

    udpconn = udp_new(NULL, HTONS(0), NULL);

    //udpconn = udp_new(&ipaddr, HTONS(0xF0B0+1), NULL);
    udp_bind(udpconn, HTONS(0xF0B0));
    // udp_attach(udpconn, NULL);

    PRINTF("Created connection with remote peer ");
    PRINT6ADDR(&udpconn->ripaddr);
    PRINTF("\r\nlocal/remote port %u/%u\r\n", HTONS(udpconn->lport),HTONS(udpconn->rport));

    print_local_addresses();
    etimer_set(&udp_periodic_timer, 60*CLOCK_SECOND);

    while(1){
      PRINTF("--- Relay: Waiting for events.\r\n");
      //   tcpip_poll_udp(udpconn);
      PROCESS_WAIT_EVENT();
      //    PROCESS_YIELD();
      udphandler(ev, data);
    }


    PROCESS_END();
  }
