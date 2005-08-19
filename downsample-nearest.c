
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <cairo.h>
#include "tools.h"
#include "setup.h"

static int width = 64;
static int height = 64;

#define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))

static int test (cairo_surface_t *surface, const char *png_filename)
{
    cairo_surface_t *image = cairo_image_surface_create_from_png (png_filename);
    cairo_pattern_t *pattern;
    uint64_t before = 0;
    uint64_t after = 0;
    cairo_t *cr;

    cr = cairo_create (surface);

    cairo_set_source_rgba (cr, 0, 0, 0, 0);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_rectangle (cr, 0, 0, width, height);
    cairo_fill (cr);

    cairo_scale (cr, 1.0 / 8.0, 1.0 / 8.0);

    pattern = cairo_pattern_create_for_surface (image);
    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_NONE);
    cairo_pattern_set_filter (pattern, CAIRO_FILTER_NEAREST);
    cairo_set_source (cr, pattern);

    rdtscll (before);
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    cairo_paint (cr);
    rdtscll (after);

    cairo_destroy (cr);
    cairo_pattern_destroy (pattern);

    return (int) (after - before);
}

static const char *filenames[] = {
    "512x512-lenna",
    "512x512-primrose",
    "512x512-redsquare",
    "512x512-transparent"
};

int main( int argc, char **argv )
{
    cairo_surface_t *surface;
    int i;

    surface = output_create_surface (argv [0], width, height);

    for (i = 0; i < sizeof (filenames) / sizeof (const char *); i++) {
        char input_filename [1024];
        char output_filename [1024];
        int j;

        fprintf (stderr, "Testing %s...\n", filenames [i]);
        sprintf (input_filename, "%s.png", filenames [i]);
        sprintf (output_filename, "%s-downsample-nearest-out.png",
                 filenames [i]);

        test (surface, input_filename);
        cairo_surface_write_to_png (surface, output_filename);

        for (j = 0; j < NUM_RUNS; j++) {
            int cur = test (surface, input_filename);
            fprintf (stderr, "\t%d: %d (%.2f ms)\n", j, cur,
                     get_milliseconds (cur));
        }
    }

    cairo_surface_destroy (surface);
    output_cleanup ();
    return 0;
}

