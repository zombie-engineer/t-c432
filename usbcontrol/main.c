#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <errno.h>

#define ARRAY_SIZE(_a) (sizeof(_a)/sizeof(_a[0]))

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
  for (libusb_device **d = dlist; d && *d; d++) {
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

#define PACKET_SIZE 64

static int transfer_raw_bytes(libusb_device_handle *h, int ep_out,
  char *data, int size)
{
  char buf[PACKET_SIZE];
  int status;
  int actual;
  int len;

  status = libusb_bulk_transfer(h, ep_out, data, PACKET_SIZE, &actual, 0);
  printf("bulk_transfer ep: 0x%02x, st: %d\n", ep_out, status);

  if (status != LIBUSB_SUCCESS) {
    return -1;
  }

  return 0;
}
static int transfer_string(libusb_device_handle *h, int ep_out,
  const char *str)
{
  char buf[PACKET_SIZE];
  int status;
  int actual;
  int len;

  memset(buf, sizeof(buf), 0);
  actual = 0;
  len = strlen(str);
  strncpy(buf, str, len);
  buf[len] = 0;
  status = libusb_bulk_transfer(h, ep_out, buf, PACKET_SIZE, &actual, 0);
  printf("bulk_transfer ep: 0x%02x, st: %d\n", ep_out, status);

  if (status != LIBUSB_SUCCESS) {
    return -1;
  }

  return 0;
}

static void test_ep_in(libusb_device_handle *h, int ep_in)
{
  int actual;
  int status;

  char buf[PACKET_SIZE];

  while(1) {
    int status;
    memset(buf, 0, sizeof(buf));
    status = libusb_bulk_transfer(h, ep_in, buf, PACKET_SIZE, &actual, 0);
    if (status != LIBUSB_SUCCESS) {
      abort();
    }

    printf("bulk_transfer ep: 0x%02x, st: %d, actual:%d\n", ep_in, status,
	actual);
  }
}

static int transfer_back(libusb_device_handle *h, int ep_in,
  int *word)
{
  char buf[PACKET_SIZE];
  int status;
  int actual;
  int len;

  memset(buf, sizeof(buf), 0);
  actual = 0;
  status = libusb_bulk_transfer(h, ep_in, buf, PACKET_SIZE, &actual, 0);
  printf("bulk_transfer ep: 0x%02x, st: %d\n", ep_in, status);

  if (status != LIBUSB_SUCCESS) {
    return -1;
  }

  *word = *(int *)buf;

  return 0;
}

static void test_ep_out(libusb_device_handle *h, int ep_out)
{
  bool should_break = false;

  const char *strings[] = {
    "test_1",
    "test_2",
    "test_3"
  };

  while(!should_break) {
    for (int i = 0; i < ARRAY_SIZE(strings); ++i)
    {
      if (transfer_string(h, ep_out, strings[i]))
        should_break = true;
    }
  }
}

static libusb_device_handle *device_open(int *ep_in, int *ep_out)
{
  struct libusb_config_descriptor *cfg;
  libusb_device_handle *h;
  libusb_device *d;
  int iface;

  h = open_device_by_vid_pid(0xd001, 0xd002);
  if (!h)
    return NULL;

  d = libusb_get_device(h);
  if (!d) {
    libusb_close(h);
    return NULL;
  }

  if (libusb_get_config_descriptor(d, 0, &cfg) != LIBUSB_SUCCESS) {
    libusb_close(h);
    return NULL;
  }

  *ep_in = cfg->interface->altsetting->endpoint[0].bEndpointAddress;
  *ep_out = cfg->interface->altsetting->endpoint[1].bEndpointAddress;
  iface = 0;
  libusb_free_config_descriptor(cfg);

  if (libusb_claim_interface(h, iface) != LIBUSB_SUCCESS) {
    libusb_close(h);
    return NULL;
  }

  return h;
}

int read_single_word(char *buf, int size)
{
  int i = 0;

  while(i < size) {
    char ch = getchar();

    // printf("%c-", ch);
    if (ch == EOF) {
      perror("read STDIN");
      return -1;
    }

    if (ch == '\n') {
      break;
    }

    buf[i++] = ch;
  }

  return i;
}

int io_loop(libusb_device_handle *h, int ep_in, int ep_out)
{
  char buf[64];
  int len;
  int word;
  buf[0] = 0;
  char data[6];
  int idx = 0;
  while(1) {
    memset(data, 0, sizeof(data));
    data[idx / 2] = 0xf << ((idx % 2) * 4);
    transfer_raw_bytes(h, ep_out, data, sizeof(data));
    idx++;
    if (idx > 11)
      idx = 0;

    usleep(100000);
  }
  while(1) {
    len = read_single_word(buf, sizeof(buf) - 1);
    if (len == -1) {
      return -1;
    }
    buf[len] = 0;
    // printf("New word: %s\n", buf);
    if (transfer_string(h, ep_out, buf)) {
      break;
    }
    if (transfer_back(h, ep_in, &word)) {
      break;
    }
    printf("New word: %s:%d\n", buf, word);
  }
}

int main()
{
  libusb_device_handle *h;
  int ret = -1;
  int ep_in;
  int ep_out;
  struct libusb_transfer t;

  libusb_init(&ctx);
  h = device_open(&ep_in, &ep_out);
  if (!h) {
    printf("Failed to open device. Ensure it is connected\n");
    return -1;
  }

  ret =  io_loop(h, ep_in, ep_out);
  if (ret) {
    return ret;
  }

  test_ep_out(h, ep_out);
  test_ep_in(h, ep_in);

#if 0
  libusb_fill_bulk_transfer(&t, h, ep_out, buf, sizeof(buf), bulk_completed, NULL,
    2000);

  if (libusb_submit_transfer(&t) != LIBUSB_SUCCESS) {
    goto out_close;
  }
#endif

out_close:
  libusb_close(h);
out:
  libusb_exit(ctx);
  return ret;
}
