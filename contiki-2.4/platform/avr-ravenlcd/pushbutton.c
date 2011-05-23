#include "pushbutton.h"
#include "uart.h"
#include "main.h"
#include "lcd.h"
#include <util/delay.h>

#define PRESSED 0x01
#define NOT_PRESSED 0x00
uint8_t state = NOT_PRESSED;

void pushbutton_init(void) {
  // set data direction to output
  PUSHBUTTON_DDR &= ~(1 << PUSHBUTTON_PIN);
}

void entprellung( volatile uint8_t *port, uint8_t maske ) {
  uint8_t   port_puffer;
  uint8_t   entprellungs_puffer;
 
  for( entprellungs_puffer=0 ; entprellungs_puffer!=0xff ; ) {
    entprellungs_puffer<<=1;
    port_puffer = *port;
    _delay_us(150);
    if( (*port & maske) == (port_puffer & maske) )
      entprellungs_puffer |= 0x01;
  }
}

void pushbutton_loop(void ) {
  entprellung( &PUSHBUTTON_PORT, (1<<PUSHBUTTON_PIN) ); // ggf. Prellen abwarten 
  if ( PUSHBUTTON_PORT & (1<<PUSHBUTTON_PIN) )           // dann stabilen Wert einlesen
  { // We're not pressed.
    led_off();
    state=NOT_PRESSED;
    // Invalid frame for debugging purposes.
    //uart_serial_send_frame(0xEE , 0, 0);
  } else { // we're pressed.
    if (state==NOT_PRESSED) {
      led_on();
      uart_serial_send_frame(SEND_PUSHBUTTON , 0, 0);
      state=PRESSED;
    }
  }

}


