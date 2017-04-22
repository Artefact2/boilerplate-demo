/* Author: Romain "Artefact2" Dalmaso <artefact2@gmail.com> */

/* This program is free software. It comes without any warranty, to the
 * extent permitted by applicable law. You can redistribute it and/or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include "demo.h"
#include "mus.h"

#define DEFAULT_RATE 48000
#define DEFAULT_PSIZE 256
#define DEFAULT_BSIZE 2048
#define DEFAULT_WIDTH 1920
#define DEFAULT_HEIGHT 1080

static snd_pcm_t* pcm;

static Display* display;
static Window window;

static int ww = DEFAULT_WIDTH, wh = DEFAULT_HEIGHT;
static bool stop = false;

demo_state_t state;

static void gen_audio_samples(snd_async_handler_t* data) {	
	const snd_pcm_channel_area_t* areas;
	snd_pcm_uframes_t offset, frames;

	if(snd_pcm_state(pcm) == 4) { /* xrun? */
		/* XXX doesn't always work */
		snd_pcm_recover(pcm, -EPIPE, 0);
		//snd_pcm_drop(pcm);
		snd_pcm_prepare(pcm);
		snd_pcm_start(pcm);
	}
	
	if((frames = snd_pcm_avail_update(pcm)) > 0) {
		d_assertz(snd_pcm_mmap_begin(pcm, &areas, &offset, &frames));
		xm_generate_samples(state.xm, (float*)((char*)(areas[0].addr) + (offset << 3)), frames);
		d_asserteq(snd_pcm_mmap_commit(pcm, offset, frames), frames);
		state.ts += (float)frames / state.rate;
	}
}

static void setup(void) {
	void* params;
	snd_pcm_uframes_t bsize = DEFAULT_BSIZE, psize = DEFAULT_PSIZE;
	snd_async_handler_t* ah;

	GLint att[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };
	Window root;
	GLXContext glc;
	XVisualInfo* vi;
	XSetWindowAttributes swa;
	
	d_assertz(snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0));
	snd_pcm_hw_params_malloc((snd_pcm_hw_params_t**)(&params));
	snd_pcm_hw_params_any(pcm, params);
	d_assertz(snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_MMAP_INTERLEAVED));
	d_assertz(snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_FLOAT));
	d_assertz(snd_pcm_hw_params_set_rate_resample(pcm, params, 0));
	state.rate = DEFAULT_RATE;
	d_assertz(snd_pcm_hw_params_set_rate_near(pcm, params, &state.rate, 0));
	d_assertz(snd_pcm_hw_params_set_channels(pcm, params, 2));
	d_assertz(snd_pcm_hw_params_set_buffer_size_near(pcm, params, &bsize));
	d_assertz(snd_pcm_hw_params_set_period_size_near(pcm, params, &psize, 0));
	d_assertz(snd_pcm_hw_params(pcm, params));
	snd_pcm_hw_params_free(params);

	d_debug("ALSA: rate %u, period size %lu, buffer size %lu\n", state.rate, psize, bsize);
	state.ts = -(float)bsize / state.rate; /* Compensate for buffer latency, may do more harm than good */

	/* XXX is this really necessary? */
	/*snd_pcm_sw_params_malloc((snd_pcm_sw_params_t**)(&params));
	snd_pcm_sw_params_current(pcm, params);
	d_assertz(snd_pcm_sw_params(pcm, params));
	d_assertz(snd_pcm_sw_params_set_start_threshold(pcm, params, bsize - psize));
	d_assertz(snd_pcm_sw_params_set_avail_min(pcm, params, psize));
	snd_pcm_sw_params_free(params);*/
	
	d_assertz(xm_create_context(&state.xm, (const char*)mus, state.rate));

	/* https://www.khronos.org/opengl/wiki/Programming_OpenGL_in_Linux:_GLX_and_Xlib */
	display = XOpenDisplay(NULL);
	d_assertp(display != NULL);
	root = DefaultRootWindow(display);
	vi = glXChooseVisual(display, 0, att);
	d_assertp(vi != NULL);
	swa.colormap = XCreateColormap(display, root, vi->visual, AllocNone);
	swa.event_mask = KeyPressMask | StructureNotifyMask;
	window = XCreateWindow(display, root, 0, 0, ww, wh, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	XMapWindow(display, window);
	state.ratio = (float)ww / (float)wh;

	glc = glXCreateContext(display, vi, NULL, True);
	d_assertp(glc != NULL);
	glXMakeCurrent(display, window, glc);

	scene1_setup();
	scene2_setup();
	
	d_assertz(snd_async_add_pcm_handler(&ah, pcm, gen_audio_samples, NULL));
	d_assertz(snd_pcm_prepare(pcm));
	gen_audio_samples(NULL);
	sleep(1); /* XXX */
	d_assertz(snd_pcm_start(pcm));
}

static void render(void) {
	unsigned char pat;
	
	while(XPending(display)) {
		XEvent xev;
		XNextEvent(display, &xev);

		if(xev.type == KeyPress) {
			KeySym s = XLookupKeysym(&xev.xkey, 0);
			if(s == XK_Escape || s == XK_q) {
				stop = true;
			}
		} else if(xev.type == ConfigureNotify) {
			ww = xev.xconfigure.width;
			wh = xev.xconfigure.height;
			state.ratio = (float)ww / (float)wh;
		}
	}
	
	glViewport(0, 0, ww, wh);

	xm_get_position(state.xm, NULL, &pat, NULL, NULL);
	if((pat >> 2) & 1) {
		scene1_render();
	} else {
		scene2_render();
	}
	
	glXSwapBuffers(display, window);
}

int main(void) {
	setup();
	while(!stop) render();
	exit(0);
	return 0;
}
