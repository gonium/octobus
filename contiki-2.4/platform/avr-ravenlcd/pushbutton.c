#include "pushbutton.h"
#include "uart.h"
#include "main.h"

void pushbutton_init(void) {
  // set data direction to output
  PUSHBUTTON_DDR &= ~(1 << PUSHBUTTON_PIN);

}



void pushbutton_loop(void ) {
  //if (pushbutton_pressed()) {
    uart_serial_send_frame(SEND_PUSHBUTTON , 0, 0);
  //}
}


