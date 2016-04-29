all: serialLaplsolv parallelLaplsolv

serialLaplsolv:serialLaplsolv.f90
	gfortran  $^ -o $@

parallelLaplsolv:parallelLaplsolv.f90
	gfortran -fopenmp $^ -o $@
clean:
	rm parallelLaplsolv serialLaplsolv
