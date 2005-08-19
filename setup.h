
#ifndef SETUP_H_INCLUDED
#define SETUP_H_INCLUDED

/**
 * Number of times to go for.  Set this nice and high if you want to get
 * a good oprofile sample.
 */
#define NUM_RUNS 256

/**
 * Set this to 1 to use realtime priority.  Dangerous!  This will hang your
 * machine for the duration of the test!  Infinite loop and you must reboot!
 */
#define USE_REALTIME_PRIORITY 0

#endif /* SETUP_H_INCLUDED */
