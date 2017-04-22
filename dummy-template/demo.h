/* Author: Romain "Artefact2" Dalmaso <artefact2@gmail.com> */

/* This program is free software. It comes without any warranty, to the
 * extent permitted by applicable law. You can redistribute it and/or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <xm.h>

#if DEMOS_DEBUG
#include <stdio.h>
#include <stdlib.h>

#define d_litteral(s) #s

/* Impure assertion, may have side-effects, only the check is optimized out */
#define d_assert(x) do { if(!(x)) { fprintf(stderr, "assertion failed: %s in %s:%d\n", d_litteral(x), __FILE__, __LINE__); exit(1); } } while(0)

/* Pure assertion, no side effects, safe to optimize out */
#define d_assertp(x) d_assert(x)

/* Impure, assert something is zero */
#define d_assertz(x) d_assert((x) == 0)

/* Impure x, pure y, assert x and y are equal */
#define d_asserteq(x, y) d_assert((x) == (y))

#define d_debug(...) fprintf(stderr, __VA_ARGS__)

#else

#define d_assert(x) (x)
#define d_assertp(x)
#define d_assertz(x) (x)
#define d_asserteq(x, y) (x)
#define d_debug(...) 

#endif

struct demo_state_s {
	xm_context_t* xm; /* xm context */
	unsigned int rate; /* rate of xm context and pcm device */
	float ts; /* seconds since demo start */
	float ratio; /* aspect ratio of window */
};
typedef struct demo_state_s demo_state_t;

extern demo_state_t state;

void scene1_setup(void);
void scene1_render(void);
