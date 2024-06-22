#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <math.h>
#define RR 500.0
#define LL (47.0/1000)
#define CC (47.0/1000000000)
#define EE 0.0

int func(double t, const double y[], double f[],
	 void *params){
  (void)(t); /* avoid unused parameter warning */
  double mu = *(double *)params;
  f[0] = (EE - RR*y[0] -y[1])/LL;
  f[1] = y[0]/CC;
  return GSL_SUCCESS;
}

int jac(double t, const double y[], double *dfdy,
     double dfdt[], void *params){

  (void)(t); /* avoid unused parameter warning */
  double mu = *(double *)params;
  gsl_matrix_view dfdy_mat
    = gsl_matrix_view_array (dfdy, 2, 2);
  gsl_matrix * m = &dfdy_mat.matrix;
  gsl_matrix_set(m, 0, 0,  1.0/CC);
  gsl_matrix_set(m, 0, 1,     0.0);
  gsl_matrix_set(m, 1, 0,  -RR/LL);
  gsl_matrix_set(m, 1, 1, -1.0/LL);
  dfdt[0] = 0.0;
  dfdt[1] = 0.0;
  return GSL_SUCCESS;
}

int main (void) {
  double mu = 0.0;
  gsl_odeiv2_system sys ={func, jac, 2, &mu};
  gsl_odeiv2_driver *d =
    gsl_odeiv2_driver_alloc_y_new(&sys,
				  gsl_odeiv2_step_rk8pd,
                                  1e-6, 1e-6, 0.0);
  int i;
  double t = 0.0, t1 = 1.0;
  double y[2] = { 0.0, 10.0 };

  for (i = 1; i <= 10000; i++) {
      double ti = i * t1 / 10000000.0;
      int status = gsl_odeiv2_driver_apply(d, &t, ti, y);

      if (status != GSL_SUCCESS) {
          printf ("error, return value=%d\n", status);
          break;
      }
      if(i%100==0){
	printf ("%.5e %.5e %.5e\n", t, y[0], y[1]);
      }
  }

  gsl_odeiv2_driver_free (d);
  return 0;
}
