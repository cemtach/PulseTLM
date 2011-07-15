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
#include <string.h>
#ifdef HAVE_UNISTD_H
   #include <unistd.h>
#endif
#ifdef HAVE_LIMITS_H
   #include <limits.h>
#else
   #define FLT_MAX 3.40282347E+38F
   #define PATH_MAX 1024
   #define INT_MAX 2147483647
#endif
#ifdef HAVE_MATH_H
   #include <math.h>
#endif
#ifdef HAVE_FLOAT_H
   #include <float.h>
#endif

#ifdef HAVE_SYS_TIMES_H
   #include <sys/times.h>
#endif
#ifdef HAVE_SYS_TYPES_H
   #include <sys/types.h>
#endif

#if TIME_WITH_SYS_TIME
   #include <sys/time.h>
   #include <time.h>
#else
   #if HAVE_SYS_TIME_H
      #include <sys/time.h>
   #else
      #include <time.h>
   #endif
#endif


#include <sys/types.h>
#ifdef HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen ((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) ((dirent)->d_namlen)
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif


#include "PulseTLM.h"

//
// Input parsing definitions to use getopt.
//
extern char *optarg;
extern int optind, opterr, optopt;


int
main(int argc, char **argv)
{
  //
  // Generic array traversal
  //
  int i,j,k,n;
  //
  // Overall mesh dimensions of the TLM mesh in each cartesian direction
  //
  int nx, ny, nz;
  //
  // The spatial deltas in each cartesian direction with ds being the
  // smallest of the three;
  //
  double dx = -FLT_MAX;
  double dy = -FLT_MAX;
  double dz = -FLT_MAX;
  double ds;
  //
  // The temporal delta or how far time advances for one iteration
  //
  double dt;
  //
  // The current simulation time
  //
  double currentSimulationTime = 0.0;
  //
  // Spatial scaling of the voxel size based off the smallest spatial delta
  //
  double u,v,w;
  //
  // Voltage array pointer to contain all 18 base voltages which
  // represent the electric and magnetic fields within the voxel and
  // material reference information.  Utilize the single array
  // reference to ensure continuous memory access.
  //
  TlmVoxel *voxelBase;
  TlmVoxel ***voxel;
  //
  // Access to the logfile where runtime information is dumped.
  // PATH_MAX should be defined on POSIX systems to be at least 1024.
  //
  char logFileName[PATH_MAX];
  FILE *logFile;
  //
  // The actual input file where all problem specification information
  // would come from.
  //
  char inputFileName[PATH_MAX];
  int inputFileNameLength;
  FILE *inputFile;
  //
  // An error string for general output of errors and status messages
  //
  char errorString[1024];
  //
  // Generic timing information and statistics
  //
#ifdef HAVE_CLOCK_GETTIME
  struct timespec startTimespec;
  struct timespec stopTimespec;
  double seconds;
#endif
#ifdef HAVE_SYS_TIMES_H
  struct tms startTMS;
  struct tms stopTMS;
  double maximumSeconds;
  double userSeconds;
  double systemSeconds;
  double usSeconds;
#endif
#ifdef HAVE_TIME
  time_t startTime;
  time_t stopTime;
#endif
#ifdef HAVE_CLOCK
  clock_t startClock;
  clock_t stopClock;
  double clockSecondsCPU;
#endif
  //
  // The current iteration in the simulation
  //
  int currentIteration = 0;
  //
  // The maximum iteration requested by the user
  //
  int maximumIteration = -INT_MAX;
  //
  // The maximum simulation time requested by the user which overrides
  // the maximumIteration.
  //
  double maximumSimulationTime = -FLT_MAX;
  //
  // A linked list of boundaries specified in the input file, the
  // current boundary and the last boundary in the list.
  //
  Boundary *boundaryList = NULL;
  Boundary *boundary = NULL;
  Boundary *lastBoundary = NULL;
  //
  // A linked list of excitations specified in the input file, the
  // current excitation and the last excitation in the list.
  //
  Excitation *excitationList = NULL;
  Excitation *excitation = NULL;
  Excitation *lastExcitation = NULL;
  //
  // A linked list of all the materials specified in the input file,
  // the current material and the last material in the list.
  //
  Material *materialList = NULL;
  Material *material = NULL;
  Material *lastMaterial = NULL;
  //
  // A linked list of all the outputs specified in the input file,
  // the current output and the last output in the list.
  //
  Output *outputList = NULL;
  Output *output = NULL;
  Output *lastOutput = NULL;
  //
  // A linked list of all the TlmMaterials precalculated for rapid and
  // memory conserving access.
  //
  TlmMaterial *tlmMaterialList = NULL;
  TlmMaterial *tlmMaterial = NULL;
  TlmMaterial *lastTlmMaterial = NULL;
  //
  // A dat or 1D ASCII file access to each excitation drive
  //
  char excitationFileName[PATH_MAX];
  FILE *excitationFile;
  //
  // A viz control file.  Note that bob does not have the control file
  // as an option, by outputing data through the viz format, the
  // scaling is retained and BoB can still be used on the individual
  // bob files.
  //
  char vizFileName[PATH_MAX];
  FILE *vizFile;
  //
  // An XML file
  //
  char xmlFileName[PATH_MAX];
  FILE *xmlFile;
  //
  // Generic variables of no great inportance, (input parsing,
  // temporaries, etc)
  //
  int c;
  double rtmp1, rtmp2, rtmp3, rtmp4;
  double mur, epsr;
  double delt1, delt2, delt3, delt4, delt5, delt6;
  double yx, yy, yz, zx, zy, zz, g0;
  double v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16,v17,v18;
  double stimulus;
  double ex, ey, ez, hx, hy, hz;
  //
  // Filename used in the cleaning of the generated files
  //
  DIR *currentDirectory;
  struct dirent *thisDirectoryEntry;
  char removeFilename[PATH_MAX];
  char cwd[PATH_MAX+1];




  //
  // Start the overall simulation timing
  //
#ifdef HAVE_TIME
  startTime = time(NULL);
#endif
#ifdef HAVE_TIMES
  times(&startTMS);
#endif
#ifdef HAVE_CLOCK_GETTIME
  clock_gettime(CLOCK_REALTIME, &startTimespec);
#endif
#ifdef HAVE_CLOCK
  startClock = clock();
#endif
  //
  // Options parsing
  //
  if (argc <= 1)
    {
    sprintf(errorString, "usage: PulseTLM inputfile\n");
    fprintf(stderr, errorString);
    exit(1);
    }
  while ((c = getopt(argc, argv, "h?")) != -1)
    {
    // <FIXME>getopt long, to support --help?  Also the 
    // usage message doesn't mention the help option</FIXME>
    switch (c)
      {
      case '?':
      case 'h':
	sprintf(errorString, "usage: PulseTLM inputfile\n");
	fprintf(stderr, errorString);
	exit(1);
	break;
      }
    }
  //
  // Gain access to the current directory, if unable then probably
  // can't run the simulation here as we couldn't open any files.
  //
#ifdef HAVE_GETCWD
  if (getcwd(cwd, PATH_MAX+1) == NULL)
    {
    sprintf(errorString, "Can't open current directory, probably can't run here");
    fprintf(logFile, errorString);
    perror(errorString);
    exit(9);
    }
#else
  strcpy(cwd, ".");
#endif
  //
  // Assume the input file is the last argument on the command line,
  // save the name and remember it's length.
  //
  strcpy(inputFileName, argv[optind]);
  inputFileNameLength = strlen(inputFileName);
  //
  // Attempt to open the requested input file and fail if there's no
  // valid input file.
  //
  if ((inputFile = fopen(inputFileName, "r")) == NULL)
    {
    sprintf(errorString, "Can't open %s for read", inputFileName);
    fprintf(inputFile, errorString);
    perror(errorString);
    exit(9);
    }
  //
  // Gain access to the log file early on for all output.  The logs
  // are dated to avoid overwriting previous runs and help remind
  // when/where the simulation was run.  Create the log file based off
  // the input file name to easily locate it when directory listing.
  //
  sprintf(logFileName, "%s.log", inputFileName);
  if ((logFile = fopen(logFileName, "w")) == NULL)
    {
    sprintf(errorString, "Can't open %s for write", logFileName);
    fprintf(logFile, errorString);
    perror(errorString);
    exit(9);
    }
  //
  // Echo the starting time of this simulation.
  //
  fprintf(logFile, "\n");
  fprintf(logFile, "Simulation of %s commencing on %s", inputFileName, ctime(&startTime));
  fprintf(logFile, "\n");
  //
  // Remove any files that will be generated by this simulation, which
  // will avoid checking each time userOutput is called.  This
  // effectively cleans the directory for this particular set of files
  // to be generated.  This needs to be done because the userOutput
  // routines are append which would very neatly append any new data
  // to the end of the previously existing data files!  This is done
  // by removing anything that remotely resembles the inputFileName
  // and is longer.
  //
  fprintf(logFile, "\n");
  fprintf(logFile, "Old simulation file removal for %s commencing\n", inputFileName);
  fprintf(logFile, "\n");
  currentDirectory = opendir(cwd);
  if (currentDirectory == NULL)
    {
    sprintf(errorString, "Can't open %s for simulation file cleaning", cwd);
    fprintf(logFile, errorString);
    perror(errorString);
    exit(9);
    }
  while((thisDirectoryEntry = readdir(currentDirectory)) != NULL)
    {
    //
    // Locate the files that begin similarly to the inputFileName
    //
    if (strncmp(thisDirectoryEntry->d_name, inputFileName, inputFileNameLength) == 0)
      {
      //
      // Yet this particular filename may be longer than the
      // inputFileName in which case assume it is a result file and
      // blow it away.  Don't remove the log file, of course, since
      // it's recording everything.
      //
      if ( (strlen(thisDirectoryEntry->d_name) > inputFileNameLength) &&
	   (strcasecmp(thisDirectoryEntry->d_name,logFileName) != 0) )
	{
	sprintf(errorString, "Removing %s\n", thisDirectoryEntry->d_name);
	fprintf(logFile, errorString);
	unlink(thisDirectoryEntry->d_name);
	}
      }
    }
  if (closedir(currentDirectory) != 0)
    {
    sprintf(errorString, "Can't close %s after simulation file cleaning", cwd);
    fprintf(logFile, errorString);
    perror(errorString);
    exit(9);
    }
  fprintf(logFile, "\n");
  fprintf(logFile, "Old simulation file removal for %s complete\n", inputFileName);
  fprintf(logFile, "\n");
  //
  // Create one valid boundary so the initial list pointer will be
  // valid through the call to parse.  Set a key to parse code that
  // this is the initial malloc'ed boundary.
  //
  while ( (boundaryList = (Boundary *)malloc(sizeof(Boundary))) == NULL )
    {
    sprintf(errorString, "Allocation of %u bytes failed, pausing for %d seconds...\n",
	    sizeof(Boundary), 10);
    fprintf(logFile, errorString);
    perror(errorString);
    //
    sleep(10);
    }
  //
  boundary = boundaryList;
  boundary->next = NULL;
  boundary->type = 0;
  lastBoundary = boundaryList;
  //
  // Create one valid excitation so the initial list pointer will be
  // valid through the call to parse.  Set a key to parse code that
  // this is the initial malloc'ed excitation.
  //
  while ( (excitationList = (Excitation *)malloc(sizeof(Excitation))) == NULL )
    {
    sprintf(errorString, "Allocation of %u bytes failed, pausing for %d seconds...\n",
	    sizeof(Excitation), 10);
    fprintf(logFile, errorString);
    perror(errorString);
    //
    sleep(10);
    }
  //
  excitation = excitationList;
  excitation->next = NULL;
  excitation->type = 0;
  lastExcitation = excitationList;
  //
  // Create one valid material so the initial list pointer will be
  // valid through the call to parse.
  //
  while ( (materialList = (Material *)malloc(sizeof(Material))) == NULL )
    {
    sprintf(errorString, "Allocation of %u bytes failed, pausing for %d seconds...\n",
	    sizeof(Material), 10);
    fprintf(logFile, errorString);
    perror(errorString);
    //
    sleep(10);
    }
  //
  material = materialList;
  material->relativePermeability = 1.0;
  material->relativePermittivity = 1.0;
  material->conductivity = 0.0;
  material->next = NULL;
  lastMaterial = materialList;
  //
  // Create one valid output so the initial list pointer will be
  // valid through the call to parse.  Set a key to parse code that
  // this is the initial malloc'ed output.
  //
  while ( (outputList = (Output *)malloc(sizeof(Output))) == NULL )
    {
    sprintf(errorString, "Allocation of %u bytes failed, pausing for %d seconds...\n",
	    sizeof(Output), 10);
    fprintf(logFile, errorString);
    perror(errorString);
    //
    sleep(10);
    }
  //
  output = outputList;
  output->next = NULL;
  output->type = 0;
  lastOutput = outputList;
  //
  // Parse the input file and setup the mesh data
  //
  fprintf(logFile, "\nParse of %s commencing.\n\n", inputFileName);
  parseFile(inputFile, logFile,
	    &nx, &ny, &nz,
	    &maximumIteration, &maximumSimulationTime,
	    boundaryList, lastBoundary,
	    excitationList, lastExcitation,
	    materialList, lastMaterial,
	    outputList, lastOutput,
	    &dx, &dy, &dz);
  fprintf(logFile, "\nParse of %s complete.\n\n", inputFileName);
  //
  // The input file has been parsed, so close it.
  //
  fclose(inputFile);
  //
  // Allocate the memory for the main computational mesh and create
  // both access methods as viable.
  //
  voxelBase = (TlmVoxel *)malloc(nx*ny*nz*sizeof(TlmVoxel));
  if (voxelBase == NULL)
    {
    sprintf(errorString, "Unable to allocate %d bytes for voxelBase array\n",
	    nx*ny*nz*sizeof(TlmVoxel));
    fprintf(logFile, errorString);
    exit(9);
    }
  voxel = (TlmVoxel ***)malloc(nx*sizeof(TlmVoxel **));
  if (voxel == NULL)
    {
    sprintf(errorString, "Unable to allocate %d bytes for voxel[i] access array\n",
	    nx*sizeof(TlmVoxel **));
    fprintf(logFile, errorString);
    exit(9);
    }
  for(n=0, i=0; i<nx; i++)
    {
    voxel[i] = (TlmVoxel **)malloc(ny*sizeof(TlmVoxel *));
    if (voxel[i] == NULL)
      {
      sprintf(errorString, "Unable to allocate %d bytes for voxel[i][j] access array\n",
	      ny*sizeof(TlmVoxel **));
      fprintf(logFile, errorString);
      exit(9);
      }
    for(j=0; j<ny; j++)
      {
      voxel[i][j] = &(voxelBase[n]);
      n += nz;
      }
    }
  //
  // Find the smallest spatial delta
  //
  if ( (dx <= dy) && (dx <= dz) )
    {
    ds = dx;
    }
  if ( (dy <= dx) && (dy <= dz) )
    {
    ds = dy;
    }
  if ( (dz <= dx) && (dz <= dy) )
    {
    ds = dz;
    }
  //
  // Calculate scaling constants based off the spatial deltas.  These
  // are relative delta values which scale the stub quantities and
  // essentially distort the voxel to the requested size.
  //
  u = dx/ds;
  v = dy/ds;
  w = dz/ds;
  //
  // Set the first material, free space, to cover the entire mesh.
  //
  materialList->xLow = 1;
  materialList->xHigh = nx;
  materialList->yLow = 1;
  materialList->yHigh = ny;
  materialList->zLow = 1;
  materialList->zHigh = nz;
  //
  // Traverse the entire list of material blocks specified in the
  // input file and set the beginning parts of the allocated voxel
  // array to the material parameters.
  //
  material = materialList;
  while(material != NULL)
    {
    for(i=material->xLow-1; i<material->xHigh; i++)
      {
      for(j=material->yLow-1; j<material->yHigh; j++)
	{
	for(k=material->zLow-1; k<material->zHigh; k++)
	  {
	  voxel[i][j][k].v1 = material->relativePermeability;
	  voxel[i][j][k].v2 = material->relativePermittivity;
	  voxel[i][j][k].v3 = material->conductivity;
	  }
	}
      }
    //
    // Cycle through the list to the next material
    //
    material = material->next;
    }
  //
  // Calculate the temporal delta based on the distributed materials.
  //
  rtmp1 = FLT_MAX;
  for(n=0; n<nx*ny*nz; n++)
    {
    mur = voxelBase[n].v1;
    epsr = voxelBase[n].v2;
    //
    delt1=v*w*epsr/(u*2.0*LIGHT_SPEED);
    delt2=u*w*epsr/(v*2.0*LIGHT_SPEED);
    delt3=v*u*epsr/(w*2.0*LIGHT_SPEED);
    delt4=v*w*mur/(u*2.0*LIGHT_SPEED);
    delt5=u*w*mur/(v*2.0*LIGHT_SPEED);
    delt6=v*u*mur/(w*2.0*LIGHT_SPEED);
    //
    rtmp1 = (rtmp1 < delt1) ? rtmp1:delt1;   // pick smallest of rtmp1 and delt1
    rtmp1 = (rtmp1 < delt2) ? rtmp1:delt2;   // pick smallest of rtmp1 and delt2
    rtmp1 = (rtmp1 < delt3) ? rtmp1:delt3;   // pick smallest of rtmp1 and delt3
    rtmp1 = (rtmp1 < delt4) ? rtmp1:delt4;   // pick smallest of rtmp1 and delt4
    rtmp1 = (rtmp1 < delt5) ? rtmp1:delt5;   // pick smallest of rtmp1 and delt5
    rtmp1 = (rtmp1 < delt6) ? rtmp1:delt6;   // pick smallest of rtmp1 and delt6
    }
  //
  // Establish the temporal delta based on the previous estimate code
  //
  dt = rtmp1;
  //
  // Echo calculated temporal delta to the logfile
  //
  fprintf(logFile, "Temporal delta, dt = %lg\n", dt*ds);
  //
  // If the maximumSimulationTime variable has been set, then override
  // the maximumIteration value.
  //
  if (maximumSimulationTime > 0.0)
    {
#ifdef HAVE_RINT
    maximumIteration = (int)rint(maximumSimulationTime/dt);
#endif
#ifndef HAVE_RINT
#ifdef HAVE_CEIL
    maximumIteration = (int)ceil(maximumSimulationTime/dt);
#endif
#endif
    }
  //
  // Establish the first precalculated value, tlmMaterial.
  //
  tlmMaterialList = (TlmMaterial *)malloc(sizeof(TlmMaterial));
  tlmMaterial = tlmMaterialList;
  tlmMaterial->next = NULL;
  lastTlmMaterial = tlmMaterial;
  //
  // Calculate the values for this voxel which will be the first
  // allocated tlmMaterial.  All other voxels might simply reference
  // this first one if all the sizes and materials are identical.
  //
  mur = voxelBase[0].v1;
  epsr = voxelBase[0].v2;
  //
  rtmp1 = epsr/(dt*LIGHT_SPEED);
  rtmp2 = mur/(dt*LIGHT_SPEED);
  //
  tlmMaterial->yx = 2.0*(v*w*rtmp1/u - 2.0);
  tlmMaterial->yy = 2.0*(u*w*rtmp1/v - 2.0);
  tlmMaterial->yz = 2.0*(v*u*rtmp1/w - 2.0);
  tlmMaterial->zx = 2.0*(v*w*rtmp2/u - 2.0);
  tlmMaterial->zy = 2.0*(u*w*rtmp2/v - 2.0);
  tlmMaterial->zz = 2.0*(v*u*rtmp2/w - 2.0);
  //
  // Calculate stub parameters for each voxel and place them in the
  // precalculated list to conserve memory.
  //
  for(n=0; n<nx*ny*nz; n++)
    {
    //
    // Calculate the values for this voxel
    //
    mur = voxelBase[n].v1;
    epsr = voxelBase[n].v2;
    //
    rtmp1 = epsr/(dt*LIGHT_SPEED);
    rtmp2 = mur/(dt*LIGHT_SPEED);
    //
    yx = 2.0*(v*w*rtmp1/u - 2.0);
    yy = 2.0*(u*w*rtmp1/v - 2.0);
    yz = 2.0*(v*u*rtmp1/w - 2.0);
    zx = 2.0*(v*w*rtmp2/u - 2.0);
    zy = 2.0*(u*w*rtmp2/v - 2.0);
    zz = 2.0*(v*u*rtmp2/w - 2.0);
    //
    // Search the TlmMaterial list for viable pointers.  If one is
    // located, simply refer to it, otherwise, create a new
    // precalculated material, add it to the list and continue to the
    // next voxel.
    //
    tlmMaterial = tlmMaterialList;
    while(tlmMaterial != NULL)
      {
      if ( (tlmMaterial->yx == yx) &&
	   (tlmMaterial->yy == yy) &&
	   (tlmMaterial->yz == yz) &&
	   (tlmMaterial->zx == zx) &&
	   (tlmMaterial->zy == zy) &&
	   (tlmMaterial->zz == zz) )
	{
	//
	// This set of values exists in the precalculated list, break
	// out of the search routine, leaving tlmMaterial at a valid
	// pointer.
	//
	break;
	}
      //
      // Cycle through the precalculated values 
      //
      tlmMaterial = tlmMaterial->next;
      }
    //
    // If tlmMaterial is a valid pointer then simply set this voxel to
    // refer to that precalculated set of values, otherwise we must
    // add another set of precalculated values to the list.
    //
    if (tlmMaterial != NULL)
      {
      voxelBase[n].tlmMaterial = tlmMaterial;
      }
    else
      {
      lastTlmMaterial->next = (TlmMaterial *)malloc(sizeof(TlmMaterial));
      tlmMaterial = lastTlmMaterial->next;
      tlmMaterial->yx = yx;
      tlmMaterial->yy = yy;
      tlmMaterial->yz = yz;
      tlmMaterial->zx = zx;
      tlmMaterial->zy = zy;
      tlmMaterial->zz = zz;
      tlmMaterial->next = NULL;
      lastTlmMaterial = tlmMaterial;
      //
      voxelBase[n].tlmMaterial = tlmMaterial;
      }
    }
  //
  // Clear entire memory array explicitly.
  //
  for(n=0; n<nx*ny*nz; n++)
    {
    voxelBase[n].v1 = 0.0;
    voxelBase[n].v2 = 0.0;
    voxelBase[n].v3 = 0.0;
    //
    voxelBase[n].v4 = 0.0;
    voxelBase[n].v5 = 0.0;
    voxelBase[n].v6 = 0.0;
    //
    voxelBase[n].v7 = 0.0;
    voxelBase[n].v8 = 0.0;
    voxelBase[n].v9 = 0.0;
    //
    voxelBase[n].v10 = 0.0;
    voxelBase[n].v11 = 0.0;
    voxelBase[n].v12 = 0.0;
    //
    voxelBase[n].v13 = 0.0;
    voxelBase[n].v14 = 0.0;
    voxelBase[n].v15 = 0.0;
    //
    voxelBase[n].v16 = 0.0;
    voxelBase[n].v17 = 0.0;
    voxelBase[n].v18 = 0.0;
    }

  //
  // Traverse the outputs output any data which is only done at the
  // beginning of the file.  This will avoid rechecking during output.
  // This is also only really necessary in the cases where the file is
  // always opened append.
  //
  output = outputList;
  while(output != NULL)
    {
    switch(output->type)
      {
      case VIZ:
	//
	// Gain access to the viz file.
	//
	sprintf(vizFileName, "%s_o%d.viz", inputFileName, output->uniqueNumber);
	while ((vizFile = fopen(vizFileName, "w")) == NULL)
	  {
	  //
	  // If unable to open the file, inform user, delay and try again
	  snprintf(errorString, 1024, "Difficulty opening [%s]; pausing for 15 sec", vizFileName);
	  perror(errorString);
	  sleep(15);
	  }
	//
	// Place intro information into the viz file
	//
	fprintf(vizFile, "#Viz V1.0\n");
	fprintf(vizFile, "time: %lg %lg\n", 0.0, dt*ds);
	fprintf(vizFile, "color: gbry.cmap\n\n");
	//
	// Close it up since we're done with intro information
	//
	fclose(vizFile);
	break;
      case PNM:
	//
	// All the PNM files are opened write and require no initial data.
	//
	break;
      case MTV:
	//
	// MTV (plotMTV) data format 2D output
	//
	break;
      case GRACE:
	//
	// grace (xmgr) data format 1D output
	//
	break;
      case DAT:
	//
	// dat data format or straight 1D ASCII file
	//
	break;
      case BINARY:
	//
	// binary dump 1-2-3D dump of ex, ey, ez, hx, hy, hz
	//
	break;
      case XML:
	//
	// XML dump 1-2-3D dump of ex, ey, ez, hx, hy, hz
	//
	sprintf(xmlFileName, "%s_o%d.xml", inputFileName, output->uniqueNumber);
	while ((xmlFile = fopen(xmlFileName, "w")) == NULL)
	  {
	  //
	  // If unable to open the file, inform user, delay and try again
	  snprintf(errorString, 1024, "Difficulty opening [%s]; pausing for 15 sec", vizFileName);
	  perror(errorString);
	  sleep(15);
	  }
	//
	// Place intro information into the viz file
	//
	fprintf(xmlFile, "<?xml version=\"1.0\" encoding='UTF-8'?>\n<pulsetlm>\n");
	//
	// Close it up since we're done with intro information
	//
	fclose(xmlFile);
	break;
      default:
	snprintf(errorString, 1024,
		 "Output type %d not recognized....", output->type);
	fprintf(logFile, errorString);
	break;
      }
    //
    // Cycle through all the outputs
    //
    output = output->next;
    }

  fprintf(logFile, "\n");
  fprintf(logFile, "\n");

  //
  // This is the main TLM calculation/iteration loop which performs
  // each chunk of calculations to advance time forward.
  //
  for(currentIteration = 0; currentIteration < maximumIteration; currentIteration++)
    {


    //
    // Trek through the output list performing any modulo output at
    // the request of the user.
    //
    userOutput(logFile,
	       nx, ny, nz,
	       ds, dt,
	       u, v, w,
	       currentIteration,
	       maximumIteration,
	       currentSimulationTime,
	       inputFileName,
	       voxelBase, voxel,
	       outputList);






    //
    // Apply any excitations that are not impulsive, those should not
    // be present if realtime excitations are specified in the file as
    // well.
    //
    excitation = excitationList;
    while(excitation != NULL)
      {
      //
      // Gain access to the excitations file to record the actual
      // stimulus values.
      //
      sprintf(excitationFileName, "%s_e%d.dat",
	      inputFileName, excitation->uniqueNumber);
      while ((excitationFile = fopen(excitationFileName, "a")) == NULL)
	{
	//
	// If unable to open the file, inform user, delay and try again
	//
	fprintf(stderr, "Difficulty opening %s, pausing for 15 sec",
		excitationFileName);
	perror(" ");
	sleep(15);
	}
      //
      // Calculate the stimulus value based on the excitation type.
      //
      stimulus = 0.0;
      switch(excitation->type)
	{
	case IMPULSE:
	  if (currentIteration == 0)
	    {
	    stimulus = excitation->value1;
	    }
	  break;
	case SINUSOID:
 	  if ( (currentSimulationTime - excitation->value3) > 0.0)
 	    {
	    stimulus = sin(2.0*M_PI*excitation->value2*(currentSimulationTime -
							excitation->value3));
	    stimulus *= excitation->value1;
	    stimulus += excitation->value4;
  	    }
	  break;
	case EXPONENTIAL:
//  	  if ( (currentSimulationTime - excitation->value2) > 0.0)
//  	    {
	    stimulus = exp(-pow((currentSimulationTime -
				 excitation->value2)/excitation->value3, 2.0));
	    stimulus *= excitation->value1;
	    stimulus += excitation->value4;
//   	    }
	  break;
	default:
	  snprintf(errorString, 1024,
		   "Excitation type %d not recognized....",
		   excitation->type);
	  fprintf(logFile, errorString);
	  break;
	}
      //
      // Dump the data into the file
      //
      fprintf(excitationFile, "%lg %lg\n", currentSimulationTime, stimulus);
      //
      // Close off the file for the next time around
      //
      fclose(excitationFile);
      //
      // Set the appropriate field value for the excitation
      //
      ex = 0.0;
      ey = 0.0;
      ez = 0.0;
      hx = 0.0;
      hy = 0.0;
      hz = 0.0;
      switch(excitation->field)
	{
	case EX:
	  ex = stimulus;
	  break;
	case EY:
	  ey = stimulus;
	  break;
	case EZ:
	  ez = stimulus;
	  break;
	case HX:
	  hx = stimulus;
	  break;
	case HY:
	  hy = stimulus;
	  break;
	case HZ:
	  hz = stimulus;
	  break;
	default:
	  snprintf(errorString, 1024, "Excitation field type %d not recognized....", excitation->field);
	  fprintf(logFile, errorString);
	  break;
	}
      //
      // Map those excited fields into the voltages utilized in
      // the TLM method.
      //
      for(i=excitation->xLow-1; i<excitation->xHigh; i++)
	{
	for(j=excitation->yLow-1; j<excitation->yHigh; j++)
	  {
	  for(k=excitation->zLow-1; k<excitation->zHigh; k++)
	    {
	    //
	    // Access the stub parameters to make the calculations
	    // easier to read and optimize.
	    //
	    tlmMaterial = voxel[i][j][k].tlmMaterial;
	    //
	    yx = (tlmMaterial)->yx;
	    yy = (tlmMaterial)->yy;
	    yz = (tlmMaterial)->yz;
	    zx = (tlmMaterial)->zx;
	    zy = (tlmMaterial)->zy;
	    zz = (tlmMaterial)->zz;
	    g0 = 0.0;
	    //
	    voxel[i][j][k].v1  += (u*ex + w*Z_0*hz)*0.5;
	    voxel[i][j][k].v2  += (u*ex + v*Z_0*hy)*0.5;
	    voxel[i][j][k].v3  += (v*ey - w*Z_0*hz)*0.5;
	    //		    
	    voxel[i][j][k].v4  += (v*ey + u*Z_0*hx)*0.5;
	    voxel[i][j][k].v5  += (w*ez - u*Z_0*hx)*0.5;
	    voxel[i][j][k].v6  += (w*ez + v*Z_0*hy)*0.5;
	    //
	    voxel[i][j][k].v7  += (w*ez + u*Z_0*hx)*0.5;
	    voxel[i][j][k].v8  += (v*ey - u*Z_0*hx)*0.5;
	    voxel[i][j][k].v9  += (u*ex + v*Z_0*hy)*0.5;
	    //
	    voxel[i][j][k].v10 += (w*ez - v*Z_0*hy)*0.5;
	    voxel[i][j][k].v11 += (v*ey + w*Z_0*hz)*0.5;
	    voxel[i][j][k].v12 += (u*ex - w*Z_0*hz)*0.5;
	    //
	    voxel[i][j][k].v13 += u*ex*0.5;
	    voxel[i][j][k].v14 += v*ey*0.5;
	    voxel[i][j][k].v15 += w*ez*0.5;
	    //
	    voxel[i][j][k].v16 -= zx*Z_0*u*hx*0.5;
	    voxel[i][j][k].v17 -= zy*Z_0*v*hy*0.5;
	    voxel[i][j][k].v18 -= zz*Z_0*w*hz*0.5;
	    }
	  }
	}
      //
      // Cycle through all the excitations
      //
      excitation = excitation->next;
      }




    //
    // Perform the scattering operation for each voxel.  This is
    // generally the most time and CPU comsuming portion of the
    // simulation.
    //
    for(n=0; n<nx*ny*nz; n++)
      {
      //
      // Store the voltages in temporary variables since the
      // update equations will modify the values and thus skew
      // results.
      //
      v1 = voxelBase[n].v1;
      v2 = voxelBase[n].v2;
      v3 = voxelBase[n].v3;
      //                
      v4 = voxelBase[n].v4;
      v5 = voxelBase[n].v5;
      v6 = voxelBase[n].v6;
      //                
      v7 = voxelBase[n].v7;
      v8 = voxelBase[n].v8;
      v9 = voxelBase[n].v9;
      //                
      v10 = voxelBase[n].v10;
      v11 = voxelBase[n].v11;
      v12 = voxelBase[n].v12;
      //                
      v13 = voxelBase[n].v13;
      v14 = voxelBase[n].v14;
      v15 = voxelBase[n].v15;
      //                
      v16 = voxelBase[n].v16;
      v17 = voxelBase[n].v17;
      v18 = voxelBase[n].v18;
      //
      tlmMaterial = voxelBase[n].tlmMaterial;
      //
      yx = (tlmMaterial)->yx;
      yy = (tlmMaterial)->yy;
      yz = (tlmMaterial)->yz;
      zx = (tlmMaterial)->zx;
      zy = (tlmMaterial)->zy;
      zz = (tlmMaterial)->zz;
      g0 = 0.0;
      //
      // Perform the scattering for the voltages representing the
      // electric and magnetic fields
      //
      voxelBase[n].v1 = (2.0*(v3-v11+v18)+(v1-v12)*(zz*0.5))
	/(4.0+zz)+(2.0*(v2+v9+v13*yx)
				-(v1+v12)*(yx*0.5))/(yx+4.0+g0);

      voxelBase[n].v2 = (2.0*(v6-v10-v17)+(v2-v9)*(zy*0.5))
	/(4.0+zy)+(2.0*(v1+v12+v13*yx)
				-(v2+v9)*(yx*0.5))/(4.0+yx+g0);

      voxelBase[n].v3 = (2.0*(v1-v12-v18)+(v3-v11)*(zz*0.5))
	/(4.0+zz)+(2.0*(v4+v8+v14*yy)
				-(v3+v11)*(yy*0.5))/(4.0+yy+g0);

      voxelBase[n].v4 = (2.0*(v5-v7+v16)+(v4-v8)*(zx*0.5))
	/(4.0+zx)+(2.0*(v3+v11+v14*yy)
				-(v4+v8)*(yy*0.5))/(4.0+yy+g0);

      voxelBase[n].v5 = (2.0*(v4-v8-v16)+(v5-v7)*(zx*0.5))
	/(4.0+zx)+(2.0*(v6+v10+v15*yz)
				-(v5+v7)*(yz*0.5))/(4.0+yz+g0);

      voxelBase[n].v6 = (2.0*(v2-v9+v17)+(v6-v10)*(zy*0.5))
	/(4.0+zy)+(2.0*(v5+v7+v15*yz)
				-(v6+v10)*(yz*0.5))/(4.0+yz+g0);

      voxelBase[n].v7 = (2.0*(-v4+v8+v16)-(v5-v7)*(zx*0.5))
	/(4.0+zx)+(2.0*(v6+v10+v15*yz)
				-(v5+v7)*(yx*0.5))/(4.0+yz+g0);

      voxelBase[n].v8 = (2.0*(-v5+v7-v16)-(v4-v8)*(zx*0.5))
	/(4.0+zx)+(2.0*(v3+v11+v14*yy)
				-(v4+v8)*(yy*0.5))/(4.0+yy+g0);

      voxelBase[n].v9 = (2.0*(-v6+v10+v17)-(v2-v9)*(zy*0.5))
	/(4.0+zy)+(2.0*(v1+v12+v13*yx)
				-(v2+v9)*(yx*0.5))/(4.0+yx+g0);

      voxelBase[n].v10 = (2.0*(-v2+v9-v17)-(v6-v10)*(zy*0.5))
	/(4.0+zy)+(2.0*(v5+v7+v15*yz)
				-(v6+v10)*(yz*0.5))/(4.0+yz+g0);

      voxelBase[n].v11 = (2.0*(-v1+v12+v18)-(v3-v11)*(zz*0.5))
	/(4.0+zz)+(2.0*(v4+v8+v14*yy)
				-(v3+v11)*(yy*0.5))/(4.0+yy+g0);

      voxelBase[n].v12 = (2.0*(-v3+v11-v18)-(v1-v12)*(zz*0.5))
	/(4.0+zz)+(2.0*(v2+v9+v13*yx)
				-(v1+v12)*(yx*0.5))/(4.0+yx+g0);

      //
      // Upgrade the voltages representing the materials and voxel size
      //
      voxelBase[n].v13 = (2.0*(v1+v2+v9+v12)+(yx-4.0)*v13)/(4.0+yx+g0);

      voxelBase[n].v14 = (2.0*(v3+v4+v8+v11)+(yy-4.0)*v14)/(4.0+yy+g0);

      voxelBase[n].v15 = (2.0*(v5+v6+v7+v10)+(yz-4.0)*v15)/(4.0+yz+g0);

      voxelBase[n].v16 = -(2.0*zx*(v4-v5+v7-v8)+
			   (4.0-zx)*v16)/(4.0+zx);

      voxelBase[n].v17 = -(2.0*zy*(-v2+v6+v9-v10)+
			   (4.0-zy)*v17)/(4.0+zy);

      voxelBase[n].v18 = -(2.0*zz*(v1-v3+v11-v12)+
			   (4.0-zz)*v18)/(4.0+zz);
      }
    
    //
    // Perform the reflections off the faces of each voxel.  Take
    // care, here.  The operations here may be a bit perplexing
    // because immediately after this is a generic swap of all
    // connections in the next set of loops.  "Connect the voxels" In
    // other words, the reflections here end up on the "wrong" side of
    // the interface, becasuse they will be swapped in the code
    // shortly after this operation.
    //
    boundary = boundaryList;
    while(boundary != NULL)
      {
      switch (boundary->type)
	{
	case 1:
	  //
	  // Calculate the reflections off the upper x face
	  //
	  for(i=boundary->xLow-1; i<boundary->xHigh; i++)
	    {
	    for(j=boundary->yLow-1; j<boundary->yHigh; j++)
	      {
	      for(k=boundary->zLow-1; k<boundary->zHigh; k++)
		{
		if(i==nx-1)
		  {
		  voxel[i][j][k].v10 *= boundary->rho;
		  voxel[i][j][k].v11 *= boundary->rho;
		  }
		else
		  {
		  v10 = voxel[i][j][k].v10;
		  voxel[i][j][k].v10 =
		    boundary->tau*voxel[i][j][k].v10 +
		    boundary->rho*voxel[i+1][j][k].v6;
		  
		  v11 = voxel[i][j][k].v11;
		  voxel[i][j][k].v11 = 
		    boundary->tau*voxel[i][j][k].v11 + 
		    boundary->rho*voxel[i+1][j][k].v3;
		  
		  voxel[i+1][j][k].v6 = 
		    boundary->tau*voxel[i+1][j][k].v6 +
		    boundary->rho*v10;

		  voxel[i+1][j][k].v3 = 
		    boundary->tau*voxel[i+1][j][k].v3 +
		    boundary->rho*v11;
		  }
		}
	      }
	    }
	  break;
	case 2:
	  //
	  // Calculate the reflections off the upper y face
	  //
	  for(i=boundary->xLow-1; i<boundary->xHigh; i++)
	    {
	    for(j=boundary->yLow-1; j<boundary->yHigh; j++)
	      {
	      for(k=boundary->zLow-1; k<boundary->zHigh; k++)
		{
		if(j==ny-1)
		  {
		  voxel[i][j][k].v7 *= boundary->rho;
		  voxel[i][j][k].v12 *= boundary->rho;
		  }
		else
		  {
		  v7 = voxel[i][j][k].v7;
		  voxel[i][j][k].v7 = 
		    boundary->tau*voxel[i][j][k].v7 +
		    boundary->rho*voxel[i][j+1][k].v5;
		  
		  v12 = voxel[i][j][k].v12;
		  voxel[i][j][k].v12 = 
		    boundary->tau*voxel[i][j][k].v12 +
		    boundary->rho*voxel[i][j+1][k].v1; 
		  
		  voxel[i][j+1][k].v5 = 
		    boundary->tau*voxel[i][j+1][k].v5 +
		    boundary->rho*v7;

		  voxel[i][j+1][k].v1 = 
		    boundary->tau*voxel[i][j+1][k].v1 +
		    boundary->rho*v12;
		  }
		}
	      }
	    }
	  break;
	case 3:
	  //
	  // Calculate the reflections off the upper z face
	  //
	  for(i=boundary->xLow-1; i<boundary->xHigh; i++)
	    {
	    for(j=boundary->yLow-1; j<boundary->yHigh; j++)
	      {
	      for(k=boundary->zLow-1; k<boundary->zHigh; k++)
		{
		if(k==nz-1)
		  {
		  voxel[i][j][k].v8 *= boundary->rho;
		  voxel[i][j][k].v9 *= boundary->rho;
		  }
		else
		  {
		  v8 = voxel[i][j][k].v8;
		  voxel[i][j][k].v8 = 
		    boundary->tau*voxel[i][j][k].v8 + 
		    boundary->rho*voxel[i][j][k+1].v4;

		  v9 = voxel[i][j][k].v9;
		  voxel[i][j][k].v9 = 
		    boundary->tau*voxel[i][j][k].v9 + 
		    boundary->rho*voxel[i][j][k+1].v2;

		  voxel[i][j][k+1].v4 = 
		    boundary->tau*voxel[i][j][k+1].v4 + 
		    boundary->rho*v8;

		  voxel[i][j][k+1].v2 = 
		    boundary->tau*voxel[i][j][k+1].v2 +
		    boundary->rho*v9;
		  }
		}
	      }
	    }
	  break;
	case 4:
	  //
	  // Calculate the reflections off the lower x face
	  //
	  for(i=boundary->xLow-1; i<boundary->xHigh; i++)
	    {
	    for(j=boundary->yLow-1; j<boundary->yHigh; j++)
	      {
	      for(k=boundary->zLow-1; k<boundary->zHigh; k++)
		{
		if(i==0)
		  {
		  voxel[i][j][k].v3 *= boundary->rho;
		  voxel[i][j][k].v6 *= boundary->rho;
		  }
		else
		  {
		  v3 = voxel[i][j][k].v3;
		  voxel[i][j][k].v3 = 
		    boundary->tau*voxel[i][j][k].v3 +
		    boundary->rho*voxel[i-1][j][k].v11;

		  v6 = voxel[i][j][k].v6;
		  voxel[i][j][k].v6 = 
		    boundary->tau*voxel[i][j][k].v6 +
		    boundary->rho*voxel[i-1][j][k].v10;

		  voxel[i-1][j][k].v11 = 
		    boundary->tau*voxel[i-1][j][k].v11 +
		    boundary->rho*v3;

		  voxel[i-1][j][k].v10 = 
		    boundary->tau*voxel[i-1][j][k].v10 +
		    boundary->rho*v6;
		  }
		}
	      }
	    }
	  break;
	case 5:
	  //
	  // Calculate the reflections off the lower y face
	  //
	  for(i=boundary->xLow-1; i<boundary->xHigh; i++)
	    { 
	    for(j=boundary->yLow-1; j<boundary->yHigh; j++)
	      {
	      for(k=boundary->zLow-1; k<boundary->zHigh; k++)
		{
		if(j==0)
		  {
		  voxel[i][j][k].v1 *= boundary->rho;
		  voxel[i][j][k].v5 *= boundary->rho;
		  }
		else
		  {
		  v1 = voxel[i][j][k].v1;
		  voxel[i][j][k].v1 = 
		    boundary->tau*voxel[i][j][k].v1 + 
		    boundary->rho*voxel[i][j-1][k].v12;

		  v5 = voxel[i][j][k].v5;
		  voxel[i][j][k].v5 = 
		    boundary->tau*voxel[i][j][k].v5 + 
		    boundary->rho*voxel[i][j-1][k].v7;

		  voxel[i][j-1][k].v7 = 
		    boundary->tau*voxel[i][j-1][k].v7 +
		    boundary->rho*v5;

		  voxel[i][j-1][k].v12 = 
		    boundary->tau*voxel[i][j-1][k].v12 +
		    boundary->rho*v1;
		  }
		}
	      }
	    }
	  break;
	case 6:
	  //
	  // Calculate the reflections off the lower z face
	  //
	  for(i=boundary->xLow-1; i<boundary->xHigh; i++)
	    { 
	    for(j=boundary->yLow-1; j<boundary->yHigh; j++)
	      {
	      for(k=boundary->zLow-1; k<boundary->zHigh; k++)
		{
		if(k==0)
		  {
		  voxel[i][j][k].v2 *= boundary->rho;
		  voxel[i][j][k].v4 *= boundary->rho;
		  }
		else
		  {
		  v2 = voxel[i][j][k].v2;
		  voxel[i][j][k].v2 = 
		    boundary->tau*voxel[i][j][k].v2 +
		    boundary->rho*voxel[i][j][k-1].v9;

		  v4 = voxel[i][j][k].v4;
		  voxel[i][j][k].v4 = 
		    boundary->tau*voxel[i][j][k].v4 +
		    boundary->rho*voxel[i][j][k-1].v8;

		  voxel[i][j][k-1].v8 = 
		    boundary->tau*voxel[i][j][k-1].v8 +
		    boundary->rho*v4;

		  voxel[i][j][k-1].v9 = 
		    boundary->tau*voxel[i][j][k-1].v9 +
		    boundary->rho*v2;
		  }

		}
	      }
	    }
	  break;
	default:
	  snprintf(errorString, 1024, "Boundary type %d not recognized....", boundary->type);
	  fprintf(logFile, errorString);
	  break;
	}
      //
      // Cycle through to the next boundary
      //
      boundary = boundary->next;
      }




    //
    // Connect the voxels in the x direction.  Note that temporary
    // variables must be used since this is in essence a memory swap.
    // Without the tempoaries, the originals would be overwritten
    // before the swap was finished.
    //
    for(i=0;i<nx-1;i++)
      {
      for(j=0;j<ny;j++)
	{
	for(k=0;k<nz;k++)
	  {
	  //
	  v3 = voxel[i+1][j][k].v3;
	  voxel[i+1][j][k].v3 = voxel[i][j][k].v11;
	  voxel[i][j][k].v11 = v3;
	  //
	  v6 = voxel[i+1][j][k].v6;
	  voxel[i+1][j][k].v6 = voxel[i][j][k].v10;
	  voxel[i][j][k].v10 = v6;
	  }
	}
      }

    //
    // Connect the voxels in the y direction
    //
    for(j=0;j<ny-1;j++)
      {
      for(i=0;i<nx;i++)
	{
	for(k=0;k<nz;k++)
	  {
	  //
	  v5 = voxel[i][j+1][k].v5;
	  voxel[i][j+1][k].v5 = voxel[i][j][k].v7;
	  voxel[i][j][k].v7 = v5;
	  //
	  v1 = voxel[i][j+1][k].v1;
	  voxel[i][j+1][k].v1 = voxel[i][j][k].v12;
	  voxel[i][j][k].v12 = v1;
	  }
	}
      }

    //
    // Connect the voxels in the z direction
    //
    for(k=0;k<nz-1;k++)
      {
      for(i=0;i<nx;i++)
	{
	for(j=0;j<ny;j++)
	  {
 	  //
	  v4 = voxel[i][j][k+1].v4;
	  voxel[i][j][k+1].v4 = voxel[i][j][k].v8;
	  voxel[i][j][k].v8 = v4;
	  //
	  v2 = voxel[i][j][k+1].v2;
	  voxel[i][j][k+1].v2 = voxel[i][j][k].v9;
	  voxel[i][j][k].v9 = v2;
	  }
	}
      }
    //
    // Time in simulated seconds that the simulation has progressed.
    //
    currentSimulationTime += dt*ds;  
    //
    // Print (to logfile) the iteration number and current simulated time.
    //
    fprintf(logFile, "#%d %lg sec\n", currentIteration+1, currentSimulationTime);
    fflush(logFile);
    }










  //
  // Call output once more for any last output at the end of the simulation
  //
  userOutput(logFile,
	     nx, ny, nz,
	     ds, dt,
	     u, v, w,
	     currentIteration,
	     maximumIteration,
	     currentSimulationTime,
	     inputFileName,
	     voxelBase, voxel,
	     outputList);


  //
  //
  //
#ifdef HAVE_CLOCK
  stopClock = clock();
#endif
#ifdef HAVE_CLOCK_GETTIME
  clock_gettime(CLOCK_REALTIME, &stopTimespec);
#endif
#ifdef HAVE_TIMES
  times(&stopTMS);
#endif
#ifdef HAVE_TIME
  stopTime = time(NULL);
#endif


  fprintf(logFile, "\n");
  fprintf(logFile, "\n");

  fprintf(logFile, "\n");
  fprintf(logFile, "Simulation of %s commenced on %s", inputFileName, ctime(&startTime));
  fprintf(logFile, "Simulation of %s complete on %s", inputFileName, ctime(&stopTime));
  fprintf(logFile, "\n");


  //
  // Top level timing stats including parsing, etc
  //

#ifdef HAVE_CLOCK_GETTIME
  seconds = ( ((double)stopTimespec.tv_sec - (double)startTimespec.tv_sec) +
	      1.0e-9*((double)stopTimespec.tv_nsec - (double)startTimespec.tv_nsec) );
  fprintf(logFile, "Simulation required %lg sec (clock_gettime)\n", seconds);
#endif
#ifdef HAVE_TIMES
  userSeconds = ((double)stopTMS.tms_utime -
		 (double)startTMS.tms_utime)/(double)sysconf(_SC_CLK_TCK);
  systemSeconds = ((double)stopTMS.tms_stime -
		   (double)startTMS.tms_stime)/(double)sysconf(_SC_CLK_TCK);
  usSeconds = userSeconds + systemSeconds;
  fprintf(logFile, "Simulation required %lg CPUsec, User %lg CPUsec, System %lg CPUsec (times)\n",
	  usSeconds, userSeconds, systemSeconds);
#endif
#ifdef HAVE_CLOCK
  clockSecondsCPU = ((double)stopClock - (double)startClock)/(double)CLOCKS_PER_SEC;
  fprintf(logFile, "Simulation required %lg CPUsec (clock)\n", clockSecondsCPU);
#endif

}

