program serialLaplsolv
!-----------------------------------------------------------------------
! Serial program for solving the heat conduction problem 
! on a square using the Jacobi method. 
! Written by Fredrik Berntsson (frber@math.liu.se) March 2003
! Modified by Berkant Savas (besav@math.liu.se) April 2006
!-----------------------------------------------------------------------
  integer, parameter                  :: n=1000, maxiter=1000
  double precision,parameter          :: tol=1.0E-3
  double precision,dimension(0:n+1,0:n+1) :: T
  double precision,dimension(n)       :: tmp1,tmp2
  double precision                    :: error
  real                                :: t1,t0
  integer                             :: j,k
  
  ! Set boundary conditions and initial values for the unknowns
  T=0.0D0
  T(0:n+1 , 0)     = 1.0D0
  T(0:n+1 , n+1)   = 1.0D0
  T(n+1   , 0:n+1) = 2.0D0
  

  ! Solve the linear system of equations using the Jacobi method
  t0=omp_get_wtime()
  
  do k=1,maxiter
     
     tmp1=T(1:n,0)
     error=0.0D0
     
     do j=1,n
        tmp2=T(1:n,j)
        T(1:n,j)=(T(0:n-1,j)+T(2:n+1,j)+T(1:n,j+1)+tmp1)/4.0D0
        error=max(error,maxval(abs(tmp2-T(1:n,j))))
        tmp1=tmp2
     end do
     
     if (error<tol) then
        exit
     end if
     
  end do
  
  t1=omp_get_wtime()

  write(unit=*,fmt=*) 'Time:',t1-t0,'Number of Iterations:',k
  write(unit=*,fmt=*) 'Temperature of element T(1,1)  =',T(1,1)

 
end program serialLaplsolv
