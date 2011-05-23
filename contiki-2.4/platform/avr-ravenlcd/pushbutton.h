#ifndef RAVENLCD_PUSHBUTTON_H
#define RAVENLCD_PUSHBUTTON_H 1

#include <avr/io.h>

#define PUSHBUTTON_PORT PORTE
#define PUSHBUTTON_DDR DDRE
#define PUSHBUTTON_PIN PE3

void pushbutton_init(void);
void pushbutton_loop(void );

#endif /* RELAY_H */

