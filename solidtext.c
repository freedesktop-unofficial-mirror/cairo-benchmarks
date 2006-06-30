
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <cairo.h>
#include "tools.h"

#define NUM_RUNS 16
#define WIDTH 512
#define HEIGHT 512

#define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))

const char *text[] = {
    "this is just a bunch of text",
    "the particular words don't matter",
    "but maybe the performance does"
};

static int test (cairo_surface_t *surface)
{
    cairo_pattern_t *pattern;
    uint64_t before = 0;
    uint64_t after = 0;
    cairo_t *cr;
    int i;

    cr = cairo_create (surface);

    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_rectangle (cr, 0, 0, WIDTH, HEIGHT);
    cairo_fill (cr);

    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

    cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
			    CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 14.0);

    rdtscll (before);
    for (i = 0; i < sizeof (text) * 10 / sizeof (const char *); i++) {
	cairo_move_to (cr, 10, 10.5 + (i * 16));
	if ((i & 1) == 0)
	    pattern = cairo_pattern_create_rgb (1.0, 0.0, 0.0);
	else
	    pattern = cairo_pattern_create_rgb (0.0, 1.0, 0.0);
	cairo_set_source (cr, pattern);
	cairo_pattern_destroy (pattern);
	cairo_show_text (cr, text[i % 3]);
    }
    rdtscll (after);

    cairo_destroy (cr);

    return (int) (after - before);
}

int main( int argc, char **argv )
{
    cairo_surface_t *surface;
    int j;

    surface = output_create_surface (argv [0], WIDTH, HEIGHT);

    fprintf (stderr, "Testing solidtext...\n");
    test (surface);
    cairo_surface_write_to_png (surface, "solidtext-out.png");

    for (j = 0; j < NUM_RUNS; j++) {
	int cur = test (surface);
	fprintf (stderr, "\t%d: %d (%.2f ms)\n", j, cur,
		 get_milliseconds (cur));
    }

    cairo_surface_destroy( surface );
    output_cleanup ();
    return 0;
}

