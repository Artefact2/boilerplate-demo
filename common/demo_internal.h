/* Author: Romain "Artefact2" Dalmaso <artefact2@gmail.com> */

/* This program is free software. It comes without any warranty, to the
 * extent permitted by applicable law. You can redistribute it and/or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#if DEMOS_DEBUG
#include <stdio.h>
#include <stdlib.h>

#define d_litteral(s) #s

#define d_assert(x) do { if(!(x)) { fprintf(stderr, "assertion failed: %s in %s:%d\n", d_litteral(x), __FILE__, __LINE__); exit(1); } } while(0)
#define d_assertz(x) d_assert((x) == 0)
#define d_asserteq(x, y) d_assert((x) == (y))

#define d_debug(...) fprintf(stderr, __VA_ARGS__)

#else

#define d_assert(x) (x)
#define d_assertz(x) (x)
#define d_asserteq(x, y) (x)
#define d_debug(...) 

#endif
