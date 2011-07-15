
//
// PulseTLM.c
//
// This is PulseTLM a more full-featured Transmission Line Matrix
// (TLM) method simulation engine.
//
// Copyright (C) 1999 Paul Hayes
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or any later version, with the following conditions
// attached in addition to any and all conditions of the GNU
//
// General Public License: When reporting or displaying any results or
// animations created using this code or modification of this code,
// make the appropriate citation referencing PulseTLM by name and
// including the version number.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// Contacting the authors:
//
// Paul Hayes
//
// info@cemtach.com
// 
// http://cemtach.com
//

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_LIMITS_H
   #include <limits.h>
#else
   #define FLT_MAX 3.40282347E+38F
   #define PATH_MAX 1024
   #define INT_MAX 2147483647
#endif
#ifdef HAVE_FLOAT_H
   #include <float.h>
#endif
#ifdef HAVE_MATH_H
   #include <math.h>
#endif
#ifdef HAVE_UNISTD_H
   #include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
   #include <sys/types.h>
#endif
#include <sys/stat.h>
#include <sys/errno.h>


#define XY_SLICE 1
#define XZ_SLICE 2
#define YZ_SLICE 3


#include "PulseTLM.h" // for output formats, etc

//
// pull the actual e/h field quantity out of the TLM voxel data in the
// time-domain stream.  We leave the Fourier processing to
// post-processing.
//
// 1, ex
// 2, ey
// 3, ez
// 4, hx
// 5, hy
// 6, hz
// 7, |e|
// 8, |h|
//
double
fieldAcquire(FILE *logFile,
	     TlmVoxel *activeVoxel,
	     TlmMaterial *activeMaterial,
	     double u, double v, double w,
	     unsigned int component)
  {
  //
  // An error string for general otuput of errors and status messages
  char errorString[1024];
  //
  //
  double value;
  double yx, yy, yz, zx, zy, zz, g0;
  double ex, ey, ez, hx, hy, hz, eMagnitude, hMagnitude;
  //
  // Access the stub parameters to make the calculations
  // easier to read and optimize.
  //
  yx = activeMaterial->yx;
  yy = activeMaterial->yy;
  yz = activeMaterial->yz;
  zx = activeMaterial->zx;
  zy = activeMaterial->zy;
  zz = activeMaterial->zz;
  g0 = 0.0;
  //
  // Pick the value according to the user's output
  // request
  //
  switch(component)
    {
    case 1:
      //
      // ex
      ex = 2.0*(activeVoxel->v1 + activeVoxel->v2 +
		activeVoxel->v9 + activeVoxel->v12 +
		yx*activeVoxel->v13) / (u*(4.0+yx));
      value = ex;
      break;
    case 2:
      //
      // ey
      ey = 2.0*(activeVoxel->v3 + activeVoxel->v4 +
		activeVoxel->v8 + activeVoxel->v11 +
		yy*activeVoxel->v14) / (v*(4.0+yy));
      value = ey;
      break;
    case 3:
      //
      // ez
      ez = 2.0*(activeVoxel->v5 + activeVoxel->v6 +
		activeVoxel->v7 + activeVoxel->v10 +
		yz*activeVoxel->v15) / (w*(4.0+yz));
      value = ez;
      break;
    case 4:
      //
      // hx
      hx = -2.0*(activeVoxel->v4 - activeVoxel->v5 +
		 activeVoxel->v7 - activeVoxel->v8 -
		 activeVoxel->v16) / (Z_0*u*(4.0+zx));
      value = hx;
      break;
    case 5:
      //
      // hy
      hy = -2.0*(-activeVoxel->v2 + activeVoxel->v6 +
		 activeVoxel->v9 - activeVoxel->v10 -
		 activeVoxel->v17)/(Z_0*v*(4.0+zy));
      value = hy;
      break;
    case 6:
      //
      // hz
      hz = -2.0*(-activeVoxel->v3 + activeVoxel->v1 +
		 activeVoxel->v11 - activeVoxel->v12 -
		 activeVoxel->v18)/(Z_0*w*(4.0+zz));
      value = hz;
      break;
    case 7:
      //
      // eMagnitude
      ex = 2.0*(activeVoxel->v1 + activeVoxel->v2 +
		activeVoxel->v9 + activeVoxel->v12 +
		yx*activeVoxel->v13) / (u*(4.0+yx));
      ey = 2.0*(activeVoxel->v3 + activeVoxel->v4 +
		activeVoxel->v8 + activeVoxel->v11 +
		yy*activeVoxel->v14) / (v*(4.0+yy));
      ez = 2.0*(activeVoxel->v5 + activeVoxel->v6 +
		activeVoxel->v7 + activeVoxel->v10 +
		yz*activeVoxel->v15) / (w*(4.0+yz));
      //
      value = sqrt(ex*ex + ey*ey + ez*ez);
      break;
    case 8:
      //
      // hMagnitude
      hx = -2.0*(activeVoxel->v4 - activeVoxel->v5 +
		 activeVoxel->v7 - activeVoxel->v8 -
		 activeVoxel->v16) / (Z_0*u*(4.0+zx));
      hy = -2.0*(-activeVoxel->v2 + activeVoxel->v6 +
		 activeVoxel->v9 - activeVoxel->v10 -
		 activeVoxel->v17)/(Z_0*v*(4.0+zy));
      hz = -2.0*(-activeVoxel->v3 + activeVoxel->v1 +
		 activeVoxel->v11 - activeVoxel->v12 -
		 activeVoxel->v18)/(Z_0*w*(4.0+zz));
      //
      value = sqrt(hx*hx + hy*hy + hz*hz);
      break;
    default:
      snprintf(errorString, 1024,
	       "Output field type %d not recognized....", component);
      fprintf(logFile, errorString);
      break;
    }

  return value;
  }










void
userOutput(FILE *logFile,
	   int nx,
	   int ny,
	   int nz,
	   double ds,
	   double dt,
	   double u,
	   double v,
	   double w,
	   int currentIteration,
	   int maximumIteration,
	   double currentSimulationTime,
	   char *inputFileName,
	   TlmVoxel *voxelBase,
	   TlmVoxel ***voxel,
	   Output *outputList)
{
  //
  // An error string for general otuput of errors and status messages
  //
  char errorString[1024];
  //
  // The current output as the list was passed into this function
  //
  Output *output = NULL;
  //
  // Generic array traversal variables
  //
  int i,j,k,n;
  //
  // A brick of bytes file access
  //
  char bobFileName[PATH_MAX];
  FILE *bobFile;
  //
  // A dat or 1D ASCII file access
  //
  char datFileName[PATH_MAX];
  FILE *datFile;
  //
  // A PNM file access
  //
  char pnmFileName[PATH_MAX];
  FILE *pnmFile;
  //
  // mtv file access
  char mtvFileName[PATH_MAX];
  FILE *mtvFile;
  //
  // A plane of float values for output
  //
  float *valueBase;
  //
  // A viz control file.  Note that bob does not have the control file
  // as an option, by outputing data through the viz format, the
  // scaling is retained and BoB can still be used on the individual
  // bob files.
  //
  char vizFileName[PATH_MAX];
  FILE *vizFile;
  //
  // XML file.
  //
  char xmlFileName[PATH_MAX];
  FILE *xmlFile;
  //
  // Structure for accessing the detailed file information.
  //
  struct stat fileStatistics;
  //
  // Scaling variables
  //
  double min, max, norm, drange, scalingValue, value;
  //
  // Generic variables of no great inportance, (input parsing,
  // temporaries, etc)
  //
  double yx, yy, yz, zx, zy, zz, g0;
  double ex, ey, ez, hx, hy, hz, eMagnitude, hMagnitude;
  int iLow, iHigh, jLow, jHigh;
  //
  // Creation of PNM image files variables
  //
  int ir,ib,ig;
  double hh,ss,vv;
  char red, green, blue;

  unsigned int iDelta, jDelta, kDelta;
  unsigned int sliceLogic;
  //
  // The active voxel reference which should aid in compiler
  // optimization
  TlmVoxel *activeVoxel = NULL;
  //
  // The active voxel's material reference, for the same 
  // reason as above....
  TlmMaterial *activeMaterial = NULL;

  //
  // Walk the entire output list creating/updating any output files as
  // necessary.
  //
  output = outputList;
  while(output != NULL)
    {
    //
    // If this output modulo is met or this is the last iteration,
    // then output should occur.
    //
    if ( ((currentIteration % output->modulo) == 0) ||
	 (currentIteration == maximumIteration) )
      {
      //
      // Select based on the requested output type 
      //
      switch(output->type)
	{
	case VIZ:
	  //
	  // Viz/BoB autoscaled 3D output
	  //
	  //
	  // Gain access to the viz control file as append access
	  // since we will be reopening this file again and again for
	  // each modulo requested output.
	  //
	  sprintf(vizFileName, "%s_o%d.viz", inputFileName, output->uniqueNumber);
// 	  //
// 	  // Check for the existence of the file, if it doesn't exist,
// 	  // then create it with the initial information.
// 	  //
// 	  stat(vizFileName, &fileStatistics);
// 	  if (fileStatistics.st_size == 0)
// 	    {
// 	    //
// 	    // If unable to open the file for read, it probably
// 	    // doesn't exist so attempt to create one with the intro
// 	    // information.
// 	    //
// 	    while ((vizFile = fopen(vizFileName, "a")) == NULL)
// 	      {
// 	      //
// 	      // If unable to open the file, exit with a descriptive failure.
// 	      //
// 	      fprintf(stderr, "Difficulty opening %s", vizFileName);
// 	      perror(" ");
// 	      }
// 	    //
// 	    // Place intro information into the viz file
// 	    //
// 	    fprintf(vizFile, "#Viz V1.0\n");
// 	    fprintf(vizFile, "time: %lg %lg\n", 0.0, dt*ds);
// 	    fprintf(vizFile, "color: gbry.cmap\n");
// 	    fprintf(vizFile, "\n");
// 	    //
// 	    // Close it up since we're done with intro informatio
// 	    //
// 	    fclose(vizFile);
// 	    }
	  //
	  // Attempt to close any open vizFile which may have been opened for read
	  //
	  while ((vizFile = fopen(vizFileName, "a")) == NULL)
	    {
	    //
	    // If unable to open the file, warn with a descriptive
	    // failure, however continue to run if possible... just
	    // skip this output...
	    snprintf(errorString, 1024, "Difficulty opening [%s]", vizFileName);
	    fprintf(logFile, errorString);
	    perror(errorString);
	    break;
	    }
	  
	  //
	  // Create the filename for this iteration, which includes
	  // the iteration number.
	  //
	  sprintf(bobFileName, "%s_o%d_%06d.bob",
		  inputFileName, output->uniqueNumber, currentIteration);
	  //
	  // open a new data file for this iteration:
	  //
	  while ((bobFile = fopen(bobFileName, "wb")) == NULL)
	    {
	    //
	    // If unable to open the file, warn with a descriptive
	    // failure, however continue to run if possible... just
	    // skip this output...
	    snprintf(errorString, 1024, "Difficulty opening [%s]", bobFileName);
	    fprintf(logFile, errorString);
	    perror(errorString);
	    break;
	    }
	  //
	  // Locate the min and max values in order to autoscale
	  //
	  min = FLT_MAX;
	  max = -FLT_MAX;
	  //
	  for(i=output->xLow-1; i<output->xHigh; i++)
	    {
	    for(j=output->yLow-1; j<output->yHigh; j++)
	      {
	      for(k=output->zLow-1; k<output->zHigh; k++)
		{
		activeVoxel = &(voxel[i][j][k]);
		activeMaterial = activeVoxel->tlmMaterial;
		//
		//
		value= fieldAcquire(logFile,
				    activeVoxel, activeMaterial,
				    u, v, w,
				    output->field);
		//
		//
		if (value < min)
		  {
		  min = value;
		  }
		if (value > max)
		  {
		  max = value;
		  }
		}
	      }
	    }
	  //
	  // Set norm to be max or min, whichever is greater in magnitude.
	  //
	  norm = (fabs(max) > fabs(min)) ? fabs(max) : fabs(min);
	  if (norm == 0.0)
	    {
	    //
	    // If everything is zero, give norm a tiny value to avoid division by zero.
	    //
	    norm = FLT_EPSILON;
	    }
	  scalingValue = 127.0/norm;
	  //
	  // Write to standard output the minimum and maximum values
	  // from this iteration and the minimum and maximum values
	  // that will be written to the bob file this iteration.
	  //
	  fprintf(logFile, "\t%lg(%d) < BoB < %lg(%d)\n",
		  min, (int)(128.0 + scalingValue*min),
		  max, (int)(128.0 + scalingValue*max));
	  //
	  // Scale each value in the mesh to the range of integers
	  // from zero through 254 and write them to the output file
	  // for this iteration.
	  //
	  for(k=output->zLow-1; k<output->zHigh; k++)
	    {
	    for(j=output->yLow-1; j<output->yHigh; j++)
	      {
	      for(i=output->xLow-1; i<output->xHigh; i++)
		{
		activeVoxel = &(voxel[i][j][k]);
		activeMaterial = activeVoxel->tlmMaterial;
		//
		//
		value= fieldAcquire(logFile,
				    activeVoxel, activeMaterial,
				    u, v, w,
				    output->field);
		//
		// Put the value out to the file
		putc((int)(128.0 + scalingValue*value), bobFile);
		}
	      }
	    }
	  //
	  // Close the output file for this iteration.
	  //
	  fclose(bobFile);
	  //
	  // Write the dimensions and name of the output file for this
	  // iteration to the viz control.
	  //
	  fprintf(vizFile, "%dx%dx%d %s\n",
		  (output->xHigh - output->xLow + 1),
		  (output->yHigh - output->yLow + 1),
		  (output->zHigh - output->zLow + 1),
		  bobFileName);
	  //
	  // Write identification of the corners of the mesh and the max and
	  // min values for this iteration to the viz control file.
	  //
	  fprintf(vizFile, "bbox: 0.0 0.0 0.0 %lg %lg %lg %lg %lg\n",
		  ds*u*(double)nx, ds*v*(double)ny, ds*w*(double)nz, min, max);
	  //
	  // Close off the viz control file for this iteration
	  //
	  fclose(vizFile);
	  break;
	case PNM:
	  //
	  // PNM image format autoscaled 2D output
	  //
	  // Create the filename for this iteration, which includes
	  // the iteration number.
	  //
	  sprintf(pnmFileName, "%s_o%d_%08d.pnm",
		  inputFileName, output->uniqueNumber, currentIteration);
	  //
	  // open a new data file for this iteration:
	  //
	  while ((pnmFile = fopen(pnmFileName, "wb")) == NULL)
	    {
	    //
	    // If unable to open the file, exit with a descriptive failure.
	    //
	    snprintf(errorString, 1024, "Difficulty opening [%s]", pnmFileName);
	    fprintf(logFile, errorString);
	    perror(errorString);
	    break;
	    }
	  //
	  // Pick the plane of output based on the user input line
	  //
	  if (output->zLow == output->zHigh)
	    {
	    iLow = output->xLow-1;
	    iHigh = output->xHigh-1;
	    //
	    jLow = output->yLow-1;
	    jHigh = output->yHigh-1;
	    }
	  if (output->yLow == output->yHigh)
	    {
	    iLow = output->xLow-1;
	    iHigh = output->xHigh-1;
	    //
	    jLow = output->zLow-1;
	    jHigh = output->zHigh-1;
	    }
	  if (output->xLow == output->xHigh)
	    {
	    iLow = output->yLow-1;
	    iHigh = output->yHigh-1;
	    //
	    jLow = output->zLow-1;
	    jHigh = output->zHigh-1;
	    }
	  //
	  // Create a temporary plane of floats
	  //
	  valueBase = (float *)malloc((iHigh-iLow+1)*(jHigh-jLow+1)*sizeof(float));
	  //
	  // Locate the min and max values in order to autoscale
	  //
	  min = FLT_MAX;
	  max = -FLT_MAX;
	  //
	  n = 0;
	  for(i=output->xLow-1; i<output->xHigh; i++)
	    {
	    for(j=output->yLow-1; j<output->yHigh; j++)
	      {
	      for(k=output->zLow-1; k<output->zHigh; k++)
		{
		activeVoxel = &(voxel[i][j][k]);
		activeMaterial = activeVoxel->tlmMaterial;
		//
		//
		value= fieldAcquire(logFile,
				    activeVoxel, activeMaterial,
				    u, v, w,
				    output->field);
		//
		//
		if (value < min)
		  {
		  min = value;
		  }
		if (value > max)
		  {
		  max = value;
		  }
		//
		//
		//
		valueBase[n] = value;
		n++;
		}
	      }
	    }
	  //
	  // Set norm to be max or min, whichever is greater in magnitude.
	  //
	  norm = (fabs(max) > fabs(min)) ? fabs(max) : fabs(min);
	  if (norm == 0.0)
	    {
	    //
	    // If everything is zero, give norm a tiny value to avoid division by zero.
	    //
	    norm = FLT_EPSILON;
	    }
	  //
	  // Write to standard output the minimum and maximum values
	  // from this iteration and the minimum and maximum values
	  // that will be written to the bob file this iteration.
	  //
	  fprintf(logFile, "\t%lg(%d) < PNM < %lg(%d)\n",
		  min, (int)(128.0 + scalingValue*min),
		  max, (int)(128.0 + scalingValue*max));
	  //
	  // Use P3 for ASCII and P6 for binary data in color
	  // Add 10 to the mesh size for color bar, if present.
	  //
	  fprintf(pnmFile,"P6\n");
	  fprintf(pnmFile,"%d %d\n", (iHigh-iLow+1)+10, (jHigh-jLow+1));
	  //
	  // Color 3 byte values each limited to 0->255 in range
	  //
	  fprintf(pnmFile,"255\n");
	  //
	  // Okay, write out the data in character form
	  //
	  n = 0;
	  for(j=jHigh; j>=jLow; j--)
	    {
	    //
	    // Create the color bar segment on this line of the image
	    //
	    value = 2.0*(double)((j-(jHigh - jLow + 1)/2))/(jHigh - jLow + 1);
	    if (value > 1.0) {value = 1.0;}
	    if (value < -1.0) {value = -1.0;}
	    for(i=-10; i<iLow; i++)
	      {
	      //
	      // Locate the appropriate color within the ranged color map
	      // and write the RGB values into the image file.
	      //
	      cmap2(&hh,&ss,&vv,value);
	      hsvrgb(hh,ss,vv,&ir,&ig,&ib);
	      red = ir; green = ig; blue = ib;
	      fwrite(&red, sizeof(char), 1, pnmFile);
	      fwrite(&green, sizeof(char), 1, pnmFile);
	      fwrite(&blue, sizeof(char), 1, pnmFile);
	      }
	    //
	    // Cycle through the rest of the data
	    //
	    for(i=iLow; i<=iHigh; i++)
	      {
	      value = valueBase[n];
	      n++;
	      //
	      // Locate the appropriate color within the ranged color map
	      // and write the RGB values into the image file.
	      //
	      cmap2(&hh,&ss,&vv,value/norm);
	      //
	      // Convert from HSV color map into RGB values for PNM format
	      //
	      hsvrgb(hh,ss,vv,&ir,&ig,&ib);
	      red = ir; green = ig; blue = ib;
	      fwrite(&red, sizeof(char), 1, pnmFile);
	      fwrite(&green, sizeof(char), 1, pnmFile);
	      fwrite(&blue, sizeof(char), 1, pnmFile);
	      }
	    }
	  //
	  // Close off the PNM file
	  //
	  fclose(pnmFile);
	  break;
	case MTV:
	  //
	  // MTV (plotMTV) data format 2D output
	  //
	  sprintf(mtvFileName, "%s_o%d_%06d.mtv",
		  inputFileName, output->uniqueNumber, currentIteration);
	  while ((mtvFile = fopen(mtvFileName, "a")) == NULL)
	    {
	    //
	    // If unable to open the file, warn with a descriptive
	    // failure, however continue to run if possible... just
	    // skip this output...
	    snprintf(errorString, 1024, "Difficulty opening [%s]", datFileName);
	    fprintf(logFile, errorString);
	    perror(errorString);
	    break;
	    }
	  //
	  //
	  iDelta= (output->xHigh - output->xLow)+1;
	  jDelta= (output->yHigh - output->yLow)+1;
	  kDelta= (output->zHigh - output->zLow)+1;

	  //
	  // Locate the min and max values in order to autoscale
	  min = FLT_MAX;
	  max = -FLT_MAX;
	  //
	  for(i=output->xLow-1; i<output->xHigh; i++)
	    {
	    for(j=output->yLow-1; j<output->yHigh; j++)
	      {
	      for(k=output->zLow-1; k<output->zHigh; k++)
		{
		activeVoxel = &(voxel[i][j][k]);
		activeMaterial = activeVoxel->tlmMaterial;
		//
		//
		value= fieldAcquire(logFile,
				    activeVoxel, activeMaterial,
				    u, v, w,
				    output->field);
		//
		//
		if (value < min)
		  {
		  min = value;
		  }
		if (value > max)
		  {
		  max = value;
		  }
		}
	      }
	    }
	  //
	  // Output header information describing this slice of data
	  fprintf(mtvFile, "$ DATA=CONTOUR\n");
	  fprintf(mtvFile, "%% \n");
	  fprintf(mtvFile, "%% nsteps=50\n");
	  fprintf(mtvFile, "%% fitpage=true\n");
	  fprintf(mtvFile, "%% contstyle=2\n");
	  fprintf(mtvFile, "%% cmin = %g\n", min);
	  fprintf(mtvFile, "%% cmax = %g\n", max);
	  fprintf(mtvFile, "%% contclip=true\n");
	  fprintf(mtvFile, "%% grid = on\n");
	  fprintf(mtvFile, "#%% xmin = 0.0  xmax = 100.0\n");
	  fprintf(mtvFile, "#%% ymin = 0.0  ymax = 200.0\n");
	  fprintf(mtvFile, "#%% xticks = 20\n");
	  fprintf(mtvFile, "#%% yticks = 30\n");
	  fprintf(mtvFile, "#%% vxmin = 0.0  vxmax = 49.0\n");
	  fprintf(mtvFile, "#%% vymin = 0.0  vymax = 153.0\n");
	  //
	  //
	  if ( (iDelta > kDelta) && (jDelta > kDelta) )
	    {
	    sliceLogic= XY_SLICE;
	    //
	    fprintf(mtvFile, "%% toplabel= \"XY 2D slice\"\n");
	    fprintf(mtvFile, "%% xlabel = \"xAxis\"\n");
	    fprintf(mtvFile, "%% ylabel = \"yAxis\"\n");
 	    fprintf(mtvFile, "%% nx = %u\n", iDelta);
 	    fprintf(mtvFile, "%% ny = %u\n", jDelta);
	    }
	  if ( (iDelta > jDelta) && (kDelta > jDelta) )
	    {
	    sliceLogic= XZ_SLICE;
	    //
	    fprintf(mtvFile, "%% toplabel= \"XZ 2D slice\"\n");
	    fprintf(mtvFile, "%% xlabel = \"xAxis\"\n");
	    fprintf(mtvFile, "%% ylabel = \"zAxis\"\n");
 	    fprintf(mtvFile, "%% nx = %u\n", iDelta);
 	    fprintf(mtvFile, "%% ny = %u\n", kDelta);
	    //
	    }
	  if ( (jDelta > iDelta) && (kDelta > iDelta) )
	    {
	    sliceLogic= YZ_SLICE;
	    //
	    fprintf(mtvFile, "%% toplabel= \"YZ 2D slice\"\n");
	    fprintf(mtvFile, "%% xlabel = \"yAxis\"\n");
	    fprintf(mtvFile, "%% ylabel = \"zAxis\"\n");
 	    fprintf(mtvFile, "%% nx = %u\n", jDelta);
 	    fprintf(mtvFile, "%% ny = %u\n", kDelta);
	    //
	    }

	  //
	  //
	  for(i=output->xLow-1; i<output->xHigh; i++)
	    {
	    for(j=output->yLow-1; j<output->yHigh; j++)
	      {
	      for(k=output->zLow-1; k<output->zHigh; k++)
		{
		activeVoxel = &(voxel[i][j][k]);
		activeMaterial = activeVoxel->tlmMaterial;
		//
		//
		value= fieldAcquire(logFile,
				    activeVoxel, activeMaterial,
				    u, v, w,
				    output->field);
		//
		fprintf(mtvFile, "%g\n", value);
		}
	      }
	    }
	  //
	  // Close the output file for this iteration.
	  fclose(mtvFile);


	  break;
	case GRACE:
	  //
	  // grace (xmgr) data format 1D output
	  //
	  break;
	case DAT:
	  //
	  // dat data format or straight 1D ASCII file
	  sprintf(datFileName, "%s_o%d.dat", inputFileName, output->uniqueNumber);
	  while ((datFile = fopen(datFileName, "a")) == NULL)
	    {
	    //
	    // If unable to open the file, warn with a descriptive
	    // failure, however continue to run if possible... just
	    // skip this output...
	    snprintf(errorString, 1024, "Difficulty opening [%s]", datFileName);
	    fprintf(logFile, errorString);
	    perror(errorString);
	    break;
	    }
	  //
	  // Set the i,j,k values into the mesh
	  i = output->xLow-1;
	  j = output->yLow-1;
	  k = output->zLow-1;
	  activeVoxel = &(voxel[i][j][k]);
	  activeMaterial = activeVoxel->tlmMaterial;
	  //
	  //
	  value= fieldAcquire(logFile,
			      activeVoxel, activeMaterial,
			      u, v, w,
			      output->field);
	  //
	  // Dump the data into the file
	  //
	  fprintf(datFile, "%lg %lg\n", currentSimulationTime, value);
	  //
	  // Close off the file for the next time around
	  //
	  fclose(datFile);
	  break;
	case BINARY:
	  //
	  // binary dump 1-2-3D dump of ex, ey, ez, hx, hy, hz
	  //
	  break;
	case VTU:
	  //
	  // VTK/ParaView/ViSiT styled format, initial incarnation
	  // StructuredGrid (.vts) — Serial vtkStructuredGrid (structured).
	  //
	  sprintf(xmlFileName, "%s_o%d_%06d.vtr",
		  inputFileName, output->uniqueNumber, currentIteration);
	  while ((xmlFile = fopen(xmlFileName, "a")) == NULL)
	    {
	    //
	    // If unable to open the file, warn with a descriptive
	    // failure, however continue to run if possible... just
	    // skip this output...
	    snprintf(errorString, 1024, "Difficulty opening [%s]", xmlFileName);
	    fprintf(logFile, errorString);
	    perror(errorString);
	    break;
	    }
	  //
	  // Proceed with VTK styled XML output.  Note since we don't
	  // have "true geometry" at this stage, we can only output
	  // the "relative geometry".  Should the user require it, the
	  // XML file could be edited later.  byte_order will be
	  // essentially irrelevant here since this output will not be
	  // binary.
          fprintf(xmlFile, "<VTKFile type=\"RectilinearGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n");
          fprintf(xmlFile,
		  "<RectilinearGrid WholeExtent=\"1 %d 1 %d 1 %d\">\n",
		  nx, ny, nz);
          fprintf(xmlFile,
		  "<Piece Extent=\"%d %d %d %d %d %d\">\n",
		  output->xLow, output->xHigh,
		  output->yLow, output->yHigh,
		  output->zLow, output->zHigh);
	  //
	  //
          fprintf(xmlFile, "<PointData>\n");
          fprintf(xmlFile, "        <DataArray type=\"Float32\" ");
	  fprintf(xmlFile, "Name=\"");
	  switch(output->field)
	    {
	    case EX:
	      fprintf(xmlFile, "ex");
	      break;
	    case EY:
	      fprintf(xmlFile, "ey");
	      break;
	    case EZ:
	      fprintf(xmlFile, "ez");
	      break;
	    case HX:
	      fprintf(xmlFile, "hx");
	      break;
	    case HY:
	      fprintf(xmlFile, "hy");
	      break;
	    case HZ:
	      fprintf(xmlFile, "hz");
	      break;
	    case E_MAGNITUDE:
	      fprintf(xmlFile, "eMag");
	      break;
 	    case H_MAGNITUDE:
	      fprintf(xmlFile, "hMag");
	      break;
	    }
	  fprintf(xmlFile, "\" format=\"ascii\">\n");
	  //
	  for(k=output->zLow-1; k<output->zHigh; k++)
	    {
	    for(j=output->yLow-1; j<output->yHigh; j++)
	      {
	      for(i=output->xLow-1; i<output->xHigh; i++)
		{
		activeVoxel = &(voxel[i][j][k]);
		activeMaterial = activeVoxel->tlmMaterial;
		//
		//
		value= fieldAcquire(logFile,
				    activeVoxel, activeMaterial,
				    u, v, w,
				    output->field);
		fprintf(xmlFile, "%g\n", value);

		}
	      }
	    }

          fprintf(xmlFile, "        </DataArray>\n");

          fprintf(xmlFile, "</PointData>\n");
	  //
//           fprintf(xmlFile, "<CellData>...</CellData>\n");
//           fprintf(xmlFile, "<Points>...</Points>\n");
          fprintf(xmlFile, " <Coordinates>\n");
          fprintf(xmlFile, "<DataArray type=\"Float32\" name=\"x\">\n");
	  for(i=output->xLow-1; i<output->xHigh; i++)
	    {
	    fprintf(xmlFile, "%d ", i+1);
	    }
	  fprintf(xmlFile, "\n</DataArray>\n");
	  //
	  //
	  fprintf(xmlFile, "<DataArray type=\"Float32\" name=\"y\">\n");
	  for(j=output->yLow-1; j<output->yHigh; j++)
	    {
	    fprintf(xmlFile, "%d ", j+1);
	    }
	  fprintf(xmlFile, "\n  </DataArray>\n");
	  //
	  //
	  fprintf(xmlFile, "<DataArray type=\"Float32\" name=\"z\">\n");
	  for(k=output->zLow-1; k<output->zHigh; k++)
	    {
	    fprintf(xmlFile, "%d ", k+1);
	    }
	  fprintf(xmlFile, "\n  </DataArray>\n");
	  //
	  //
	  fprintf(xmlFile, "</Coordinates>\n");
	  //
	  //
	  fprintf(xmlFile, "</Piece>\n");
          fprintf(xmlFile, "</RectilinearGrid>\n");




          fprintf(xmlFile, "</VTKFile>\n");

          fprintf(xmlFile, "\n");
          fprintf(xmlFile, "\n");
          fprintf(xmlFile, "\n");
          fprintf(xmlFile, "\n");
          fprintf(xmlFile, "\n");
	  //
	  // Close the output file for this iteration.
	  fclose(xmlFile);
	  break;
	case XML:
	  //
	  // XML data format 
	  sprintf(xmlFileName, "%s_o%d.xml", inputFileName, output->uniqueNumber);
	  while ((xmlFile = fopen(xmlFileName, "a")) == NULL)
	    {
	    //
	    // If unable to open the file, warn with a descriptive
	    // failure, however continue to run if possible... just
	    // skip this output...
	    snprintf(errorString, 1024, "Difficulty opening [%s]", xmlFileName);
	    fprintf(logFile, errorString);
	    perror(errorString);
	    break;
	    }
	  //
	  //
	  //
          fprintf(xmlFile, "  <t value=\"%d\">\n", currentIteration);
	  for(i=output->xLow-1; i<output->xHigh; i++)
	    {
            fprintf(xmlFile, "    <x value=\"%d\">\n", i);

	    for(j=output->yLow-1; j<output->yHigh; j++)
	      {
              fprintf(xmlFile, "      <y value=\"%d\">\n", j);
	      for(k=output->zLow-1; k<output->zHigh; k++)
		{
                fprintf(xmlFile, "        <z value=\"%d\">\n", k);

		activeVoxel = &(voxel[i][j][k]);
		activeMaterial = activeVoxel->tlmMaterial;
		//
		//
		value= fieldAcquire(logFile,
				    activeVoxel, activeMaterial,
				    u, v, w,
				    output->field);
		//
		fprintf(xmlFile, "          <field>%g</field>\n", value);
                fprintf(xmlFile, "        </z>\n");
		}
              fprintf(xmlFile, "      </y>\n");
	      }
            fprintf(xmlFile, "    </x>\n");
            fprintf(xmlFile, "  </t>\n");
            if (currentIteration == maximumIteration)
              {
              fprintf(xmlFile, "</pulsetlm>");
              }
	    }
	  //
	  // Close the output file for this iteration.
	  fclose(xmlFile);
          break;
	default:
	  snprintf(errorString, 1024, "Output type %d not recognized....", output->type);
	  fprintf(logFile, errorString);
	  break;
	}
      }
    //
    // Cycle through all the outputs
    //
    output = output->next;
    }
}
       
