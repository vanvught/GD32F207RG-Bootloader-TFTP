/*
 * usb_conf.h
 */

#ifndef USB_CONF_H_
#define USB_CONF_H_

#include <stdint.h>

#include "gd32.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

#if !defined (USE_USB_FS)
# define USE_USB_FS
#endif
#define USB_FS_CORE

/*******************************************************************************
 *                 FIFO Size Configuration in Host mode
 *
 *  (i) Receive data FIFO size = (Largest Packet Size / 4) + 1 or
 *                               2x (Largest Packet Size / 4) + 1,  If a
 *                               high-bandwidth channel or multiple isochronous
 *                               channels are enabled.
 *
 *  (ii) For the host non periodic Transmit FIFO is the largest maximum packet size
 *       for all supported non periodic OUT channels. Typically, a space
 *       corresponding to two Largest Packet Size is recommended.
 *
 *  (iii) The minimum amount of RAM required for Host periodic Transmit FIFO is
 *        the largest maximum packet size for all supported periodic OUT channels.
 *        If there is at least one High Bandwidth Isochronous OUT endpoint,
 *        then the space must be at least two times the maximum packet size for
 *        that channel.
*******************************************************************************/

#define USB_RX_FIFO_FS_SIZE                            128
#define USB_HTX_NPFIFO_FS_SIZE                         96
#define USB_HTX_PFIFO_FS_SIZE                          96

#define USB_SOF_OUTPUT		0
#define USB_LOW_POWER		0

#define USE_HOST_MODE

#ifndef USB_FS_CORE
    #ifndef USB_HS_CORE
        #error "USB_HS_CORE or USB_FS_CORE should be defined"
    #endif
#endif

#ifndef USE_DEVICE_MODE
    #ifndef USE_HOST_MODE
        #error "USE_DEVICE_MODE or USE_HOST_MODE should be defined"
    #endif
#endif

#ifndef USE_USB_HS
    #ifndef USE_USB_FS
        #error "USE_USB_HS or USE_USB_FS should be defined"
    #endif
#endif

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined   ( __GNUC__ )   /* GNU Compiler */
    #ifndef __packed
        #define __packed __attribute__ ((__packed__))
    #endif
#elif defined   (__TASKING__)  /* TASKING Compiler */
    #define __packed __unaligned
#endif /* __GNUC__ */

#endif /* USB_CONF_H_ */
