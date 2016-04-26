
LFLAGS= -lpthread -lrt -lm

blurc: ppmio.c gaussw.c blurfilter.c blurmain.c
	cc  ppmio.c gaussw.c blurfilter.c blurmain.c $(LFLAGS) -o $@

clean:
	-$(RM) *.o blurc
