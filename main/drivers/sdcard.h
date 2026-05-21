#ifndef SDCARD_H_
#define SDCARD_H_

#include <stddef.h>

int sdcard_init(void);
int sdcard_write(const char *path, const void *data, size_t len);
int sdcard_read(const char *path, void *buf, size_t len);

#endif
