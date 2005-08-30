
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <cairo.h>
#include "tools.h"

#define NUM_RUNS 64
#define WIDTH 640
#define HEIGHT 480

#define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))

static int test (cairo_surface_t *surface)
{
    uint64_t before = 0;
    uint64_t after = 0;
    cairo_t *cr;

    cr = cairo_create (surface);

    rdtscll (before);
    cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
    cairo_identity_matrix (cr);
    cairo_rectangle (cr, 0.0, 0.0, WIDTH, HEIGHT);
    cairo_rectangle (cr, 200.0, 200.0, 100.0, 100.0);
    //cairo_rectangle (cr, 0, 0, width, 200);
    //cairo_rectangle (cr, 0, 200, 200, 100);
    //cairo_rectangle (cr, width - 200, 200, 200, 100);
    cairo_clip      (cr);
    cairo_new_path  (cr);

    cairo_translate (cr, 0, 0);
    cairo_rectangle (cr, 0, 0, WIDTH, HEIGHT);
    cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
    cairo_fill (cr);
    rdtscll (after);

    cairo_destroy (cr);

    return (int) (after - before);
}

int main( int argc, char **argv )
{
    cairo_surface_t *surface;
    int j;

    surface = output_create_surface (argv [0], WIDTH, HEIGHT);

    fprintf (stderr, "Testing multiple clip rectangles...\n");
    test (surface);
    cairo_surface_write_to_png (surface, "multiple-clip-surfaces-out.png");
    
    for (j = 0; j < NUM_RUNS; j++) {
        int cur = test (surface);
        fprintf (stderr, "\t%d: %d (%.2f ms)\n", j, cur,
                 get_milliseconds (cur));
    }

    cairo_surface_destroy( surface );
    output_cleanup ();
    return 0;
}



