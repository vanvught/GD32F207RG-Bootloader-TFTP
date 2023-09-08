/*
 * usbh_conf.h
 */

#ifndef USBH_CONF_H_
#define USBH_CONF_H_

#include <stdint.h>

#include "gd32.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define USBH_MAX_EP_NUM                         2
#define USBH_MAX_INTERFACES_NUM                 2
#define USBH_MAX_ALT_SETTING                    2
#define USBH_MAX_SUPPORTED_CLASS                2

#define USBH_DATA_BUF_MAX_LEN                   0x200
#define USBH_CFGSET_MAX_LEN                     0x200

#endif /* USBH_CONF_H_ */
