#include <stdio.h>
#include <libusb-1.0/libusb.h>


libusb_context *ctx = NULL;

libusb_device_handle *open_device_by_vid_pid(uint16_t vid, uint16_t pid)
{
  libusb_device **dlist;
  libusb_device_handle *dev_handle;
  libusb_device *found_d = NULL;
  struct libusb_device_descriptor desc;

  if (!ctx)
    return NULL;

  libusb_init(&ctx);
  libusb_get_device_list(ctx, &dlist);
  for (libusb_device **d = dlist; d; d++) {
    if (libusb_get_device_descriptor(*d, &desc) != LIBUSB_SUCCESS) {
      return NULL;
    }
    if (desc.idVendor == vid && desc.idProduct == pid) {
      found_d = *d;
      break;
    }
  }
  if (found_d) {
    if (libusb_open(found_d, &dev_handle) != LIBUSB_SUCCESS) {
      dev_handle = NULL;
    }

  }
  libusb_free_device_list(dlist, 1);
  return dev_handle;
}

void bulk_completed(struct libusb_transfer *t)
{
}

int main()
{
  libusb_device_handle *h;
  libusb_device *d;
  struct libusb_config_descriptor *cfg;
  int ret = -1;
  int ep_in;
  int ep_out;
  int iface;
  struct libusb_transfer t;
  char buf[256];

  libusb_init(&ctx);
  h = open_device_by_vid_pid(0xd001, 0xd002);
  if (!h) {
    goto out;
  }

  d = libusb_get_device(h);
  if (!d) {
    goto out_close;
  }

  if (libusb_get_config_descriptor(d, 0, &cfg) != LIBUSB_SUCCESS)
  {
    goto out_close;
  }

  ep_in = cfg->interface->altsetting->endpoint[0].bEndpointAddress;
  ep_out = cfg->interface->altsetting->endpoint[1].bEndpointAddress;
  iface = 0;
  libusb_free_config_descriptor(cfg);

  if (libusb_claim_interface(h, iface) != LIBUSB_SUCCESS) {
    goto out_close;
  }

  int actual;
  int st = libusb_bulk_transfer(h, ep_in, buf, 64, &actual, 0);

  printf("bulk_transfer ep: 0x%02x, st: %d\n", ep_in, st);

  memset(buf, 64, 0x30);
  st = libusb_bulk_transfer(h, ep_out, buf, 64, &actual, 0);

  printf("bulk_transfer ep: 0x%02x, st: %d\n", ep_out, st);

  libusb_fill_bulk_transfer(&t, h, ep_out, buf, sizeof(buf), bulk_completed, NULL,
    2000);

  if (libusb_submit_transfer(&t) != LIBUSB_SUCCESS) {
    goto out_close;
  }

out_close:
  libusb_close(h);
out:
  libusb_exit(ctx);
  return ret;
}
