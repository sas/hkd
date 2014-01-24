#include "config.h"

#include <err.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define INPUT_PREFIX  "/dev/input"
#define INPUT_NAME    "event0"

static void do_stuff(struct input_event *ev)
{
  if (ev->type != EV_KEY || ev->value != 1)
    return;

  switch (ev->code) {
#define CONFIG_DEF(NAME, KEY) case KEY: if (config.NAME) system(config.NAME); break;
#include "config.def"
#undef CONFIG_DEF
  }
}

int main(int argc, char **argv)
{
  int     ret;
  char    input_path[sizeof INPUT_PREFIX + sizeof '/' + sizeof INPUT_NAME + 1];
  int     fd;
  struct  input_event ev;

  config_init(argc, argv);

  sprintf(input_path, "%s/%s", INPUT_PREFIX, INPUT_NAME);

  while (true) {
    fd = open(input_path, O_RDONLY);
    if (fd == -1)
      err(EXIT_FAILURE, "%s", input_path);

    while (true) {
      ret = read(fd, &ev, sizeof ev);

      switch (ret) {
        case sizeof ev:
          do_stuff(&ev);
          break;

        case -1:
          /* Error occured while reading, warn and restart. */
          warn("%s", input_path);
        case 0:
          /* EOF reached, this should not happen. Restart. */
          goto cleanup;

        default:
          /*
           * If we did not do a full read, we can't use the contents of the
           * input_event structure. Just close everything and restart.
           */
          warnx("%s: invalid read of size %d", input_path, ret);
          goto cleanup;
      }
    }

cleanup:
    close(fd);
  }

  config_cleanup();

  return EXIT_SUCCESS;
}
