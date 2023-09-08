/*
 * usb_vbus.cpp
 */

#include <cstdio>
#include <cstdint>

#include "gd32.h"

extern "C" {
void usb_vbus_drive (uint8_t state) {
	printf("usb_vbus_drive: %u\n", state);
}
}



