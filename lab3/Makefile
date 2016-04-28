all: serialLaplsolv parallelLaplsolv

serialLaplsolv:serialLaplsolv.f90
	gfortran -fopenmp $^ -o $@

parallelLaplsolv:parallelLaplsolv.f90
	gfortran -fopenmp $^ -o $@
	export OMP_NUM_THREADS=4
clean:
	rm parallelLaplsolv serialLaplsolv
