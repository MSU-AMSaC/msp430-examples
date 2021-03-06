#ifndef __PROCESSING_H_GUARD
#define __PROCESSING_H_GUARD

#include <stdint.h>
#include "global.h"
#include "drivers/adc.h"

#define INIT_PROCESSOR(name)                   \
  {                                            \
    .execute = &execute_##name,                \
    .state   = { .name = name##_initializer }, \
  }

union processor_state
{
  #define EXPAND_PROCESSOR(name, fields, ...) \
    struct name##_state fields name;
  #include "processors.xmac.h"
  #undef EXPAND_PROCESSOR
};

struct processor
{
  uint8_t (*execute)
    (uint16_t sample, unsigned int ch, struct processor *state);
  union processor_state state;
};

#define EXPAND_PROCESSOR(name, ...) \
  uint8_t execute_##name            \
    (uint16_t sample, unsigned int ch, struct processor *state);
#include "processors.xmac.h"
#undef EXPAND_PROCESSOR

#endif /* __PROCESSING_H_GUARD */
