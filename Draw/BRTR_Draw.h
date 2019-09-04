/********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: Draw.h
 * Author: abdullahoglf
 * Created: June 13, 2015
 *******************************************************************/

#include <bur/plctypes.h>

BRTR_Plot_Setup_typ				*pParSetup; // pointer to the global setup structure
BRTR_CalcSegmentsResult_typ 	Segments;   // results of the segment calcluations will be written here
BRTR_Points_typ                 Points;      // start & end points will be written here by the subsequent functions

REAL sX1, sX2, sY1, sY2; 		 			// scaled points in X&Y coordinates
//REAL segX1,segX2,segY1,segY2;				// scaled & segmentated points for the line segments to be drawn
REAL scaleFactorX, scaleFactorY; 			// necessary scale factors	
UINT result, resultCalc, stepClear, vcStatus;
BOOL oldEnableClear;
UDINT ii; // internal iteration variable_priv;
void _rotate(float* x, float* y, float o_x, float o_y, float a);