#pragma once
#include <stdint.h>
#define USB_REQUEST_DEVICE_TO_HOST_STANDARD 0x80
#define USB_REQUEST_HOST_TO_DEVICE_STANDARD 0x00
#define USB_SETUP_REQUEST_SET_ADDRESS 5
#define USB_SETUP_REQUEST_GET_DESCRIPTOR 6
#define USB_SETUP_REQUEST_SET_DESCRIPTOR 7
#define USB_SETUP_REQUEST_GET_CONFIGURATION 8
#define USB_SETUP_REQUEST_SET_CONFIGURATION 9

#define USB_DESCRIPTOR_TYPE_DEVICE 1
#define USB_DESCRIPTOR_TYPE_CONFIGURATION 2
#define USB_DESCRIPTOR_TYPE_STRING 3
#define USB_DESCRIPTOR_TYPE_INTERFACE 4
#define USB_DESCRIPTOR_TYPE_ENDPOINT 5
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER 6
#define USB_DESCRIPTOR_TYPE_SPEED_CONFIG 7
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER 8
#define USB_DESCRIPTOR_TYPE_OTG 9

#define USB_EP_ATTRIBUTE_TYPE_CONTROL   0
#define USB_EP_ATTRIBUTE_TYPE_ISO       1
#define USB_EP_ATTRIBUTE_TYPE_BULK      2
#define USB_EP_ATTRIBUTE_TYPE_INTERRUPT 3
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
} __attribute__((packed));

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
} __attribute__((packed));

struct usb_descriptor_device_qualifier {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint8_t bNumConfigurations;
  uint8_t bReserved;
} __attribute__((packed));

struct usb_descriptor_config {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wTotalLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t bMaxPower;
} __attribute__((packed));

struct usb_descriptor_interface {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternativeSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
} __attribute__((packed));

struct usb_descriptor_endpoint {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
} __attribute__((packed));

struct usb_descriptor_string_0 {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wLangId;
} __attribute__((packed));

struct usb_descriptor_string_header {
  uint8_t bLength;
  uint8_t bDescriptorType;
} __attribute__((packed));
