//
// This code snippet hails from John B. Schneider, Patrick J. Flynn, and Kurt L. Shlager
// and http://www.eecs.wsu.edu/~schneidj/Animations/
//
// It was originally in FORTRAN yet was straightforward to convert into C.  The routine
// converts a floating-point value in xpnt into a color-mapped set of hue, saturation and
// value quantities which are returned.  The color-apping is a `nice' two-sided linear
// scale with zero being black and directly in the center of values.
//
// Note that xpnt must scale between -1.0 and 1.0 for the routines to
// work properly.  If your xpnt value scales greater than that, you
// may see awkward color map values....
//
#include <math.h>

void
cmap2(double *h,
      double *s,
      double *v,
      double xpnt)
  {
  static double c1 = 30.0/0.04;
  static double c2 = 1.0/0.2;
  static double c3 = 105.0/0.5;
  static double c4 = 45.0/0.3;
  static double c5 = 1.0/0.3;
  static double c6 = 135.0/0.7;

  if (xpnt > 0.8)
    {
    // 0.8 to +infty
    *h = 30.0 - c1*(xpnt-0.8)*(xpnt-0.8);
    *s = c2*(1.0-xpnt);
    *v = 1.0;
    }
  else if (xpnt > 0.3)
    {
    // 0.3 to 0.8
    *h = (135.0 - c3*(xpnt-0.3));
    *s = 1.0;
    *v = 1.0;
    }
  else if (xpnt > 0.0)
    {
    // 0.0 to 0.3
    *h = (180.0 - c4*xpnt);
    *s = 1.0;
    *v = xpnt*c5;
    }
  else if (xpnt > -0.3)
    {
    // -0.3 to 0.0
    *h = (180.0 - c4*xpnt);
    *s = 1.0;
    *v = fabs(xpnt)*c5;
    }
  else
    {
    // -infty to -0.3
    *h = (225.0 - c6*(xpnt+0.3));
    *s = 1.0;
    *v = 1.0;
    }
  return;
  }
