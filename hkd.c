#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define INPUT_PREFIX  "/dev/input"
#define INPUT_NAME    "event0"

int main(void)
{
  int     ret;
  char    input_path[sizeof INPUT_PREFIX + sizeof '/' + sizeof INPUT_NAME + 1];
  int     fd;
  struct  input_event ev;

  sprintf(input_path, "%s/%s", INPUT_PREFIX, INPUT_NAME);

  fd = open(input_path, O_RDONLY);
  if (fd == -1)
    err(EXIT_FAILURE, "%s", input_path);

  while (true) {
    ret = read(fd, &ev, sizeof ev);

    switch (ret) {
      case -1:
        return EXIT_FAILURE;
      case 0:
    }

    printf("%u\n", ev.code);
  }

  return EXIT_SUCCESS;
}
