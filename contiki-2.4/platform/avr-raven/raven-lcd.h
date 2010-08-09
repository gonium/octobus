#ifndef raven_lcd_h
#define raven_lcd_h

int raven_lcd_serial_input(unsigned char ch);
void send_relay_on(void);
void send_relay_off(void);
void send_relay_toggle(void);
//static void raven_ping6(void)
PROCESS_NAME(raven_lcd_process);

/* Events for the Raven LCD processing loop */
#define PING_REQUEST 0xc1
#define PING_REPLY   0xc0
#define SERIAL_CMD   0x1

// MD:  Commands for the relay.
#define RELAY_ON					  (0xD0)
#define RELAY_OFF					  (0xD1)
#define RELAY_TOGGLE			  (0xD2)


#endif
