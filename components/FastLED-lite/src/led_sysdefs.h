#ifndef __INC_LED_SYSDEFS_H
#define __INC_LED_SYSDEFS_H

#include "FastLED.h"

#include "fastled_config.h"
#include <stdint.h>

#define FASTLED_ESP32

#define FASTLED_RISCV

// Use system millis timer
// #define FASTLED_HAS_MILLIS

typedef volatile uint32_t RoReg;
typedef volatile uint32_t RwReg;
typedef unsigned long prog_uint32_t;

#define FASTLED_USE_PROGMEM 0

#define FASTLED_ALLOW_INTERRUPTS 1
#define INTERRUPT_THRESHOLD 0

#define NEED_CXX_BITS

#define FASTLED_ESP32_RAW_PIN_ORDER

#ifndef FASTLED_NAMESPACE_BEGIN
/// Start of the FastLED namespace
#define FASTLED_NAMESPACE_BEGIN
/// End of the FastLED namespace
#define FASTLED_NAMESPACE_END
/// "Using" directive for the namespace
#define FASTLED_USING_NAMESPACE
#endif

// Arduino.h needed for convenience functions
// digitalPinToPort/BitMask/portOutputRegister and the pinMode methods.
#ifdef ARDUINO
#include <Arduino.h>
#endif

/// Clock cycles per microsecond.
/// Calculated using the F_CPU preprocessor define
#define CLKS_PER_US (F_CPU / 1000000)

#endif
