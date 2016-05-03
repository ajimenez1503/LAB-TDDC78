
LFLAGS= -lpthread -lrt -lm

simulator: main.c physics.c matrix.c
	mpicc  main.c physics.c matrix.c  $(LFLAGS) -o $@

clean:
	-$(RM) *.o simulator
