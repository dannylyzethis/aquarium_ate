#ifndef WATCHDOG_COMPAT_H
#define WATCHDOG_COMPAT_H

#include <Arduino.h>

#if defined(ARDUINO_ARCH_AVR)
  #include <avr/wdt.h>
  #define APP_WDT_ENABLE_8S() wdt_enable(WDTO_8S)
  #define APP_WDT_RESET() wdt_reset()
#else
  // Placeholder for non-AVR targets (e.g., GIGA mbed core).
  // Keep no-op to preserve control flow without AVR-specific headers.
  #define APP_WDT_ENABLE_8S() do {} while (0)
  #define APP_WDT_RESET() do {} while (0)
#endif

#endif
