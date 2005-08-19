
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <cairo.h>
#include "tools.h"
#include "setup.h"

static int width = 512;
static int height = 512;

#define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))

const char *text[] = {
    "Cairo provides a stateful",
    "user-level API with",
    "capabilities similar to the",
    "PDF 1.4 imaging model.",
    "Cairo provides operations",
    "including stroking and",
    "filling Bezier cubic",
    "splines, transforming",
    "and compositing",
    "translucent images, and",
    "antialiased text rendering." };

static int test (cairo_surface_t *surface)
{
    uint64_t before = 0;
    uint64_t after = 0;
    cairo_t *cr;
    int i;

    cr = cairo_create (surface);

    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_rectangle (cr, 0, 0, width, height);
    cairo_fill (cr);

    cairo_set_line_width (cr, 0.8);
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);

    cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                            CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 32.0);

    rdtscll (before);
    for (i = 0; i < sizeof (text) / sizeof (const char *); i++) {
        cairo_move_to (cr, 10.5, 40.5 + (i * 45));
        cairo_text_path (cr, text [i]);
        cairo_set_source_rgb (cr, 0.85, 0.85, 0.75);
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

    surface = output_create_surface (argv [0], width, height);

    fprintf (stderr, "Testing textpath...\n");
    test (surface);
    cairo_surface_write_to_png (surface, "textpath-out.png");

    for (j = 0; j < NUM_RUNS; j++) {
        int cur = test (surface);
        fprintf (stderr, "\t%d: %d (%.2f ms)\n", j, cur,
                 get_milliseconds (cur));
    }

    cairo_surface_destroy( surface );
    output_cleanup ();
    return 0;
}

