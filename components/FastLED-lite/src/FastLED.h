#ifndef __INC_FASTSPI_LED2_H
#define __INC_FASTSPI_LED2_H


#include <stdint.h>

#include "fastled_config.h"
#include "led_sysdefs.h"

// Utility functions
#include "controller.h"

#include "fastled_progmem.h"

#include "lib8tion.h"
#include "pixeltypes.h"
#include "hsv2rgb.h"
#include "colorutils.h"
#include "pixelset.h"
#include "colorpalettes.h"

#include "noise.h"

FASTLED_NAMESPACE_BEGIN

/// High level controller interface for FastLED.
/// This class manages controllers, global settings, and trackings such as brightness
/// and refresh rates, and provides access functions for driving led data to controllers
/// via the show() / showColor() / clear() methods.
/// This is instantiated as a global object with the name FastLED.
/// @nosubgrouping
class CFastLED {
	// int m_nControllers;
	uint8_t  m_Scale;         ///< the current global brightness scale setting
	uint16_t m_nFPS;          ///< tracking for current frames per second (FPS) value
	uint32_t m_nMinMicros;    ///< minimum µs between frames, used for capping frame rates

public:
	CFastLED();

	/// Get a reference to a registered controller
	/// @returns a reference to the Nth controller
	CLEDController & operator[](int x);

	/// Get the number of leds in the first controller
	/// @returns the number of LEDs in the first controller
	int size() { return (*this)[0].size(); }

	/// Get a pointer to led data for the first controller
	/// @returns pointer to the CRGB buffer for the first controller
	CRGB *leds() { return (*this)[0].leds(); }
};


/// Global LED strip management instance
extern CFastLED FastLED;

FASTLED_NAMESPACE_END

#endif
