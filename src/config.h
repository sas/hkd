#ifndef CONFIG_H_
# define CONFIG_H_

struct {
# define CONFIG_DEF(NAME) const char *NAME;
# define CONFIG_KEY_DEF(NAME, KEY) CONFIG_DEF(NAME)
# include "config.def"
# undef CONFIG_DEF
# undef CONFIG_KEY_DEF
} config;

void config_init(int argc, char **argv);
void config_cleanup(void);

#endif /* !CONFIG_H_ */
