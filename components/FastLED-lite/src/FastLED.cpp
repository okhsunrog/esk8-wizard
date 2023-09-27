#define FASTLED_INTERNAL
#include "FastLED.h"

/// @file FastLED.cpp
/// Central source file for FastLED, implements the CFastLED class/object

#if defined(__SAM3X8E__)
volatile uint32_t fuckit;
#endif

FASTLED_NAMESPACE_BEGIN


CFastLED FastLED;

CLEDController *CLEDController::m_pHead = NULL;
CLEDController *CLEDController::m_pTail = NULL;

/// Global frame counter, used for debugging ESP implementations
/// @todo Include in FASTLED_DEBUG_COUNT_FRAME_RETRIES block?
uint32_t _frame_cnt=0;

/// Global frame retry counter, used for debugging ESP implementations
/// @todo Include in FASTLED_DEBUG_COUNT_FRAME_RETRIES block?
uint32_t _retry_cnt=0;

// uint32_t CRGB::Squant = ((uint32_t)((__TIME__[4]-'0') * 28))<<16 | ((__TIME__[6]-'0')*50)<<8 | ((__TIME__[7]-'0')*28);

CFastLED::CFastLED() {
	// clear out the array of led controllers
	// m_nControllers = 0;
	m_Scale = 255;
	m_nFPS = 0;
}

CLEDController & CFastLED::operator[](int x) {
	CLEDController *pCur = CLEDController::head();
	while(x-- && pCur) {
		pCur = pCur->next();
	}
	if(pCur == NULL) {
		return *(CLEDController::head());
	} else {
		return *pCur;
	}
}

/// Called at program exit when run in a desktop environment. 
/// Extra C definition that some environments may need. 
/// @returns 0 to indicate success
extern "C" int atexit(void (* /*func*/ )()) { return 0; }

#ifdef FASTLED_NEEDS_YIELD
extern "C" void yield(void) { }
#endif

#ifdef NEED_CXX_BITS
namespace __cxxabiv1
{
	#if !defined(ESP8266) && !defined(ESP32)
	extern "C" void __cxa_pure_virtual (void) {}
	#endif

	/* guard variables */

	/* The ABI requires a 64-bit type.  */
	__extension__ typedef int __guard __attribute__((mode(__DI__)));

	extern "C" int __cxa_guard_acquire (__guard *) __attribute__((weak));
	extern "C" void __cxa_guard_release (__guard *) __attribute__((weak));
	extern "C" void __cxa_guard_abort (__guard *) __attribute__((weak));

	extern "C" int __cxa_guard_acquire (__guard *g)
	{
		return !*(char *)(g);
	}

	extern "C" void __cxa_guard_release (__guard *g)
	{
		*(char *)g = 1;
	}

	extern "C" void __cxa_guard_abort (__guard *)
	{

	}
}
#endif

FASTLED_NAMESPACE_END
