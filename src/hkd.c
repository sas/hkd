#include "config.h"

#include <err.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static void do_stuff(struct input_event *ev)
{
  if (ev->type != EV_KEY || ev->value != 1)
    return;

  switch (ev->code) {
#define CONFIG_DEF(NAME)
#define CONFIG_KEY_DEF(NAME, KEY) case KEY: if (config.NAME != NULL) system(config.NAME); break;
#include "config.def"
#undef CONFIG_DEF
#undef CONFIG_KEY_DEF
  }
}

int main(int argc, char **argv)
{
  int     ret;
  int     fd;
  struct  input_event ev;

  config_init(argc, argv);

  while (true) {
    fd = open(config.input_device, O_RDONLY);
    if (fd == -1)
      err(EXIT_FAILURE, "%s", config.input_device);

    while (true) {
      ret = read(fd, &ev, sizeof ev);

      switch (ret) {
        case sizeof ev:
          do_stuff(&ev);
          break;

        case -1:
          /* Error occured while reading, warn and restart. */
          warn("%s", config.input_device);
        case 0:
          /* EOF reached, this should not happen. Restart. */
          goto cleanup;

        default:
          /*
           * If we did not do a full read, we can't use the contents of the
           * input_event structure. Just close everything and restart.
           */
          warnx("%s: invalid read of size %d", config.input_device, ret);
          goto cleanup;
      }
    }

cleanup:
    close(fd);
  }

  config_cleanup();

  return EXIT_SUCCESS;
}
