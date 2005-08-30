
#include <stdio.h>
#include <stdint.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <cairo.h>
#include <cairo-xlib-xrender.h>
#include "tools.h"

/**
 * Set this to 1 to use realtime priority.  Dangerous!  This will hang your
 * machine for the duration of the test!  Infinite loop and you must reboot!
 */
#define USE_REALTIME_PRIORITY 0

#define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))

static int mhz_calculated = 0;
double mhz;

static double cpuinfo_get_speed( void )
{
    uint64_t tsc_start, tsc_end;
    struct timeval tv_start, tv_end;
    int usec_delay;

    rdtscll( tsc_start );
    gettimeofday( &tv_start, 0 );
    usleep( 100000 );
    rdtscll( tsc_end );
    gettimeofday( &tv_end, 0 );

    usec_delay = 1000000 * (tv_end.tv_sec - tv_start.tv_sec) +
                           (tv_end.tv_usec - tv_start.tv_usec);

    return (((double) (tsc_end - tsc_start)) / ((double) usec_delay));
}

int set_realtime_priority( int max )
{   
    struct sched_param schp;

    memset( &schp, 0, sizeof( schp ) );
    if( max ) schp.sched_priority = sched_get_priority_max( SCHED_FIFO );
    else schp.sched_priority = sched_get_priority_max( SCHED_FIFO ) - 1;

    if( sched_setscheduler( 0, SCHED_FIFO, &schp ) != 0 ) {
        return 0;
    }
    
    return 1; 
}

double get_milliseconds( int cycles )
{
    if( !mhz_calculated ) {
        mhz = cpuinfo_get_speed();
        mhz_calculated = 1;
    }
    return ((double) cycles) / (mhz * 1000.0);
}

static Display *dpy;
static Pixmap pixmap;
static int xlib = 0;

static cairo_surface_t *create_xlib_surface (int width, int height)
{
    cairo_surface_t *surface;
    XRenderPictFormat *xrender_format;

    dpy = XOpenDisplay (0);

    xrender_format = XRenderFindStandardFormat (dpy, PictStandardARGB32);
    
    pixmap = XCreatePixmap (dpy, DefaultRootWindow (dpy),
                            width, height, xrender_format->depth);

    surface = cairo_xlib_surface_create_with_xrender_format (dpy, pixmap,
                 DefaultScreenOfDisplay (dpy), xrender_format, width, height);
    return surface;
}

static void cleanup_xlib (void)
{
    XFreePixmap (dpy, pixmap);
    XCloseDisplay (dpy);
}

static uint8_t *data = 0;

cairo_surface_t *output_create_surface (const char *appname, int width,
                                        int height)
{
    if (USE_REALTIME_PRIORITY) {
        if (!set_realtime_priority (0)) {
            fprintf (stderr, "Cannot set realtime priority (need root)\n");
        }
    }

    if (strstr (appname, "xlib")) {
        return create_xlib_surface (width, height);
    } else {
        data = malloc (width * height * 4);
        return cairo_image_surface_create_for_data (data, CAIRO_FORMAT_ARGB32,
                                                    width, height, width * 4);
    }
}

void output_cleanup (void)
{
    if (xlib) {
        cleanup_xlib ();
    } else {
        free (data);
    }
}


