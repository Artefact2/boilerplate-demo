/* Author: Romain "Artefact2" Dalmaso <artefact2@gmail.com> */

/* This program is free software. It comes without any warranty, to the
 * extent permitted by applicable law. You can redistribute it and/or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <demo_internal.h>
#include <stdint.h>
#include <stdbool.h>

#include "xmdata.h"

#define AUDIO_BUF_FRAMES 512
#define RATE 48000

static GLFWwindow* window;
static ao_device* aodev;
static xm_context_t* xmctx;
static double t0;

void glfw_error_callback(int code, const char* mesg) {
	DEBUG("GLFW error %i: %s", code, mesg);
}

void setup(void) {
	ao_initialize();

	ao_sample_format fmt = {
		.bits = 16,
		.rate = RATE,
		.channels = 2,
		.byte_format = AO_FMT_NATIVE,
		.matrix = "L,R",
	};

	aodev = ao_open_live(
		ao_default_driver_id(),
		&fmt,
		NULL
	);

	xm_create_context(&xmctx, (const char*)xmdata, RATE);
	//xm_set_max_loop_count(xmctx, 1);

	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();
	window = glfwCreateWindow(800, 600, "demo", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
}

void teardown(void) {
	glfwDestroyWindow(window);
	glfwTerminate();

	xm_free_context(xmctx);

	ao_close(aodev);
	ao_shutdown();
}

void* play_audio_loop(void* param) {
	float audiobuffer[AUDIO_BUF_FRAMES << 1];

	/* XXX: this is hackish */
	t0 = glfwGetTime() - 0.050;

	while(true) {
		/* Acquire semaphore for xmctx */
		//if(xm_get_loop_count(xmctx) != 0) break;
		xm_generate_samples(xmctx, audiobuffer, AUDIO_BUF_FRAMES);
		/* TODO: update timing stuff HERE */
		/* Release semaphore for xmctx */

		for(int i = 0; i < (AUDIO_BUF_FRAMES << 1); ++i) {
			((int16_t*)audiobuffer)[i] = (int16_t)((1 << 15) * audiobuffer[i]);
		}

		ao_play(aodev, (char*)audiobuffer, AUDIO_BUF_FRAMES << 2);
	}

	return param;
}

void render(void) {
	float ratio, w, z;
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
	glEnd();
}

int main(void) {
	pthread_t audiothread;

	setup();

	/* Start playing the module */
	pthread_create(&audiothread, NULL, play_audio_loop, NULL);

	while(!glfwWindowShouldClose(window)) {
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if(glfwGetKey(window, GLFW_KEY_ESCAPE) /*|| xm_get_loop_count(xmctx) > 0*/) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	teardown();
	return 0;
}
