#ifndef CONFIG_H_
# define CONFIG_H_

struct {
# define CONFIG_DEF(NAME, KEY) const char *NAME;
# include "config.def"
# undef CONFIG_DEF
} config;

void config_init(int argc, char **argv);
void config_cleanup(void);

#endif /* !CONFIG_H_ */
