#include "relay.h"

void relay_init(void) {
  // set data direction to output
  RELAY_DDR |= (1 << RELAY_PIN);

}

void relay_toggle(void) {
  static char state=0;
  if (state == 0) {
    relay_on();
    state=1;
  } else {
    relay_off();
    state=0;
  }
}

void relay_on(void) {
  // activate relay
  RELAY_PORT |= (1 << RELAY_PIN);
}

void relay_off(void) {
  // deactivate relay
  RELAY_PORT &= ~(1 << RELAY_PIN);
}
