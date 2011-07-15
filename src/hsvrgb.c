//
// This code snippet hails from John B. Schneider, Patrick J. Flynn, and Kurt L. Shlager
// and http://www.eecs.wsu.edu/~schneidj/Animations/
//
// It was originally in FORTRAN yet was straightforward to convert into C. The routine
// converts the hsv values created in cmap2 into rgb values for use in a PNM file
// format.
//
//
// input: 
// 0 <= h <= 360    (hue)
// 0 <= v <= 1.0    (brightness)
// 0 <= s <= 1.0    (saturation)
//
// output: 0 <= ir,ig,ib <= 255
//
#include <math.h>

void
hsvrgb(double h,
       double s,
       double v,
       int *ir,
       int *ig,
       int *ib)

  {
  double pqvt[4], f;
  static int irpnt[6]={2,1,0,0,3,2}, igpnt[6]={3,2,2,1,0,0}, ibpnt[6]={0,0,3,2,2,1};
  static double c1 = 1.0/60.0;
  int i;
  //
  if (s == 0.0)
    {
    *ir = 255.0*v;
    *ig = *ir;
    *ib = *ir;
    return;
    }
  if (h == 360.0) h=0.0;
  h = h*c1;
  i = floor(h);
  f = h-i;
  pqvt[2] = 255.0*v;
  pqvt[0] = pqvt[2]*(1.0-s);
  pqvt[1] = pqvt[2]*(1.0-s*f);
  pqvt[3] = pqvt[2]*(1.0-s*(1.0-f));
  *ir = pqvt[irpnt[i]];
  *ig = pqvt[igpnt[i]];
  *ib = pqvt[ibpnt[i]];
  return;
  }
