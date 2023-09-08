/*
 * usb.cpp
 */

#include "gd32.h"

extern "C" {
void usb_udelay(uint32_t nMicros) {
	udelay(nMicros);
}
void usb_mdelay(uint32_t nMillis) {
	udelay(nMillis * 1000U);
}
}
