#include "config.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void strreplace(char *str, char from, char to)
{
  while (*str != '\0') {
    if (*str == from)
      *str = to;
    ++str;
  }
}

static char *strstrip(char *str)
{
  char *end;

  while (isspace(*str))
    ++str;

  end = str + strlen(str);

  while (end > str && isspace(*(end - 1)))
    --end;

  *end = '\0';

  return str;
}

void config_init(int argc, char **argv)
{
  const char *conf_path = argc == 1 ? "/etc/hkd.conf" : argv[1];
  FILE *conf;
  char *line;
  size_t length;
  bool found_errors;

  conf = fopen(conf_path, "r");
  if (conf == NULL)
    err(EXIT_FAILURE, "%s", conf_path);

  line = NULL;
  found_errors = false;

  while (getline(&line, &length, conf) != -1) {
#define CONFIG_ERROR(...) do {  \
  warnx(__VA_ARGS__);           \
  found_errors = true;          \
  goto cleanup;                 \
} while (0)

    char *key;
    char *value;
    bool found;

    key = line;
    value = strchr(line, '=');

    if (value == NULL)
      CONFIG_ERROR("%s: invalid line `%s'", conf_path, strstrip(line));

    *value = '\0';
    ++value;

    key = strstrip(key);
    value = strstrip(value);

    found = false;

#define CONFIG_DEF(NAME, KEY) {                                   \
  char name[] = #NAME;                                            \
                                                                  \
  strreplace(name, '_', '-');                                     \
                                                                  \
  if (strcmp(key, name) == 0) {                                   \
    if (config.NAME != NULL)                                      \
      CONFIG_ERROR("%s: duplicate entry `%s'", conf_path, name);  \
                                                                  \
    config.NAME = strdup(value);                                  \
    found = true;                                                 \
  }                                                               \
                                                                  \
}
#include "config.def"
#undef CONFIG_DEF

    if (!found)
      CONFIG_ERROR("%s: invalid entry `%s'", conf_path, key);

cleanup:
    free(line);
    line = NULL;
  }

  fclose(conf);

  if (found_errors)
    exit(EXIT_FAILURE);

#undef CONFIG_ERROR
}

void config_cleanup(void)
{
#define CONFIG_DEF(NAME, KEY) free((char*) config.NAME);
#include "config.def"
#undef CONFIG_DEF
}
