//
// PulseTLM.h
//
// This is PulseTLM a more full-featured Transmission Line Matrix
// (TLM) method simulation engine.
//
// Copyright (C) 1999- Paul Hayes
//
// ThediaLineis program is free software; you can redistribute it and/or
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
// http://www.cemtach.com
// http://www.cemtachgroup.com
//



//
// Overkill on the precision is used here to allow the compiler to
// pick the precision that it supports.
//
//
// Speed of light in a vacuum in meters/second
//
#define LIGHT_SPEED 299792458.0       
//
// Permeability of free space in henry/meter
//
#define MU_0 1.2566370614359172953850573533118011536788677597500423283899778369231265625144835994512139301368468271e-6
//
// Permittivity of free space in farad/meter
//
#define EPSILON_0 8.8541878176203898505365630317107502606083701665994498081024171524053950954599821142852891607182008932e-12
//
// Impedance of freespace or sqrt(MU_0/EPS_0)
//
#define Z_0 376.734309182110149436898630439319


// Define some of "magic numbers" which represent types, fields, etc
// Boundary types:
#define RTXH 1
#define RTYH 2
#define RTZH 3
#define RTXL 4
#define RTYL 5
#define RTZL 6
// Fields, used by Excitation and Output
#define EX 1
#define EY 2
#define EZ 3
#define HX 4
#define HY 5
#define HZ 6
// fields used by Output
#define E_MAGNITUDE 7
#define H_MAGNITUDE 8
// Excitation types
#define IMPULSE 1
#define SINUSOID 2
#define EXPONENTIAL 3
// Output formats
#define VIZ 1
#define PNM 2
#define MTV 3
#define GRACE 4
#define DAT 5
#define BINARY 6
#define XML 7
#define VTU 8




//
// Typedef the structs for clarity in code
//
typedef struct outputStruct Output;
typedef struct boundaryStruct Boundary;
typedef struct excitationStruct Excitation;
typedef struct tlmVoxelStruct TlmVoxel;
typedef struct materialStruct Material;
typedef struct tlmMaterialStruct TlmMaterial;









//
// Create an output structure to define user requested output regions.
// Currently, these are integer quantities based on the mesh itself.
// The low and high values are the range.
//
struct outputStruct
{
  //
  // The type is based on the user input file.  The unsigned short
  // should yield 256 different values which is more than enough for
  // the code currently.  This determines how to output the data.
  // (viz) 3d, (pnm, mtv) 2d, (grace, dat) 1d, (binary) 1-2-3d
  //
  int type;
  //
  // Which field quantity to output.
  // (ex, ey, ez, hx, hy, hz, emagnitude, hmagnitude)
  //
  int field;
  //
  // A unique number for this output which helps keep separate
  // output requests separate.
  //
  int uniqueNumber;
  //
  // The region of output.
  //
  int xLow;
  int xHigh;
  int yLow;
  int yHigh;
  int zLow;
  int zHigh;
  //
  // A temporal modulo for the output which may be useful if an actual
  // input source is specified.
  //
  int modulo;
  //
  // Linked list pointer to the next output in the list
  //
  Output *next;
};




//
// Create a boundary structure to define user boundary regions.
// Currently, the locations are based on the mesh itself.
//
struct boundaryStruct
{
  //
  // The type is based on the user input file.  The unsigned short
  // should yield 256 different values which is more than enough for
  // the code currently.
  //
  int type;
  //
  int xLow;
  int xHigh;
  int yLow;
  int yHigh;
  int zLow;
  int zHigh;
  //
  // The constants utilized by the type boundary.
  //
  double rho;
  double tau;
  //
  // Linked list pointer to the next boundary in the list
  //
  Boundary *next;
};

//
// Create a excitation structure to define user excitation regions.
// Currently, the locations are based on the mesh itself.
//
struct excitationStruct
{
  //
  // The type is based on the user input file.  The unsigned short
  // should yield 256 different values which is more than enough for
  // the code currently.
  //
  int type;
  //
  // Which field quantity to modify.
  //
  int field;
  //
  // A unique number for this excitation which helps keep separate
  // output requests separate.
  //
  int uniqueNumber;
  //
  // The region of application, normally a box is not used, but this
  // arrangement allows point, lines and planes of excitation.
  //
  int xLow;
  int xHigh;
  int yLow;
  int yHigh;
  int zLow;
  int zHigh;
  //
  // The constants utilized by the type excitation.  The actual use may
  // vary, so specific names are avoided.
  //
  double value1;
  double value2;
  double value3;
  double value4;
  double value5;
  //
  // Linked list pointer to the next excitation in the list
  //
  Excitation *next;
};

//
// Create struct for each voxel in cartesian space to contain the
// relevant information relating to the electric and magnetic field
// information within that voxel.
//
struct tlmVoxelStruct
{
  //
  // First 12 voltages essentially model the electric and magnetic
  // fields within the voxel.
  //
  float v1;
  float v2;
  float v3;
  float v4;
  float v5;
  float v6;
  float v7;
  float v8;
  float v9;
  float v10;
  float v11;
  float v12;
  //
  // Last 6 voltages essentially model spatial distortions of the cell
  // or material parameters within the cell.
  //
  float v13;
  float v14;
  float v15;
  float v16;
  float v17;
  float v18;
  //
  // The material associated with this voxel.
  //
  TlmMaterial *tlmMaterial;
};


struct materialStruct
{
  //
  // The region of space within the overall mesh that has these
  // material properties.
  //
  int xLow;
  int xHigh;
  int yLow;
  int yHigh;
  int zLow;
  int zHigh;
  //
  // The actual material parameters within this region
  //
  double relativePermeability;
  double relativePermittivity;
  double conductivity;
  //
  // Linked list pointer to the next one in the list
  //
  Material *next;
};


struct tlmMaterialStruct
{
  //
  // Precalculated constants for use in the TLM method
  //
  double yx;
  double yy;
  double yz;
  //
  double zx;
  double zy;
  double zz;
  //
  // Linked list pointer to the next one in the list
  //
  TlmMaterial *next;
};


void
parseFile(FILE *inputFile,
	  FILE *logFile,
	  int *nx,
	  int *ny,
	  int *nz,
	  int *maximumIteration,
	  double *maximumSimulationTime,
	  Boundary *boundaryList,
	  Boundary *lastBoundary,
	  Excitation *excitationList,
	  Excitation *lastExcitation,
	  Material *materialList,
	  Material *lastMaterial,
	  Output *outputList,
	  Output *lastOutput,
	  double *dx,
	  double *dy,
	  double *dz);

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
	   Output *outputList);
       
