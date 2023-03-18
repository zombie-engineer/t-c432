#include <gtk/gtk.h>
#include <libusb-1.0/libusb.h>

#define PACKET_SIZE 64

libusb_context *ctx = NULL;
libusb_device_handle *h = NULL;
int ep_in;
int ep_out;

static libusb_device_handle *open_device_by_vid_pid(uint16_t vid, uint16_t pid)
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

void button_clicked(GtkWidget *w, gpointer data)
{
  char buf[6];
  int idx = (int)data;
  memset(buf, 0, sizeof(buf));
  buf[idx / 8] = 1 << (idx % 8);
  transfer_raw_bytes(h, ep_out, buf, sizeof(buf));

  g_print("clicked %d\n", (int)data);
}

int main(int argc, char *argv[]) {

  GtkWidget *window;
  GtkWidget *b;
  GtkWidget *t;

  int ret = -1;

  libusb_init(&ctx);
  h = device_open(&ep_in, &ep_out);

  if (!h) {
    printf("Failed to open device. Ensure it is connected\n");
    return -1;
  }

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Tooltip");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
  gtk_container_set_border_width(GTK_CONTAINER(window), 15);
  
  t = gtk_table_new(1, 44, TRUE);
  gtk_table_set_row_spacings(GTK_TABLE(t), 2);
  gtk_table_set_col_spacings(GTK_TABLE(t), 2);

  for (int i = 0; i < 44; ++i) {

    char buf[32];
    snprintf(buf, sizeof(buf), "l%d", i);
    b = gtk_button_new_with_label(buf);
    gtk_table_attach_defaults(GTK_TABLE(t), b, i, i + 1, 0, 1);
    g_signal_connect(G_OBJECT(b), "clicked",
      G_CALLBACK(button_clicked), i);
  }

  gtk_container_add(GTK_CONTAINER(window), t);

  gtk_widget_show_all(window);
  

  g_signal_connect(G_OBJECT(window), "destroy",
    G_CALLBACK(gtk_main_quit), NULL);

  gtk_main();

out_close:
  libusb_close(h);
out:
  libusb_exit(ctx);

  return 0;
}
