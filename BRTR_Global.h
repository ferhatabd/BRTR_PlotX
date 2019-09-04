/********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: Global.h
 * Author: abdullahoglf
 * Created: June 11, 2015
 *******************************************************************/

/*! \mainpage BRTR_Plot
	\section Introduction
	This library provides two main function blocks (BRTR_Line() & BRTR_Arc()) which can be used for drawing given lines or arcs on the screen.
	Together with BRTR_BufferHandler() FB it is possible to read data from ARNC0 CNC Plot Buffer and then draw G00, G01, G02, G03 functions 
	directly from the GCode.
	The library also provides a zoom feature which can be used to zoom on in one of the four quadrants on the screen.
	The functionality of the library does not depend on the size of the drawbox on the screen or the resolution of the HMI. The size of the drawbox is checked at 
	the beginning and all the scalings afterwards are done accordingly.

	\par Important
		For the library to operate successfully the BRTR_PlotSetup_typ structure must be filled in accordingly and the FBs should be fed with the reference of this structure.\r\n
		The origin of the drawboxes on the VC is normally on the upper left corner. For user friendly operations, the origin is internally shifted to the left bottom corner.
	
	\section Description
		
*/



#include <bur/plctypes.h>
#include "BRTR_PlotX.h"
#include "Math.h"

// ----------------------------
// Function/Function Block definitions
/* Calculates the number of segments required to fulfill the path speed relation */
UINT BRTR_ZoomInit(BOOL Enable, UDINT pSetup, UDINT pLimits, USINT ZoomArea);
UINT BRTR_CalculatePoints(BOOL Enable, UDINT pSetup, USINT ZoomArea, UDINT ppPoints, REAL StartX, REAL StartY, REAL EndX, REAL EndY);
REAL BRTR_LineIntercept(REAL p0, REAL StartX, REAL StartY, REAL EndX, REAL EndY, USINT Mode);
BOOL BRTR_LimitSanitize(BOOL Enable, UDINT ppResults, REAL p0, USINT Mode);
REAL GetRadius(REAL c1, REAL c2, REAL a1, REAL a2);
double ToDegree(double angRadians);
double ToRadian(double angDegrees);
double CalcAngle(REAL c1, REAL a1, REAL r, USINT q);
double RotationAngle(double ang1, double ang2, BRTR_ArcDir_enm Dir);
REAL NextPoint(REAL c1, REAL a1, REAL r, double angCurrent, double dt, BRTR_ArcDir_enm Dir, USINT Mode);
USINT CheckQuadrant(REAL cx, REAL cy, REAL x, REAL y);
REAL Scale(REAL dbMax, REAL layoutMax);
float _rAbs(float in);
// ----------------------------
// auxiliary variables that are used throughout
UINT bfrOldStatus;
BOOL initDecoderFileXL; // first time that the decoder has stopped due to ncFILE_XL
enum
{
	LINE_RAPID = 0,
	LINE_INTERPOLATED = 1,
}; 

#define 	TRUE	1;
#define		FALSE	0;