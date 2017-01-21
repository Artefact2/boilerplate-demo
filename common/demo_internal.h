/* Author: Romain "Artefact2" Dalmaso <artefact2@gmail.com> */

/* This program is free software. It comes without any warranty, to the
 * extent permitted by applicable law. You can redistribute it and/or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <ao/ao.h>
#include <xm.h>
#include <GLFW/glfw3.h>
#include <pthread.h>

#if DEMOS_DEBUG
#include <stdio.h>

#define DEBUG0(message) do {	  \
		fprintf(stderr, "%s(): " message "\n", __func__); \
		fflush(stderr); \
	} while(0)

#define DEBUG(fmt, ...) do {	  \
		fprintf(stderr, "%s(): " fmt "\n", __func__, __VA_ARGS__); \
		fflush(stderr); \
	} while(0)

#else
#define DEBUG0(message)
#define DEBUG(...)
#endif

#define MIN_2f(x, y) ((x) > (y) ? (y) : (x))
#define MAX_2f(x, y) ((x) > (y) ? (x) : (y))
#define ABS_1f(x) ((x) > 0.f ? (x) : -(x))

#define CLAMP_UP_2f(x, up) ((x) > (up) ? (up) : (x))
#define CLAMP_DOWN_2f(x, down) ((x) < (down) ? (down) : (x))
#define CLAMP_3f(x, up, down) CLAMP_UP_2f(CLAMP_DOWN_2f((x), (down)), (up))

#define CLAMP_UP_1f(x) CLAMP_UP_2f((x), 1.f)
#define CLAMP_DOWN_1f(x) CLAMP_DOWN_2f((x), 0.f)
#define CLAMP_1f(x) CLAMP_3f((x), 1.f, 0.f)

#define LERP_5f(x1,y1,x2,y2,x) ((y1) + ((x)-(x1))/((x2)-(x1))*((y2)-(y1)))

#define BEAT_3f(curtime, beattime, beatduration)	  \
	(((beattime) == 0) ? 0.f : CLAMP_1f(LERP_5f( \
		(beattime), \
		1.f, \
		(beattime) + (beatduration), \
		0.f, \
		(curtime) \
	)))
