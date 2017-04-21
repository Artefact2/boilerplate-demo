/* Author: Romain "Artefact2" Dalmaso <artefact2@gmail.com> */

/* This program is free software. It comes without any warranty, to the
 * extent permitted by applicable law. You can redistribute it and/or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <alsa/asoundlib.h>

#include <xm.h>
#include <demo_internal.h>
#include "xmdata.h"

static xm_context_t* xmctx;

static snd_pcm_t* device;
static snd_pcm_uframes_t bsize = 1024, psize = 128;
static unsigned int rate = 48000;
static const snd_pcm_channel_area_t* areas;
static snd_pcm_uframes_t offset, frames;

/* https://www.khronos.org/opengl/wiki/Programming_OpenGL_in_Linux:_GLX_and_Xlib */

void setup(void) {
	void* params;
	
	d_assertz(snd_pcm_open(&device, "default", SND_PCM_STREAM_PLAYBACK, 0));
	snd_pcm_hw_params_malloc((snd_pcm_hw_params_t**)(&params));
	snd_pcm_hw_params_any(device, params);
	d_assertz(snd_pcm_hw_params_set_access(device, params, SND_PCM_ACCESS_MMAP_INTERLEAVED));
	d_assertz(snd_pcm_hw_params_set_format(device, params, SND_PCM_FORMAT_FLOAT));
	d_assertz(snd_pcm_hw_params_set_rate_resample(device, params, 0));
	d_assertz(snd_pcm_hw_params_set_rate_near(device, params, &rate, 0));
	d_assertz(snd_pcm_hw_params_set_channels(device, params, 2));
	d_assertz(snd_pcm_hw_params_set_buffer_size_near(device, params, &bsize));
	d_assertz(snd_pcm_hw_params_set_period_size_near(device, params, &psize, 0));
	d_assertz(snd_pcm_hw_params(device, params));
	snd_pcm_hw_params_free(params);

	d_debug("ALSA: rate %u, period size %lu, buffer size %lu\n", rate, psize, bsize);

	snd_pcm_sw_params_malloc((snd_pcm_sw_params_t**)(&params));
	snd_pcm_sw_params_current(device, params);
	d_assertz(snd_pcm_sw_params(device, params));
	d_assertz(snd_pcm_sw_params_set_start_threshold(device, params, bsize - psize));
	d_assertz(snd_pcm_sw_params_set_avail_min(device, params, psize));
	snd_pcm_sw_params_free(params);
	
	d_assertz(xm_create_context(&xmctx, (const char*)xmdata, rate));
	
	d_assertz(snd_pcm_prepare(device));
	d_assertz(snd_pcm_start(device));
}

void teardown(void) {
	snd_pcm_drain(device);
	snd_pcm_close(device);
	xm_free_context(xmctx);
}

void render(void) {
	if((frames = snd_pcm_avail_update(device)) >= psize) {
		frames -= (frames % psize);
		d_assertz(snd_pcm_mmap_begin(device, &areas, &offset, &frames));
		xm_generate_samples(xmctx, (float*)((char*)(areas[0].addr) + (offset << 3)), frames);
		d_asserteq(snd_pcm_mmap_commit(device, offset, frames), frames);
	} else snd_pcm_wait(device, 1); /* XXX: tight loop bugs out randomly, only silence after it */

	static char* notes[] = { "A-", "A#", "B-", "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#" };

	printf("\n");
	for(int i = 1; i <= xm_get_number_of_channels(xmctx); ++i) {
		if(!xm_is_channel_active(xmctx, i)) {
			printf("... .. ..  ");
			continue;
		}

		float freq = xm_get_frequency_of_channel(xmctx, i);
		int oct = (int)((logf(freq / 440.0) / logf(2.0)) - 2.0);
		int note = (int)(fmodf(logf(freq / 440.0) / logf(2.0) + 10.0, 1.0) * 12.0 + .5) % 12;
		d_assert(note >= 0 && note <= 11);
		printf("%02s%01d %02d %02x  ", notes[note], oct, xm_get_instrument_of_channel(xmctx, i), (unsigned int)(64.0 * xm_get_volume_of_channel(xmctx, i)));
	}
	
/*	float ratio, w, z;
	int width, height;
	double l, t;

	l = (double)xm_get_latest_trigger_of_instrument(xmctx, 0x2) / RATE;
	t = glfwGetTime() - t0;

	z = BEAT_3f(t, l, .3f);

	l = (double)xm_get_latest_trigger_of_instrument(xmctx, 0x3) / RATE;
	w = 1.f + .1f * BEAT_3f(t, l, .2f);

	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float) height;

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
	glBegin(GL_TRIANGLES);
	glColor3f(1.f, z, z);
	glVertex3f(-0.5f*w, 0.8660254037844386f*w, 0.f);
	glColor3f(z, 1.f, z);
	glVertex3f(-0.5f*w, -0.8660254037844386f*w, 0.f);
	glColor3f(z, z, 1.f);
	glVertex3f(1.0f*w, 0.f, 0.f);
	glEnd();*/
}

int main(void) {
	setup();

	while(1) render();

	/*while(!glfwWindowShouldClose(window)) {
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}*/

	teardown();
	return 0;
}
