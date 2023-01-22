#ifndef SWITCHER_H_GUARD
#define SWITCHER_H_GUARD

#include <stdint.h>

enum {
	SWTCHR_NONE = 0,
	SWTCHR_TTY = 1,
	SWTCHR_LOCK = 2,
	SWTCHR_VIM = 3,
	SWTCHR_BROWSER = 4,
};

enum {
	SWTCHR_SELF = 0,
	SWTCHR_UP = 1,
	SWTCHR_DOWN = 2,
	SWTCHR_LEFT = 3,
	SWTCHR_RIGHT = 4,

	SWTCHR_OFF = 0,
	SWTCHR_ON = 1,
};

struct switcher_packet {
	uint8_t id;
	uint16_t payload;
	uint8_t pad[28];
};

_Static_assert(sizeof(struct switcher_packet) == 32);

#endif // SWITCHER_H_GUARD
