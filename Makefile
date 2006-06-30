
BENCHMARKS=				\
	over				\
	over-xlib			\
	over-clipped			\
	over-clipped-xlib		\
	add				\
	add-xlib			\
	upsample-bilinear		\
	upsample-bilinear-xlib		\
	upsample-nearest		\
	upsample-nearest-xlib		\
	downsample-bilinear		\
	downsample-bilinear-xlib	\
	downsample-nearest		\
	downsample-nearest-xlib		\
	lines				\
	lines-xlib			\
	curves				\
	curves-xlib			\
	gradients-linear		\
	gradients-linear-xlib		\
	multiple-clip-rectangles	\
	multiple-clip-rectangles-xlib	\
	solidtext			\
	solidtext-xlib			\
	textpath			\
	textpath-xlib			\
	texturedtext			\
	texturedtext-xlib		\
	remenic-gradient

MYCFLAGS=-Wall `pkg-config --cflags cairo libpng12` -I/usr/X11R6/include
MYLDFLAGS=`pkg-config --libs cairo libpng12` -L/usr/X11R6/lib

MYOBJS=tools.o

all: $(MYOBJS) $(BENCHMARKS)

memcpy-helper: memcpy-helper.c
	$(CC) -Wall -fPIC -shared -o memcpy-helper.so memcpy-helper.c -ldl

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) ${MYCFLAGS} $< -o $@

%-xlib: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) ${MYCFLAGS} ${MYLDFLAGS} ${MYOBJS} $^ -o $@

%: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) ${MYCFLAGS} ${MYLDFLAGS} ${MYOBJS} $^ -o $@

clean:
	rm -f ${BENCHMARKS} *.o *.so *-out.png
