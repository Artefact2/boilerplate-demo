/* Author: Romain "Artefact2" Dalmaso <artefact2@gmail.com> */

/* This program is free software. It comes without any warranty, to the
 * extent permitted by applicable law. You can redistribute it and/or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include "demo.h"

static float colors[] = {
	1.f, 0.f, .584f,
	0.f, .082f, 1.f,
	0.f, 1.f, .415f,
	1.f, .917f, 0.f,
};

void scene2_setup(void) {

}

void scene2_render(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-state.ratio, state.ratio, -1.f, 1.f, 1.f, -1.f);

	for(int i = 1; i <= 4; ++i) {
		if(!xm_is_channel_active(state.xm, i)) continue;
		
		float vol = sqrt(xm_get_volume_of_channel(state.xm, i));
		int ins = xm_get_instrument_of_channel(state.xm, i) - 1;
		float trig = .5f * (state.ts - xm_get_latest_trigger_of_channel(state.xm, i) / state.rate);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glScalef(.5f, .5f, .5f);
		glRotatef(90.f * i, 0.f, 0.f, 1.f);
		glTranslatef(1.f, 1.f, 0.f);
		glRotatef(30.f * (state.ts + logf(xm_get_frequency_of_channel(state.xm, i))/logf(2.0)), 0.f, 0.f, 1.f);
		glScalef(vol, vol, vol);

		glBegin(GL_TRIANGLES);

		glColor3f(colors[3*ins] + trig, colors[3*ins+1] + trig, colors[3*ins+2] + trig);
		glVertex3f(-.5f, .866f, 0.f);
		glVertex3f(-.5f, -.866f, 0.f);
		glVertex3f(1.f, 0.f, 0.f);
		
		glEnd();
	}
}
