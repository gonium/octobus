#ifndef ___RAVEN_RELAY_H
#define ___RAVEN_RELAY_H 1

#include <stdint.h>

PROCESS_NAME(raven_relay_process);

/* Command definitions. */
#define ON		  0x10
#define OFF		  0x11
#define STATUS	0x12
#define TOGGLE  0x13

struct switchmsg_t {
  uint16_t command;
};


#endif /* ___RAVEN-RELAY_H */
