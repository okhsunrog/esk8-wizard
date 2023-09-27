#ifndef __INC_FL_PROGMEM_H
#define __INC_FL_PROGMEM_H

#include "FastLED.h"

FASTLED_NAMESPACE_BEGIN


#define FL_PROGMEM
#define FL_PGM_READ_BYTE_NEAR(x)  (*((const  uint8_t*)(x)))
#define FL_PGM_READ_WORD_NEAR(x)  (*((const uint16_t*)(x)))
#define FL_PGM_READ_DWORD_NEAR(x) (*((const uint32_t*)(x)))


#if defined(FASTLED_ARM) || defined(ESP32) || defined(ESP8266)
#define FL_ALIGN_PROGMEM  __attribute__ ((aligned (4)))
#else
#define FL_ALIGN_PROGMEM
#endif


FASTLED_NAMESPACE_END

#endif
