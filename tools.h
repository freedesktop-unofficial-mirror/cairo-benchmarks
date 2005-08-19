
#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

#include <cairo.h>

double get_milliseconds( int cycles );
cairo_surface_t *output_create_surface (const char *appname, int width,
                                        int height);
void output_cleanup (void);

#endif /* TOOLS_H_INCLUDED */
