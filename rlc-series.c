#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <math.h>

int func(double t, const double y[], double f[],
	 void *params){
  (void)(t); /* avoid unused parameter warning */
  double *mu = params;
  f[0] = (mu[3] - mu[0]*y[0] -y[1])/mu[1];
  f[1] = y[0]/mu[2];
  return GSL_SUCCESS;
}

int jac(double t, const double y[], double *dfdy,
     double dfdt[], void *params){

  (void)(t); /* avoid unused parameter warning */
  double *mu = params;
  gsl_matrix_view dfdy_mat
    = gsl_matrix_view_array (dfdy, 2, 2);
  gsl_matrix * m = &dfdy_mat.matrix;
  gsl_matrix_set(m, 0, 0,  -mu[0]/mu[1]);
  gsl_matrix_set(m, 0, 1,    -1.0/mu[1]);
  gsl_matrix_set(m, 1, 0,     1.0/mu[2]);
  gsl_matrix_set(m, 1, 1,           0.0);
  dfdt[0] = 0.0;
  dfdt[1] = 0.0;
  return GSL_SUCCESS;
}

int main (void) {  //  mu= {R,   L,   C,  E}
  double mu[4] ={500.0, 47.0/1000, 47.0/1000000000, 0.0};
  gsl_odeiv2_system sys ={func, jac, 2, &mu};
  gsl_odeiv2_driver *d =
    gsl_odeiv2_driver_alloc_y_new(&sys,
				  gsl_odeiv2_step_rk8pd,
                                  1e-6, 1e-6, 0.0);
  int i;
  double t = 0.0, t1 = 1.0;
  double y[2] = { 0.0, 100.0 }; // Vc(0) = 100 V

  for (i = 1; i <= 10000; i++) {
      double ti = i * t1 / 10000000.0;
      int status = gsl_odeiv2_driver_apply(d, &t, ti, y);

      if (status != GSL_SUCCESS) {
          printf ("error, return value=%d\n", status);
          break;
      }
      if(i%100==0){              // t, i, V
	printf ("%.5e %.5e %.5e\n", t, y[0], y[1]);
      }
  }

  gsl_odeiv2_driver_free (d);
  return 0;
}
