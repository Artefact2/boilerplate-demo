/* Author: Romain "Artefact2" Dalmaso <artefact2@gmail.com> */

/* This program is free software. It comes without any warranty, to the
 * extent permitted by applicable law. You can redistribute it and/or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include "demo.h"

void scene1_setup(void) {

}

void scene1_render(void) {
	float w = 1.0, z = 0.0;
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-state.ratio, state.ratio, -1.f, 1.f, 1.f, -1.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(30.f * (state.ts + logf(xm_get_frequency_of_channel(state.xm, 1))/logf(2.0)), 0.f, 0.f, 1.f);
	
	glBegin(GL_TRIANGLES);
	z = state.ts - (float)xm_get_latest_trigger_of_channel(state.xm, 2) / state.rate;
	glColor3f(1.f, z, z);
	glVertex3f(-0.5f*w, 0.8660254037844386f*w, 0.f);
	
	z = state.ts - (float)xm_get_latest_trigger_of_channel(state.xm, 3) / state.rate;
	glColor3f(z, 1.f, z);
	glVertex3f(-0.5f*w, -0.8660254037844386f*w, 0.f);
	
	z = state.ts - (float)xm_get_latest_trigger_of_channel(state.xm, 4) / state.rate;
	glColor3f(z, z, 1.f);
	glVertex3f(1.0f*w, 0.f, 0.f);
	glEnd();
}
