
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
    uint64_t before = 0;
    uint64_t after = 0;
    cairo_t *cr;
    int i;

    cr = cairo_create (surface);

    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_rectangle (cr, 0, 0, WIDTH, HEIGHT);
    cairo_fill (cr);

    cairo_set_line_width (cr, 1.0);
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);

    rdtscll (before);
    for (i = 0; i < WIDTH + 1; i += 16) {
        cairo_move_to (cr, 0.5, i + 0.5);
        cairo_line_to (cr, i + 0.5, HEIGHT - 0.5);
        cairo_stroke (cr);
    }
    for (i = 0; i < WIDTH + 1; i += 16) {
        cairo_move_to (cr, i + 0.5, HEIGHT - 0.5);
        cairo_line_to (cr, WIDTH - 0.5, HEIGHT - i - 0.5);
        cairo_stroke (cr);
    }
    for (i = 0; i < WIDTH + 1; i += 16) {
        cairo_move_to (cr, WIDTH - 0.5, HEIGHT - i - 0.5);
        cairo_line_to (cr, WIDTH - i - 0.5, 0.5);
        cairo_stroke (cr);
    }
    for (i = 0; i < WIDTH + 1; i += 16) {
        cairo_move_to (cr, WIDTH - i - 0.5, 0.5);
        cairo_line_to (cr, 0.5, i + 0.5);
        cairo_stroke (cr);
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

    fprintf (stderr, "Testing lines...\n");
    test (surface);
    cairo_surface_write_to_png (surface, "lines-out.png");

    for (j = 0; j < NUM_RUNS; j++) {
        int cur = test (surface);
        fprintf (stderr, "\t%d: %d (%.2f ms)\n", j, cur,
                 get_milliseconds (cur));
    }

    cairo_surface_destroy( surface );
    output_cleanup ();
    return 0;
}

