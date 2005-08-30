#include <sys/time.h>
#include <cairo.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 400
#define HEIGHT 300
#define NUM_RUNS 100
#define M_PI 3.14159265358979323846

static int square = 1;

void
rounded_rectangle (cairo_t *cr,
		   double x, double y, double w, double h,
		   double radius)
{
    cairo_move_to (cr, x+radius, y);
    cairo_arc (cr, x+w-radius, y+radius,   radius, M_PI + M_PI / 2, M_PI * 2        );
    cairo_arc (cr, x+w-radius, y+h-radius, radius, 0,               M_PI / 2        );
    cairo_arc (cr, x+radius,   y+h-radius, radius, M_PI/2,          M_PI            );
    cairo_arc (cr, x+radius,   y+radius,   radius, M_PI,            270 * M_PI / 180);
}

static void
run_benchmark (cairo_surface_t *surface)
{
    struct timeval before;
    struct timeval after;
    int i;

    cairo_t *cr;
    cairo_pattern_t *pattern;

    gettimeofday (&before, NULL);
    for (i=0; i < NUM_RUNS; i++)
    {
	cr = cairo_create (surface);

	if (square)
	    cairo_rectangle (cr, 0, 0, WIDTH, HEIGHT);
	else
	    rounded_rectangle (cr, 0, 0, WIDTH, HEIGHT, 3.0);

	pattern = cairo_pattern_create_linear (0, 0, 0, HEIGHT);
	cairo_pattern_add_color_stop_rgb (pattern, 0, 1, 0, 0);
	cairo_pattern_add_color_stop_rgb (pattern, 1, 0, 0, 0);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern);

	cairo_fill (cr);
	cairo_destroy (cr);
    }
    gettimeofday (&after, NULL);

    fprintf (stderr, "%g msec\n",
	     (after.tv_sec - before.tv_sec) * 1000. +
	     (after.tv_usec - before.tv_usec) / 1000.);
}

int
main (int argc, char *argv[])
{
    cairo_surface_t *surface;
  
    if (argc != 2) {
	fprintf (stderr, "Usage: gradient-test [0/1] (0 == rounded, 1 == square)\n");
	exit (1);
    }

    square = atoi (argv[1]);

    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, WIDTH, HEIGHT);

    run_benchmark (surface);

    return 0;
}

