serialLaplsolv:serialLaplsolv.f90
	gfortran -fopenmp $^ -o $@

parallelLaplsolv:parallelLaplsolv.f90
	gfortran -fopenmp $^ -o $@
clean:
	rm parallelLaplsolv serialLaplsolv
