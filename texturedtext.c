
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

const char *text[] = {
    "C41r0",
    "r0x0rz",
    "!!11!" };

static int test (cairo_surface_t *surface, const char *png_filename)
{
    cairo_surface_t *image = cairo_image_surface_create_from_png (png_filename);
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
    cairo_set_font_size (cr, 130.0);

    pattern = cairo_pattern_create_for_surface (image);
    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    cairo_set_source (cr, pattern);

    rdtscll (before);
    for (i = 0; i < sizeof (text) / sizeof (const char *); i++) {
        cairo_text_extents_t extents;
        double x;

        cairo_text_extents (cr, text [i], &extents);
        x = ((WIDTH - extents.width) / 2.0) + 0.5;

        cairo_move_to (cr, x, 196.5 + (i * 128));
        cairo_text_path (cr, text [i]);
        cairo_set_source (cr, pattern);
        cairo_fill_preserve (cr);
        cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
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

    fprintf (stderr, "Testing texturedtext...\n");
    test (surface, "256x256-darkgnome.png");
    cairo_surface_write_to_png (surface, "texturedtext-out.png");

    for (j = 0; j < NUM_RUNS; j++) {
        int cur = test (surface, "256x256-darkgnome.png");
        fprintf (stderr, "\t%d: %d (%.2f ms)\n", j, cur,
                 get_milliseconds (cur));
    }

    cairo_surface_destroy( surface );
    output_cleanup ();
    return 0;
}

