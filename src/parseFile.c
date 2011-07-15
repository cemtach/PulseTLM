
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
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
   #include <unistd.h>
#endif

#include "PulseTLM.h"


// Define some of "magic numbers" which represent types, fields, etc
#define MAX_LINE_SIZE 1024
// Others defined in PulseTLM.h because they are used across other files.

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
	  double *dz)
{
  //
  // An error string for general otuput of errors and status messages
  //
  char errorString[MAX_LINE_SIZE];
  //
  // Input line.  Each line of the input file must be shorter than this.
  //
  char inputLine[MAX_LINE_SIZE];
  //
  // Input line type as the first space separated string of an input line
  //
  char inputLineType[MAX_LINE_SIZE];
  //
  // Type and field parsing variables
  //
  char type[MAX_LINE_SIZE], field[MAX_LINE_SIZE];
  //
  // The current material as the last and list were passed into this
  // function since they will be usefull in the main code.
  //
  Material *material = NULL;
  //
  // The current boundary as the last and list were passed into this
  // function since they will be usefull in the main code.
  //
  Boundary *boundary = NULL;
  //
  // The current excitation as the last and list were passed into this
  // function since they will be usefull in the main code.
  //
  Excitation *excitation = NULL;
  //
  // The current output as the last and list were passed into this
  // function since they will be usefull in the main code.
  //
  Output *output = NULL;
  //
  // Generic array traversal variables
  //
  int i,j,k,n;
  //
  // Count the number of errors.  If it is greater than zero at the
  // end of the parse, then exit suggesting that the user work over
  // the input file.
  //
  int numberOfErrors = 0;

  unsigned long lineNumber = 0;
  //
  //
  //
  //
  // Suck in each inputline one at a time and parse into the TLM mesh.
  // Note that this requires that mesh control lines appear at the
  // beginning of the input file.
  //
  while(fgets(inputLine, MAX_LINE_SIZE, inputFile) != NULL)
    {
    //
    // Clear the input line type before each parse
    //
    strcpy(inputLineType,"");
    //
    // Increment line # for parse warnings...
    lineNumber++;
    //
    // Pick the input line type out from the input line as the first
    // space separated string
    //
    sscanf(inputLine,"%s", inputLineType);
    if(strcasecmp(inputLineType, "mesh") == 0)
      {
      //
      // The mesh spec line has been located: set nx, ny and nz
      //
      sscanf(inputLine, "%s%d%d%d", inputLineType, nx, ny, nz);
      fprintf(logFile, "mesh %d %d %d\n", *nx, *ny, *nz);
      //
      // Each mesh dimension must be greater than 1 as that specified
      // a planar aspect in that coordinate direction.
      // <FIXME>Is this greater than OR EQUAL TO, or just greater?</FIXME>
      //
      if (*nx < 1)
	{
	sprintf(errorString, "nx must be > 1\n");
	fprintf(logFile, errorString);
	numberOfErrors++;
	}
      if (*ny < 1)
	{
	sprintf(errorString, "ny must be > 1\n");
	fprintf(logFile, errorString);
	numberOfErrors++;
	}
      if (*nz < 1)
	{
	sprintf(errorString, "nz must be > 1\n");
	fprintf(logFile, errorString);
	numberOfErrors++;
	}
      }
    else if(strcasecmp(inputLineType,"maximumIteration")==0)
      {
      //
      // The maximumIteration line has been located: set it
      //
      sscanf(inputLine, "%s%d", inputLineType, maximumIteration);
      fprintf(logFile, "maximumIteration %d\n", *maximumIteration);
      }
    else if(strcasecmp(inputLineType,"maximumSimulationTime")==0)
      {
      //
      // The maximumSimulationTime line has been located: set it
      //
      sscanf(inputLine, "%s%lg", inputLineType, maximumSimulationTime);
      fprintf(logFile, "maximumSimulationTime %lg\n", *maximumSimulationTime);
      }
    else if(strcasecmp(inputLineType,"material") == 0)
      {
      //
      // A media block has been encountred in the input file, add the
      // media line to the current list of media lines.
      //
      if (materialList == NULL)
	{
	//
	// This is the first media line within this input file, so
	// start the list setting material to be the one about to be
	// filled out and lastMaterial as the last in the list.
	//
	materialList = (Material *)malloc(sizeof(Material));
	material = materialList;
	}
      else
	{
	//
	// This is not the first media line, to continue the list at
	// the end setting material to be the one about to be filled
	// out and lastMaterial as the last in the list.
	//
	lastMaterial->next = (Material *)malloc(sizeof(Material));
	material = lastMaterial->next;
	}
      //
      // Truncate the list
      //
      material->next = NULL;
      lastMaterial = material;
      //
      // Parse out the information for this media block
      //
      sscanf(inputLine, "%s%d%d%d%d%d%d%lf%lf%lf",
	     inputLineType,
	     &(material->xLow),
	     &(material->xHigh),
	     &(material->yLow),
	     &(material->yHigh),
	     &(material->zLow),
	     &(material->zHigh),
	     &(material->relativePermeability),
	     &(material->relativePermittivity),
	     &(material->conductivity));
      //
      // Echo it to the logFile for user feedback
      //
      fprintf(logFile, "material %d %d   %d %d   %d %d %lg %lg %lg\n",
	      material->xLow,
	      material->xHigh,
	      material->yLow,
	      material->yHigh,
	      material->zLow,
	      material->zHigh,
	      material->relativePermeability,
	      material->relativePermittivity,
	      material->conductivity);
      }
    else if(strcasecmp(inputLineType,"rtxl") == 0 ||
	    strcasecmp(inputLineType,"rtyl") == 0 ||
	    strcasecmp(inputLineType,"rtzl") == 0 ||
	    strcasecmp(inputLineType,"rtxh") == 0 ||
	    strcasecmp(inputLineType,"rtyh") == 0 ||
	    strcasecmp(inputLineType,"rtzh") == 0)
      {
      //
      // A reflective-transmissive boundary has been encountered in
      // the input file, add to the list of boundary conditions.
      //
      if (boundaryList->type == 0)
	{
	//
	// This is the initial `key' boundary in the overall list
	// simply fill it out since it was allocated by the main code.
	//
	boundary = boundaryList;
	}
      else
	{
	//
	// This is not the first boundary, so allocate and add to the
	// end of the list setting boundary to be the one about to be
	// filled out and lastBoundary as the last in the list.
	//
	lastBoundary->next = (Boundary *)malloc(sizeof(Boundary));
	boundary = lastBoundary->next;
	}
      //
      // Truncate the list
      //
      boundary->next = NULL;
      lastBoundary = boundary;
      //
      // Parse out the reflective transmissive boundary condition
      //
      n = sscanf(inputLine, "%s%d%d%d%d%d%d%lf%lf", 
		 inputLineType, 
		 &(boundary->xLow), 
		 &(boundary->xHigh), 
		 &(boundary->yLow), 
		 &(boundary->yHigh), 
		 &(boundary->zLow), 
		 &(boundary->zHigh), 
		 &(boundary->rho),
		 &(boundary->tau));
      //
      // Fill in tau if the user didn't explicitliy specify it 
      //
      if (n == 8)
	{
	boundary->tau = 1.0 + boundary->rho;
	}
      else if (n < 8)
        {
	sprintf(errorString, "%d is too few arguments for a boundary at line %lu\n", n-1, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      //
      // Determine the int type of the boundary
      //
      if (strcasecmp(inputLineType, "rtxh") == 0)
	{
	boundary->type = RTXH;
	}
      if (strcasecmp(inputLineType, "rtyh") == 0)
	{
	boundary->type = RTYH;
	}
      if (strcasecmp(inputLineType, "rtzh") == 0)
	{
	boundary->type = RTZH;
	}
      if (strcasecmp(inputLineType, "rtxl") == 0)
	{
	boundary->type = RTXL;
	}
      if (strcasecmp(inputLineType, "rtyl") == 0)
	{
	boundary->type = RTYL;
	}
      if (strcasecmp(inputLineType, "rtzl") == 0)
	{
	boundary->type = RTZL;
	}
      //
      // Bounds checking and user warnings...
      if (boundary->xLow < 1)
        {
	sprintf(errorString, "Boundary xLow must be >= 1 at line %lu\n", lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (boundary->yLow < 1)
        {
	sprintf(errorString, "Boundary yLow must be >= 1 at line %lu\n", lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (boundary->zLow < 1)
        {
	sprintf(errorString, "Boundary zLow must be >= 1 at line %lu\n", lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (boundary->xHigh > *nx)
        {
	sprintf(errorString, "Boundary xHigh must be <= %d at line %lu\n", *nx, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (boundary->yHigh > *ny)
        {
	sprintf(errorString, "Boundary yHigh must be <= %d at line %lu\n", *ny, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (boundary->zHigh > *nz)
        {
	sprintf(errorString, "Boundary zHigh must be <= %d at line %lu\n", *nz, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }


      //
      // Echo the boundary to the logFile
      //
      fprintf(logFile,"%s %d %d   %d %d   %d %d   %lg %lg\n",
	      inputLineType,
	      boundary->xLow,
	      boundary->xHigh,
	      boundary->yLow,
	      boundary->yHigh,
	      boundary->zLow,
	      boundary->zHigh,
	      boundary->rho,
	      boundary->tau);
      //
      // Check for obvious errors
      //
      if ( (boundary->rho < -1.0) || (boundary->rho > 1.0) )
	{
	sprintf(errorString, "Flaw in rho/tau of the boundary");
	fprintf(logFile, errorString);
	numberOfErrors++;
	}
      }
    else if(strcasecmp(inputLineType, "dx") == 0)
      {
      //
      // Parse the x spatial delta from the file
      //
      sscanf(inputLine,"%s%lf",
	     inputLineType,
	     dx);
      //
      // Echo the x spatial delta to the logfile
      //
      fprintf(logFile, "dx %lf\n", *dx);
      }
    else if(strcasecmp(inputLineType, "dy") == 0)
      {
      //
      // Parse the y spatial delta from the file
      //
      sscanf(inputLine,"%s%lf",
	     inputLineType,
	     dy);
      //
      // Echo the y spatial delta to the logfile
      //
      fprintf(logFile, "dy %lf\n", *dy);
      }
    else if(strcasecmp(inputLineType, "dz") == 0)
      {
      //
      // Parse the z spatial delta from the file
      //
      sscanf(inputLine,"%s%lf",
	     inputLineType,
	     dz);
      //
      // Echo the z spatial delta to the logfile
      //
      fprintf(logFile, "dz %lf\n", *dz);
      }
    else if(strcasecmp(inputLineType, "excitation") == 0)
      {
      //
      // An excitation has been encountered in the input file, add to the
      // list of excitation conditions.
      //
      if (excitationList->type == 0)
	{
	//
	// This is the initial `key' excitation in the overall list
	// simply fill it out since it was allocated by the main code.
	//
	excitation = excitationList;
	}
      else
	{
	//
	// This is not the first excitation, so allocate and add to the
	// end of the list setting excitation to be the one about to be
	// filled out and lastExcitation as the last in the list.
	//
	lastExcitation->next = (Excitation *)malloc(sizeof(Excitation));
	excitation = lastExcitation->next;
	}
      //
      // Truncate the list
      //
      excitation->next = NULL;
      lastExcitation = excitation;
      //
      // Force initial conditions on the values
      //
      excitation->value1 = 0.0;
      excitation->value2 = 0.0;
      excitation->value3 = 0.0;
      excitation->value4 = 0.0;
      excitation->value5 = 0.0;
      //
      // Parse out the excitation
      //
      n = sscanf(inputLine, "%s%d%d%d%d%d%d%s%s%lf%lf%lf%lf%lf", 
		 inputLineType, 
		 &(excitation->xLow), 
		 &(excitation->xHigh), 
		 &(excitation->yLow), 
		 &(excitation->yHigh), 
		 &(excitation->zLow), 
		 &(excitation->zHigh),
		 field,
		 type,
		 &(excitation->value1),
		 &(excitation->value2),
		 &(excitation->value3),
		 &(excitation->value4),
		 &(excitation->value5));
      //
      // Determine the int type of the field
      //
      if (strcasecmp(field, "ex") == 0)
	{
	excitation->field = EX;
	}
      if (strcasecmp(field, "ey") == 0)
	{
	excitation->field = EY;
	}
      if (strcasecmp(field, "ez") == 0)
	{
	excitation->field = EZ;
	}
      if (strcasecmp(field, "hx") == 0)
	{
	excitation->field = HX;
	}
      if (strcasecmp(field, "hy") == 0)
	{
	excitation->field = HY;
	}
      if (strcasecmp(field, "hz") == 0)
	{
	excitation->field = HZ;
	}
      //
      // Bounds checking and user warnings...
      if (excitation->xLow < 1)
        {
	sprintf(errorString, "Excitation xLow must be >= 1 at line %lu\n", lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (excitation->yLow < 1)
        {
	sprintf(errorString, "Excitation yLow must be >= 1 at line %lu\n", lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (excitation->zLow < 1)
        {
	sprintf(errorString, "Excitation zLow must be >= 1 at line %lu\n", lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (excitation->xHigh > *nx)
        {
	sprintf(errorString, "Excitation xHigh must be <= %d at line %lu\n", *nx, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (excitation->yHigh > *ny)
        {
	sprintf(errorString, "Excitation yHigh must be <= %d at line %lu\n", *ny, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (excitation->zHigh > *nz)
        {
	sprintf(errorString, "Excitation zHigh must be <= %d at line %lu\n", *nz, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }


      //
      // Determine the int type of the excitation type
      //
      if (strcasecmp(type, "impulse") == 0)
	{
	excitation->type = IMPULSE;
	}
      if (strcasecmp(type, "sin") == 0)
	{
	excitation->type = SINUSOID;
	}
      if (strcasecmp(type, "exp1") == 0)
	{
	excitation->type = EXPONENTIAL;
	}
      //
      // Echo out the parsed information to the logFile in a way that
      // confirms the parse was performed correctly.
      //
      fprintf(logFile, "excitation %d %d   %d %d   %d %d ",
	      excitation->xLow,
	      excitation->xHigh,
	      excitation->yLow,
	      excitation->yHigh,
	      excitation->zLow,
	      excitation->zHigh);
      //
      if (excitation->field == EX)
	{
	fprintf(logFile, "ex");
	}
      if (excitation->field == EY)
	{
	fprintf(logFile, "ey");
	}
      if (excitation->field == EZ)
	{
	fprintf(logFile, "ez");
	}
      if (excitation->field == HX)
	{
	fprintf(logFile, "hx");
	}
      if (excitation->field == HY)
	{
	fprintf(logFile, "hy");
	}
      if (excitation->field == HZ)
	{
	fprintf(logFile, "hz");
	}
      //
      if (excitation->type == IMPULSE)
	{
	fprintf(logFile, " impulse %lg", excitation->value1);
	}
      if (excitation->type == SINUSOID)
	{
	fprintf(logFile, " %lg * sin(2.0*M_PI*%lg*(t - %lg)) + %lg",
		excitation->value1,
		excitation->value2,
		excitation->value3,
		excitation->value4);
	}
      if (excitation->type == EXPONENTIAL)
	{
	fprintf(logFile, " %lg * exp( -pow((t - %lg)/%lg, 2.0) ) + %lg",
		excitation->value1,
		excitation->value2,
		excitation->value3,
		excitation->value4);
	}
      fprintf(logFile, "\n");
      }
    else if(strcasecmp(inputLineType, "output") == 0)
      {
      //
      // An output has been encountered in the input file, add to the
      // list of output conditions.
      //
      if (outputList->type == 0)
	{
	//
	// This is the initial `key' output in the overall list
	// simply fill it out since it was allocated by the main code.
	//
	output = outputList;
	}
      else
	{
	//
	// This is not the first output, so allocate and add to the
	// end of the list setting output to be the one about to be
	// filled out and lastOutput as the last in the list.
	//
	lastOutput->next = (Output *)malloc(sizeof(Output));
	output = lastOutput->next;
	}
      //
      // Truncate the list
      //
      output->next = NULL;
      lastOutput = output;
      //
      // Parse out the output
      //
      n = sscanf(inputLine, "%s%d%d%d%d%d%d%s%s%d", 
		 inputLineType, 
		 &(output->xLow), 
		 &(output->xHigh), 
		 &(output->yLow), 
		 &(output->yHigh), 
		 &(output->zLow), 
		 &(output->zHigh),
		 field,
		 type,
		 &(output->modulo));
      //
      // Determine the int type of the field
      //
      if (strcasecmp(field, "ex") == 0)
	{
	output->field = EX;
	}
      if (strcasecmp(field, "ey") == 0)
	{
	output->field = EY;
	}
      if (strcasecmp(field, "ez") == 0)
	{
	output->field = EZ;
	}
      if (strcasecmp(field, "hx") == 0)
	{
	output->field = HX;
	}
      if (strcasecmp(field, "hy") == 0)
	{
	output->field = HY;
	}
      if (strcasecmp(field, "hz") == 0)
	{
	output->field = HZ;
	}
      //
      // Bounds checking and user warnings...
      if (output->xLow < 1)
        {
	sprintf(errorString, "Output xLow must be >= 1 at line %lu\n", lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (output->yLow < 1)
        {
	sprintf(errorString, "Output yLow must be >= 1 at line %lu\n", lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (output->zLow < 1)
        {
	sprintf(errorString, "Output zLow must be >= 1 at line %lu\n", lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (output->xHigh > *nx)
        {
	sprintf(errorString, "Output xHigh must be <= %d at line %lu\n", *nx, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (output->yHigh > *ny)
        {
	sprintf(errorString, "Output yHigh must be <= %d at line %lu\n", *ny, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }
      if (output->zHigh > *nz)
        {
	sprintf(errorString, "Output zHigh must be <= %d at line %lu\n", *nz, lineNumber);
	fprintf(logFile, errorString);
	numberOfErrors++;
        }

      //
      if (strcasecmp(field, "eMagnitude") == 0)
	{
	output->field = E_MAGNITUDE;
	}
      if (strcasecmp(field, "hMagnitude") == 0)
	{
	output->field = H_MAGNITUDE;
	}
      //
      // Determine the int type of the output type
      //
      if (strcasecmp(type, "viz") == 0)
	{
	output->type = VIZ;
	}
      if (strcasecmp(type, "pnm") == 0)
	{
	output->type = PNM;
	}
      if (strcasecmp(type, "mtv") == 0)
	{
	output->type = MTV;
	}
      if (strcasecmp(type, "grace") == 0)
	{
	output->type = GRACE;
	}
      if (strcasecmp(type, "dat") == 0)
	{
	output->type = DAT;
	}
      if (strcasecmp(type, "binary") == 0)
	{
	output->type = BINARY;
	}
      if (strcasecmp(type, "xml") == 0)
	{
	output->type = XML;
	}
      if (strcasecmp(type, "vtu") == 0)
	{
	output->type = VTU;
	}
      //
      // <FIXME>Need to do input checking here.<FIXME>
      //
      }
    else
      {
      //
      // The line is unrecognized but echo it to the logfile so the
      // user knows it was unrecognized.
      //
      fprintf(logFile, "##%s", inputLine);
      }
    
    } // end while
  //
  // Each of the spatial deltas must have been set
  //
  if (*dx <= 0.0)
    {
    sprintf(errorString, "dx must be set > 0.0.  Currently: dx = %lf\n", *dx);
    fprintf(logFile, errorString);
    numberOfErrors++;
    }
  if (*dy <= 0.0)
    {
    sprintf(errorString, "dy must be set > 0.0.  Currently: dy = %lf\n", *dy);
    fprintf(logFile, errorString);
    numberOfErrors++;
    }
  if (*dz <= 0.0)
    {
    sprintf(errorString, "dz must be set > 0.0.  Currently: dz = %lf\n", *dz);
    fprintf(logFile, errorString);
    numberOfErrors++;
    }
  //
  // If neither maximumIteration nor maximumSimulationTime have been
  // set, then this simulation really won't run, so error for the user
  // to fix.
  //
  if ( (*maximumIteration <= 0) && (*maximumSimulationTime <= 0.0) )
    {
    sprintf(errorString, "One of maximumIteration or maximumSimulationTime must be set\n");
    fprintf(logFile, errorString);
    numberOfErrors++;
    }
  //
  // Traverse the outputs and number them uniquely which helps keep
  // separate output requests separate.
  //
  output = outputList;
  n = 0;
  while(output != NULL)
    {
    output->uniqueNumber = n;
    n++;
    output = output->next;
    }
  //
  // Traverse the excitations and number them uniquely which helps
  // keep separate output requests separate.
  //
  excitation = excitationList;
  n = 0;
  while(excitation != NULL)
    {
    excitation->uniqueNumber = n;
    n++;
    excitation = excitation->next;
    }
  //
  // If any errors have occured, exit to allow the user to fix the
  // input file.
  //
  if (numberOfErrors > 0)
    {
    sprintf(errorString, "Errors have occured in parsing: Simulation Stopped.\n");
    fprintf(logFile, errorString);
    fflush(logFile);
    fclose(logFile);
    exit(9);
    }
  //
  // <FIXME>Finish the input file parsing by removing
  // any unnecessary information</FIXME>
  //
}
