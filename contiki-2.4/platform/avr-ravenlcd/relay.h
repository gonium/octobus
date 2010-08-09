#ifndef RELAY_H
#define RELAY_H 1

#include <avr/io.h>

#define RELAY_PORT PORTB
#define RELAY_DDR DDRB
#define RELAY_PIN PB6

void relay_init(void);
void relay_on(void);
void relay_off(void);
void relay_toggle(void);

#endif /* RELAY_H */

