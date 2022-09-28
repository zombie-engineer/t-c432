#pragma once
#include <stdint.h>
#define USB_REQUEST_DEVICE_TO_HOST_STANDARD 0x80
#define USB_SETUP_REQUEST_GET_DESCRIPTOR 6

#define USB_DESCRIPTOR_TYPE_DEVICE 1
#define USB_DESCRIPTOR_TYPE_CONFIGURATION 2
#define USB_DESCRIPTOR_TYPE_STRING 3
#define USB_DESCRIPTOR_TYPE_INTERFACE 4
#define USB_DESCRIPTOR_TYPE_ENDPOINT 5
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER 6
#define USB_DESCRIPTOR_TYPE_SPEED_CONFIG 7
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER 8
#define USB_DESCRIPTOR_TYPE_OTG 9

/*
 * USB Setup request structure. Codestyle preserved according
 * to commonly used usb presentation.
 */
struct usb_request {
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
};

struct usb_descriptor_device {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t iManufacturer;
  uint8_t iProduct;
  uint8_t iSerialNumber;
  uint8_t bNumConfigurations;
};
