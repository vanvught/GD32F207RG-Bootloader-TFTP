/*
 * usb_host_msc.cpp
 */


#include <cstdio>
#include <cstdint>

#include "gd32.h"

extern "C" {
#include "usb_host_msc.h"
#include "usbh_core.h"
}

usbh_user_cb usr_cb = {
    usbh_user_init,
    usbh_user_deinit,
    usbh_user_device_connected,
    usbh_user_device_reset,
    usbh_user_device_disconnected,
    usbh_user_over_current_detected,
    usbh_user_device_speed_detected,
    usbh_user_device_desc_available,
    usbh_user_device_address_assigned,
    usbh_user_configuration_descavailable,
    usbh_user_manufacturer_string,
    usbh_user_product_string,
    usbh_user_serialnum_string,
    usbh_user_enumeration_finish,
    usbh_user_userinput,
    usbh_usr_msc_application,
    usbh_user_device_not_supported,
    usbh_user_unrecovered_error
};

#define USBH_USR_FS_INIT             0
#define USBH_USR_FS_READLIST         1
#define USBH_USR_FS_WRITEFILE        2
#define USBH_USR_FS_DRAW             3
#define USBH_USR_FS_DEMOEND          4

static uint32_t usbh_usr_application_state = USBH_USR_FS_INIT;

void usbh_user_init() {
	static uint32_t startup = 0U;

	if (0U == startup) {
		startup = 1U;
		puts("USB host library started.");
	}
}

void usbh_user_deinit() {
    usbh_usr_application_state = USBH_USR_FS_INIT;
}

void usbh_user_device_connected() {
	puts("> Device Attached.");
}

void usbh_user_unrecovered_error() {
	puts("> Unrecovered error state.");
}

void usbh_user_device_disconnected() {
	puts("> Device Disconnected.");
}

void usbh_user_device_reset() {
	puts("> Reset the USB device.");
}

void usbh_user_device_speed_detected(uint32_t device_speed) {
	if (PORT_SPEED_HIGH == device_speed) {
		puts("> High speed device detected.");
	} else if (PORT_SPEED_FULL == device_speed) {
		puts("> Full speed device detected.");
	} else if (PORT_SPEED_LOW == device_speed) {
		puts("> Low speed device detected.");
	} else {
		puts("> Device Fault.");
	}
}

void usbh_user_device_desc_available(void *device_desc) {
	auto *pDevStr = reinterpret_cast<usb_desc_dev *>(device_desc);

	printf("VID: %04Xh\n", static_cast<uint32_t>(pDevStr->idVendor));
	printf("PID: %04Xh\n", static_cast<uint32_t>(pDevStr->idProduct));
}

void usbh_user_device_address_assigned() {
	puts("usbh_user_device_address_assigned");
}

void usbh_user_configuration_descavailable(usb_desc_config *cfg_desc, usb_desc_itf *itf_desc, usb_desc_ep *ep_desc) {
	auto *id = reinterpret_cast<usb_desc_itf *>(itf_desc);

	if (0x08U == (*id).bInterfaceClass) {
		puts("> Mass storage device connected.");
	} else if (0x03U == (*id).bInterfaceClass) {
		puts("> HID device connected.");
	}
}

void usbh_user_manufacturer_string(void *manufacturer_string) {
    printf("Manufacturer: %s\n", reinterpret_cast<char *>(manufacturer_string));
}

void usbh_user_product_string(void *product_string) {
    printf("Product: %s\n", reinterpret_cast<char *>(product_string));
}

void usbh_user_serialnum_string(void *serial_num_string) {
    printf("Serial Number: %s\n", reinterpret_cast<char *>(serial_num_string));
}

void usbh_user_enumeration_finish() {
    puts("> Enumeration completed.");
}

void usbh_user_device_not_supported() {
    puts("> Device not supported.");
}

usbh_user_status usbh_user_userinput() {
	puts("usbh_user_userinput");

    usbh_user_status usbh_usr_status = USR_IN_NO_RESP;

//    /*key User is in polling mode to detect user action */
//    if (RESET == gd_eval_key_state_get(KEY_USER)) {
//        usbh_usr_status = USR_IN_RESP_OK;
//    }

    return usbh_usr_status;
}

void usbh_user_over_current_detected() {
    puts("> Overcurrent detected.");
}

int usbh_usr_msc_application() {
	puts("usbh_usr_msc_application");

	return 0;
}
