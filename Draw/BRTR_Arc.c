/********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: BRTR_Arc.c
 * Author: abdullahoglf
 * Created: June 30, 2015
 ********************************************************************
 * Implementation of library BRTR_Plot
 ********************************************************************/

#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif

#include "BRTR_PlotX.h"
#include "BRTR_Global.h"
#include "BRTR_Draw.h"

#ifdef __cplusplus
	};
#endif

/*!
	\interface BRTR_Arc
	\brief This function block draws the arc on the configured drawbox. 

	The arc is created on the screen by drawing lines between the points which would naturally be on the circle. 
	It makes basically use of the BRTR_Line() FB from the same library.

	\param[in]	Enable		Enable of the FB
	\param[in]	pSetup		Refernce to the internal setup structure
	\param[in]	ZoomArea	Selected zoom area
	\param[in]	CenterX		X coordinate of the center
	\param[in]	CenterY		Y coordinate of the center
	\param[in]	StartX		X coordinate of the start point of the arc
	\param[in]	StartY		Y coordinate of the start point of the arc
	\param[in]	EndX		X coordinate of the end point of the arc
	\param[in]	Endy		Y coordinate of the end point of the arc
	\param[in]	Dir			Direction of the arc -> CW/CCW
	\param[out] Status		Status of the FB
*/
/* Draws a given arc */
void BRTR_Arc(struct BRTR_Arc* inst)
{
	if (inst->Enable){ // FB is enabled
		if (inst->pSetup) { // reference is not NULL
			if (inst->bExecArc) { // wait for the execute
				
				switch(inst->step) {
					// =========================================================== //
					case ARC_WAIT:	//
					// =========================================================== //
						if (inst->bExecArc) {
							inst->step		= ARC_CFG;							
							inst->Status	= 0xFFFF;
						} break;
					
					// =========================================================== //
					case ARC_CFG:	//
					// =========================================================== //
										
						pParSetup     = inst->pSetup; // assign the pointer
						inst->_internal.r             = GetRadius(inst->CenterX, inst->CenterY, inst->StartX, inst->StartY);
						inst->_internal.qs            = CheckQuadrant(inst->CenterX, inst->CenterY, inst->StartX, inst->StartY);
						inst->_internal.qe			  = CheckQuadrant(inst->CenterX, inst->CenterY, inst->EndX, inst->EndY);
						inst->_internal.angStart      = CalcAngle(inst->CenterX, inst->StartX, inst->_internal.r, inst->_internal.qs);
						inst->_internal.angEnd        = CalcAngle(inst->CenterX, inst->EndX, inst->_internal.r, inst->_internal.qe);
						inst->_internal.angOfRotation = RotationAngle(inst->_internal.angStart, inst->_internal.angEnd, inst->Direction); // calculate the angle of rotation in DEGREES
						if (!inst->_internal.angOfRotation) inst->_internal.angOfRotation = 360;
						if (!inst->_internal.r) {
							inst->BRTR_Line_priv.Enable = FALSE;
							inst->Status          = 0;
							inst->bExecArc              = FALSE;
							return;
						}
						inst->_internal.dtArc         	= (pParSetup->In.Plot.ArcSteps == 0) ? 360 : fabs(inst->_internal.angOfRotation) / pParSetup->In.Plot.ArcSteps; // step angle in DEGREES
						inst->_internal.angCurrent    	= CalcAngle(inst->CenterX, inst->StartX, inst->_internal.r, inst->_internal.qs); // in RADIANS
						inst->_internal.ax1 			= inst->StartX;
						inst->_internal.ay1 			= inst->StartY;
						inst->_internal.ax2 			= NextPoint(inst->CenterX, inst->_internal.ax1, inst->_internal.r, inst->_internal.angCurrent, ToRadian(inst->_internal.dtArc), inst->Direction, 0);
						inst->_internal.ay2 			= NextPoint(inst->CenterY, inst->_internal.ay1, inst->_internal.r, inst->_internal.angCurrent, ToRadian(inst->_internal.dtArc), inst->Direction, 1);
					
						inst->step		= ARC_DRAW;
					
						break;
				
				
					// =========================================================== //
					case ARC_DRAW:	//
					// =========================================================== //
						inst->BRTR_Line_priv.Color    = 0;
						inst->BRTR_Line_priv.pSetup   = inst->pSetup;
						inst->BRTR_Line_priv.StartX   = inst->_internal.ax1;
						inst->BRTR_Line_priv.StartY   = inst->_internal.ay1;
						inst->BRTR_Line_priv.EndX     = inst->_internal.ax2;
						inst->BRTR_Line_priv.EndY     = inst->_internal.ay2;
						inst->BRTR_Line_priv.Type     = BRTR_RAPID_INTP; // not used yet
						inst->BRTR_Line_priv.ZoomArea = inst->ZoomArea;
						inst->BRTR_Line_priv.Enable   = TRUE;
						// ---------------
						// check status
						if (!inst->BRTR_Line_priv.Status) { // FB completed successfully
							inst->_internal.ax1 = inst->_internal.ax2;
							inst->_internal.ay1 = inst->_internal.ay2;
							inst->_internal.angCurrent = (inst->Direction == CW) ? inst->_internal.angCurrent - ToRadian(inst->_internal.dtArc) : inst->_internal.angCurrent + ToRadian(inst->_internal.dtArc);
							inst->_internal.ax2 = NextPoint(inst->CenterX, inst->_internal.ax1, inst->_internal.r, inst->_internal.angCurrent, ToRadian(inst->_internal.dtArc), inst->Direction, 0);
							inst->_internal.ay2 = NextPoint(inst->CenterY, inst->_internal.ay1, inst->_internal.r, inst->_internal.angCurrent, ToRadian(inst->_internal.dtArc), inst->Direction, 1);
							inst->BRTR_Line_priv.Enable	= FALSE;
							inst->step					= ARC_CHECK;
						} else if (inst->BRTR_Line_priv.Status == BRTR_WAR_LIMIT_VIOLATION) { // A given line is outside the limits of the drawing window
							inst->BRTR_Line_priv.Enable = FALSE;
							inst->_internal.ax1 = inst->_internal.ax2;
							inst->_internal.ay1 = inst->_internal.ay2;
							inst->_internal.angCurrent = (inst->Direction == CW) ? inst->_internal.angCurrent - ToRadian(inst->_internal.dtArc) : inst->_internal.angCurrent + ToRadian(inst->_internal.dtArc);
							inst->_internal.ax2 = NextPoint(inst->CenterX, inst->_internal.ax1, inst->_internal.r, inst->_internal.angCurrent, ToRadian(inst->_internal.dtArc), inst->Direction, 0);
							inst->_internal.ay2 = NextPoint(inst->CenterY, inst->_internal.ay1, inst->_internal.r, inst->_internal.angCurrent, ToRadian(inst->_internal.dtArc), inst->Direction, 1);
							inst->step					= ARC_CHECK;
						} else if (inst->BRTR_Line_priv.Status != 0xFFFF && inst->BRTR_Line_priv.Status != 0xFFFE){ // FB has an error
							inst->BRTR_Line_priv.Enable = FALSE;
							inst->Status 		= inst->BRTR_Line_priv.Status;
							inst->bExecArc     	= FALSE;
							inst->step			= ARC_ERROR;
							inst->bExecArc      = FALSE;
						} // end of FB status check	
						break;
					
					// =========================================================== //
					case ARC_CHECK:	//
					// =========================================================== //
						if (++inst->ii == pParSetup->In.Plot.ArcSteps) { // FB completed
							inst->BRTR_Line_priv.Enable = FALSE;
							inst->Status          		= 0;
							inst->bExecArc              = FALSE;
						} else {
							inst->step					= ARC_DRAW;	
						} break;
				} /* switch{} */
				
			} // end of the execution check
		} else { // NULL reference
			inst->Status = BRTR_ERR_NULL_POINTER;
		} // end of the reference check
	} else { // FB is not enabled
		inst->Status		= 0xFFFE;
		inst->ii           	= 0;
		inst->step			= ARC_WAIT;
		inst->bExecArc     	= FALSE;
		inst->BRTR_Line_priv.Enable = FALSE;
		//
		// reset temp variables
		memset(&inst->_internal, 0, sizeof(inst->_internal));
	} // end of FB enable check	
	
	
	BRTR_Line(&inst->BRTR_Line_priv);
	
	// ---------------------------------------
	// check for the enable cmd from the previous cycle
	if (inst->Enable > inst->oldEnableArc){
		inst->Status = 0xFFFF;
		inst->bExecArc     = TRUE;
	} // end of old enable check	
	inst->oldEnableArc = inst->Enable;
} // end of BRTR_Arc() implementation

/* !
	\interface GetRadius
	\brief This function calculates the radius of the current arc
	\param[in]	c1		X coordinate of the center of the arc
	\param[in]	c2		Y coordinate of the center of the arc
	\param[in]	a1		X coordinate of the selected point on the arc
	\param[in]	a2		Y coordinate of the selected point on the arc
	\retval				Radius of the circle
/*
/* Calculate the radius */
REAL GetRadius(REAL c1, REAL c2, REAL a1, REAL a2){
	REAL r = sqrtf(powf((a1-c1),2) + powf((a2-c2),2)); // radius in mm
	return r;
} // end of the GetRadius() implementation

/* !
	\interface ToDegree
	\brief This function converts a given angle from degrees to radians
	\param[in]	angRadians	The angle to be converted to degrees
	\retval					The resulting angle in degrees
*/
/* Convert degrees to radians */
double ToDegree(double angRadians) {
	double result = angRadians * (double) BRTR_RAD_2_DEG;
	return result;
} // end of the implementation of ToDegree()

/* !
	\interface ToRadian
	\brief This function converts a given angle from degrees to radians
	\param[in]	angDegrees	The angle to be converted to radians
/*
/* Convert radians to degrees */
double ToRadian(double angDegrees){
	double result = angDegrees * (double) BRTR_DEG_2_RAD;
	return result;
} // end of the implementation of ToRadian

/*!
	\interface CalcAngle
	\brief This function creates the angle that any line (from the center of the circle to a point on the circle)
	makes with the positive x axis of the base coordinate system.
	\param[in] c1		x or y coordinate of the center
	\param[in] a1		x or y coordinate of the point on the circle (c1 and a1 must be from the same coordinate)
	\param[in] inst->_internal.r		Radius of the circle
	\param[in] q		Actual quadrant of the selected point according to the center
	\retval				Current angle on the arc
*/
/* Calculates the angle on the circle */
double CalcAngle(REAL c1, REAL a1, REAL r, USINT q){													  	
	double result; // result variable
	result = (double)((a1-c1)/r);
	result = acos(result);	
	switch (q)
	{
		case 0: 
		case 1:	
		case 2: result = result; break;
		case 3: 
		case 4: result = ToRadian(360) - result; break;
		default: result = result; break;
	}     
	return result; // return the resulting angle in RADIANS
} // end of the implementation of CalcAngle()

/*!
 	\interface RotationAngle
	\brief This function calculates the rotation angle between the start and end points of an arc
	\param[in] ang1		Angle of the start point
	\param[in] ang2		Angle of the end point
	\param[in] Dir		The direction of the rotation [CW/CCW]
	\retval				Angle between start&end points of the arc
*/
/* Calculate the rotation angle */
double RotationAngle(double ang1, double ang2, BRTR_ArcDir_enm Dir){
	double t; // angle of rotation	 
	switch (Dir)
	{
		case CW: // clockwise rotation
			t = ToDegree(ang1)-ToDegree(ang2);
			break;

		case CCW: // counter clockwise rotation
			t=ToDegree(ang2)-ToDegree(ang1);
			break;
		
		default: // should not happen
			t=0;
			break;
	}

	return (t >= 0) ? t : t+360;	
} // end of the implmentation of RotationAngle()

/*!
	\interface NextPoint
	\brief This function calculates the next point on the arc for x & y coordinates seperately
	\param[in]	c1			X or Y coordinate of the center of the arc
	\param[in]	a1			X or Y coordinate of the current point on the arc -> NOT USED
	\param[in]	inst->_internal.r			Radius of the current arc	
	\param[in]	inst->_internal.angCurrent	Current angle on the arc
	\param[in]	dt			The step between two points on the arc (in radians)
	\param[in]  Dir			Direction of the rotation CW/CCW
	\param[in] 	Mode		Mode=0 calculates the next point in X coordinate; Mode=1 calculates the next point on Y coordinate
	\retval					X or Y coordinate of the next point on the arc
*/

/* Calculate the next point on the arc */
REAL NextPoint(REAL c1, REAL a1, REAL r, double angCurrent, double dt, BRTR_ArcDir_enm Dir, USINT Mode){
	REAL a2; // the resulting coordinate
	double angFinal; // final angle with the step is considered
	angFinal = (Dir==CW) ? (angCurrent - dt) : (angCurrent + dt);
	
	switch (Mode)
	{
		case 0: // calculate the x coordinate
			a2 =  c1 + r*cos((REAL)(angFinal)); 
			break;
	
		case 1: // calculate the y coordinate
			a2 = c1 + r*sin((REAL)(angFinal));
			break;
		
		default: // should not happen
			a2 = 0;
			break;
	}
	return a2;	
} // end of the implementation of NextPoint()

/*!
	\interface CheckQuadrant
	\brief This function checks the quadrant of a given point with respect to the center
	\param[in]	cx		X coordinate of the center
	\param[in]	cy		Y coordinate of the center
	\param[in]	x		X coordinate of the point to be checked
	\param[in]	y		Y coordinate of the point to be checked
	\retval				Quadrant of the current point wrt. the center
*/
/* Quadrant check for a given point on the circle */
USINT CheckQuadrant(REAL cx, REAL cy, REAL x, REAL y){
	USINT q; // the quadrant
	// -----------------------
	// check for the act. quadrant -- taking care only of the arccosine zone -> [0,PI]
	if      ( y>=cy ) q = 1;
	else if ( y<cy  ) q = 3;
	else 			  q = 0;
	return q;		
} // end of the implementation of CheckQuadrant()

