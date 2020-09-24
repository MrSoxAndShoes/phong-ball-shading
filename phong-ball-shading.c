/*
 * As noted in the article, this code was written for the
 * MetaWare High C Compiler. - EA 9/23/2020
 */

#include <math.h>
#include <stdio.h>
#include <time.h>

clock_t start, stop;
float pi;
int colors[] = { 3, 6, 10, 13, 6, 3, 10, 13, 6, 3, 13, 10 },
    d[] = { 640, 350, 1 }, i, k,
    palette[] = { 000, 010, 001, 011, 020, 002, 022, 077, 040, 004, 044, 060, 006, 066, 007, 077 },
    x, y, x_min, x_max, y_min, y_max;
unsigned short random;

main()
{
	float a, b, c, l0, l1, l2, ln1, n0, n1, n2, p, q, r = 128, s, t, v[12][3];
	int n;

	/* Put EGA in hi-res graphics mode & initialize palette. */
	video_int(0x10);
	for (n = 0; n < 16; n++)
		video_int(0x1000, n + (palette[n] << 8));

	/* Print title & start timing. */
#ifdef Intel
	printf("\n\n\t\t       80387 Phong Shading Demonstration\n");
#else
	printf("\n\n\t\t    Weitek 1167 Phong Shading Demonstration\n");
#endif
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n \n\n\n\n\n\n\n");
	start = clock();

	/* Pixel aspect ratio. */
	a = 1.3;

	/* Screen center coordinates. */
	b = .5 * (d[0]-1);
	c = .5 * (d[1]-1);

	/* Unit length light source vector. */
	l0 = -1 / sqrt(3.);
	l1 = l0;
	l2 = -l0;

	/* Ratio of circumference to diamater or a circle. */
	pi = 4 * atan(1.);

	/* A dozen vertices evenly spread over a unit sphere. */
	v[0][0] = 0;
	v[0][1] = 0;
	v[0][2] = 1;

	s = sqrt(5.);

	for (i = 1; i < 11; i++) {
		p = pi * i / 5;
		v[i][0] = 2 * cos(p) / s;
		v[i][1] = 2 * sin(p) / s;
		v[i][2] = (1. - i % 2 * 2) / s;
	}

	v[11][0] =  0;
	v[11][1] =  0;
	v[11][2] = -1;

	/* Loop to Phong shade each pixel. */
	y_max = c + r;
	y_min = 2 * c - y_max;

	for (y = y_min; y <= y_max; y++) {
		s = y-c;
		n1 = s / r;
		ln1 = l1 * n1;
		sx = r*r - s*s;
		x_max = b + a * sqrt(s);
		x_min = 2 * b - x_max;

		for (x = x_min; x <= x_max; x++) {
			t = (x - b) / a;
			n0 = t / r;
			t = sqrt(s - t*t);
			n2 = t / r;

			/* Compute dot product and clamp to positive view. */
			ln = l0 * n0 + ln1 + l2 * n2;
			if (ln < 0) ln = 0;

			/* cos(e.r) ** 27 */
			t = ln * n2;
			t += t - l2;
			t *= t * t;
			t *= t * t;
			t *= t * t;

			/* Nearest vertex to normal yields max dot product. Get its color. */
			for (i = 0, p = 0; i < 11; i++) {
				if (p < (q = n0 * v[i][0] + n1 * v[i][1] + n2 * v[i][2])) {
					p = q;
					k = colors[i];
				}
			}

			/* Aggregate ambient, diffuse, & specular intensities & do dither. */
			i = k - 2.5  + 2.5 * ln + t + (random = 37 * random + 1) / 65536.;

			/* Clamp values outside range of three color levels to black or white. */
			if (i < k - 2) i = 0; else if (i > k) i = 15;

			draw_dot();
		}
	}

	stop = clock();
	reg_reset();

	printf("\t\t\t\tTime = %3.2f seconds", ((double) (stop - start)) / CLK_TCK);
	while (clock() - stop < 5 * CLK_TCK);

	/* Put EGA in 80-column mode. */
	video_int(0x03);
}
