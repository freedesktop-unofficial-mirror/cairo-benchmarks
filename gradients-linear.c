
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <cairo.h>
#include "tools.h"

#define NUM_RUNS 64
#define WIDTH 512
#define HEIGHT 512

#define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))

static int test (cairo_surface_t *surface)
{
    cairo_pattern_t *pattern;
    uint64_t before = 0;
    uint64_t after = 0;
    cairo_t *cr;

    cr = cairo_create (surface);

    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_rectangle (cr, 0, 0, WIDTH, HEIGHT);
    cairo_fill (cr);

    pattern = cairo_pattern_create_linear (0.0, 0.0, WIDTH, HEIGHT);
    cairo_pattern_add_color_stop_rgba (pattern, 1, 0, 0, 0, 1);
    cairo_pattern_add_color_stop_rgba (pattern, 0, 1, 1, 1, 1);
    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    cairo_set_source (cr, pattern);

    rdtscll (before);
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    cairo_rectangle (cr, 0, 0, WIDTH, HEIGHT);
    cairo_fill (cr);
    rdtscll (after);

    cairo_destroy (cr);
    cairo_pattern_destroy (pattern);

    return (int) (after - before);
}

int main( int argc, char **argv )
{
    cairo_surface_t *surface;
    int j;

    surface = output_create_surface (argv [0], WIDTH, HEIGHT);

    fprintf (stderr, "Testing gradients-linear...\n");
    test (surface);
    cairo_surface_write_to_png (surface, "gradients-linear-out.png");

    for (j = 0; j < NUM_RUNS; j++) {
        int cur = test (surface);
        fprintf (stderr, "\t%d: %d (%.2f ms)\n", j, cur,
                 get_milliseconds (cur));
    }

    cairo_surface_destroy( surface );
    output_cleanup ();
    return 0;
}

