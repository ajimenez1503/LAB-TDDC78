
LFLAGS= -lpthread -lrt -lm

thresc: thresmain.c ppmio.c thresfilter.c
	cc  thresmain.c ppmio.c thresfilter.c $(LFLAGS) -o $@


clean:
	-$(RM) *.o thresc
