program ParallelLaplsolv
use omp_lib
!-----------------------------------------------------------------------
! Parallel program for solving the heat conduction problem 
! on a square using the Jacobi method. 
!-----------------------------------------------------------------------
!PROBLEM: n must be divisible between number of thread.
	integer, parameter									:: number_thread=5
  integer, parameter                  :: n=1000, maxiter=1000 ! constant never change
  double precision,parameter          :: tol=1.0E-3
  double precision,dimension(0:n+1,0:n+1) :: T !matrix (n+1)x(n+1)
  double precision,dimension(n)       :: tmp1,tmp2,tmp3 !array of size n
  double precision                    :: error
  double precision                    :: t1,t0
  integer                             :: i,j,k
	integer															:: chuck
	double precision,dimension(1:n,0:2*number_thread) :: helper !matrix (n)x(2*number_thread). 2 per thread 
  

	CALL OMP_SET_NUM_THREADS(number_thread)
  if(number_thread/=omp_get_max_threads()) then
     write(unit=*,fmt=*) 'Number of thread must be =',number_thread,' EXIT'
     call EXIT(0)
  end if
  ! Set boundary conditions and initial values for the unknowns
  T=0.0D0	!all the matrix equal 0
  T(0:n+1 , 0)     = 1.0D0 ! first column equal 1
  T(0:n+1 , n+1)   = 1.0D0 ! last column equal 1
  T(n+1   , 0:n+1) = 2.0D0 ! last row equal 2
  
	 
	chuck=n/number_thread !calculate chuck
	write(unit=*,fmt=*) 'Number of thread=',number_thread,', chuck=',chuck    

	
	i=0 
	do j=0,number_thread -1 ! from from 0 to 2*number_thread
		!write(unit=*,fmt=*) 'jjjjjjjjjjjj ',j   
    helper(1:n,i)=T(1:n,j*chuck) 
		!write(unit=*,fmt=*) 'i=',i,'j=',j*chuck
		i=i+1 
		helper(1:n,i)=T(1:n,(j+1)*chuck+1)  
		!write(unit=*,fmt=*) 'i=',i,'j=',(j+1)*chuck+1
		i=i+1      
  end do


  ! Solve the linear system of equations using the Jacobi method
  t0=omp_get_wtime()
  
  do k=1,maxiter
    error=0.0D0 !do reduction and take the max.
  
    i=0 
	  do j=0,number_thread -1 ! from from 0 to 2*number_thread
		  !write(unit=*,fmt=*) 'jjjjjjjjjjjj ',j   
      helper(1:n,i)=T(1:n,j*chuck) 
		  !write(unit=*,fmt=*) 'i=',i,'j=',j*chuck
		  i=i+1 
		  helper(1:n,i)=T(1:n,(j+1)*chuck+1)  
		  !write(unit=*,fmt=*) 'i=',i,'j=',(j+1)*chuck+1
		  i=i+1      
    end do
     

    !$OMP PARALLEL PRIVATE(j,tmp1,tmp2,tmp3) SHARED (chuck,T,helper)
    !$omp do schedule ( STATIC, chuck ) reduction(MAX:error) 
      do j=1,n
			  if(mod(j-1,chuck)==0) then
				  tmp1=helper(1:n,(j/chuck)+OMP_GET_THREAD_NUM())
				  !write(unit=*,fmt=*) 'j=',j,"i=",(j/chuck)+OMP_GET_THREAD_NUM()
          tmp2=T(1:n,j)
          T(1:n,j)=(T(0:n-1,j)+T(2:n+1,j)+T(1:n,j+1)+tmp1)/4.0D0
			  else if (mod(j,chuck)==0) then
				  tmp3=helper(1:n,((j+1)/chuck)+OMP_GET_THREAD_NUM())
				  !write(unit=*,fmt=*) 'j=',j ,"i=",((j+1)/chuck)+OMP_GET_THREAD_NUM()
          tmp2=T(1:n,j)
          T(1:n,j)=(T(0:n-1,j)+T(2:n+1,j)+tmp3+tmp1)/4.0D0
        else
          tmp2=T(1:n,j)
          T(1:n,j)=(T(0:n-1,j)+T(2:n+1,j)+T(1:n,j+1)+tmp1)/4.0D0
			  end if
          error=max(error,maxval(abs(tmp2-T(1:n,j))))
          tmp1=tmp2
      end do
    !$omp end do
    !$OMP END PARALLEL
     
     if (error<tol) then
        exit
     end if
     
  end do
 
  t1=omp_get_wtime()

  write(unit=*,fmt=*) 'Time:',t1-t0,'Number of Iterations:',k
  write(unit=*,fmt=*) 'Temperature of element T(1,1)  =',T(1,1)

end program ParallelLaplsolv
