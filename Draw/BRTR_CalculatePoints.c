/********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: BRTR_ZoomInit.c
 * Author: abdullahoglf
 * Created: June 18, 2015
 *******************************************************************/

#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif

#include "BRTR_PlotX.h"
#include "BRTR_Global.h"
#ifdef __cplusplus
	};
#endif

extern	float	border;
/*!
	\interface BRTR_CalculatePoints
	\brief This function is basically the core connection between the real coordinates and the drawbox coordinates.
	\section Description
	This function has two main duties. 
	\par 1.Scaling
	It scales the user coordinates in a way that it would be fitting to the drawbox on the screen as well as keeping correct ratios at X & Y coordinates.
	\par 2.Zoom Calculations
	In this function is calculated the limits for a requested zoom area. After the window is defined then all the incoming Start & End points are processed
	to see if they are within the window or not. If a given line is 100% outisde of the window it is never drawn.  If it is partly inside of the window
	then the related point (start point, end point or both of them at some occasions) is shifted to be fully inside of the window. \r\n
	In this way when zooming is active, only the necessary lines are drawn and the other ones are skipped.

	\param[in]	Enable		Enable of the function
	\param[in]  pSetup		Reference to the BRTR_PlotSetup_typ structure
	\param[in]  ZoomArea	Actual requested zoom are [1,4]
	\param[in]	ppPoints	Reference to the BRTR_Points_typ structure
	\param[in]	StartX		X coordinate of the start point
	\param[in]	StartY		Y coordinate of the start point
	\param[in]  EndX		X coordinate of the end point
	\param[in]	EndY		Y coordinate of the end point
	\retval 0				Calculation is successful
	\retval 40000			A given point is outside of the current window limits. The higher control will skip drawing it after seeing this result.
	\retval 52001			Null reference is given to at least one of the pointers
	\retval 52003			A faulty calculation occured -> at least one of the resulting points is negative meaning there is something wrong with the parameters of the BRTR_PlotSetup_typ or the GCode itself.
	\retval 65534			Function is not enabled
	\retval xxxxx			For all the other results, check the results of the subsequent functions that are used within BRTR_CalculatePoints.
*/	

/* Calculate and shift (if necessary) the points in terms of drawbox coordinates */
UINT BRTR_CalculatePoints(BOOL Enable, UDINT pSetup, USINT ZoomArea, UDINT ppPoints, REAL StartX, REAL StartY, REAL EndX, REAL EndY){
	BRTR_Plot_Setup_typ*	pParSetup;
	BRTR_DrawboxLimits_typ  DrawboxLim;
	BRTR_Points_typ*        pPoints;
	UINT functionResult;
	REAL scaleFactorX, scaleFactorY, sx1, sx2, sy1, sy2, slope, distX, distY, dx, dy;
	BOOL withinWindowS, withinWindowE, withinWindowSx,withinWindowSy,withinWindowEx,withinWindowEy,interceptX, interceptY;
	REAL interceptX1, interceptX2, interceptY1, interceptY2;
	REAL scaledStartY, scaledEndY;
	// ------------------------------
	// check for enable
	if(Enable) { // function is enabled
		// ---------------------------------
		// check for pointers
		if(pSetup && ppPoints) { // references are not null
			pParSetup = pSetup; 
			pPoints   = ppPoints; // assign the pointers
			distX     = EndX - StartX;
			distY     = EndY - StartY;
			slope     = (distX) ? (distY/distX) : (0);
			functionResult = BRTR_ZoomInit(1, pSetup, &DrawboxLim, ZoomArea); // call the function to calculate the new layout limits with the given zoom detail
			
			if (border < 0)
				border = 0;
			
			if(!functionResult){ // BRTR_ZoomInit is successful
				// --------------------
				// check for the current zoom details and shift the points if necessary	
				if (ZoomArea) { // zoom request to an area is present
					// get the results for limit window checks
					withinWindowSx = BRTR_LimitSanitize(1, &DrawboxLim, StartX, 0); // x coordinate of the start point
					withinWindowSy = BRTR_LimitSanitize(1, &DrawboxLim, StartY, 1); // y coordinate of the start point
					withinWindowEx = BRTR_LimitSanitize(1, &DrawboxLim, EndX, 0);   // x coordinate of the end point
					withinWindowEy = BRTR_LimitSanitize(1, &DrawboxLim, EndY, 1);   // y coordinate of the end point
					withinWindowS  = withinWindowSx && withinWindowSy; // start point is within window
					withinWindowE  = withinWindowEx && withinWindowEy; // end point is within window
					
					if((StartX  < DrawboxLim.eX1 && EndX < DrawboxLim.eX1) ||
						(StartX > DrawboxLim.eX2 && EndX > DrawboxLim.eX2) ||
						(StartY < DrawboxLim.eY1 && EndY < DrawboxLim.eY1) ||
					(StartY > DrawboxLim.eY2 && EndY > DrawboxLim.eY2)) { // completely outside the limits
						return BRTR_WAR_LIMIT_VIOLATION;
					} 
					else if (withinWindowS && withinWindowE) { ; } // both are within window; do not change anything  // CASE 1
					else if (withinWindowS && !withinWindowE) { // start point is, end point is not within the window // CASE 2
						if(EndY <= StartY && StartX > EndX) { // the case when the end point is above (acc. to the drawbox coordinates)and to the left of the start point
							interceptY2 = BRTR_LineIntercept(DrawboxLim.eX1, StartX, StartY, EndX, EndY, 1); // calculate the Y interception point
							interceptY  = BRTR_LimitSanitize(1, &DrawboxLim, interceptY2, 1); // check if the interception point of Y is within the window
							if(interceptY){ // interception point is within the limit window
								EndX = DrawboxLim.eX1;
								EndY = interceptY2;
							}else { // interception point is outside of the limits; intercept the line with the Y coordinate window
								EndX = BRTR_LineIntercept(DrawboxLim.eY1, StartX, StartY, EndX, EndY, 0); // solve the line eqn for the X interception
								EndY = DrawboxLim.eY1;							
							}
						} // end of case 2.1
						else if (EndY <= StartY && StartX == EndX) { // end point is directly above the start point
							EndX = EndX; // x coor. doesnt change
							EndY = DrawboxLim.eY1;
						} // end of case 2.2
						else if (EndY <= StartY && StartX < EndX){ // end point to the above and right of the start point
							interceptX2 = BRTR_LineIntercept(DrawboxLim.eY1, StartX, StartY, EndX, EndY, 0); // solve the line interception for X
							interceptX  = BRTR_LimitSanitize(1, &DrawboxLim, interceptX2, 0); // check the limits at x
							if (interceptX){ // interception point is within the window
								EndY = DrawboxLim.eY1;
								EndX = interceptX2;
							} else { // interception point is not within the window; take the interception with Y
								EndY = BRTR_LineIntercept(DrawboxLim.eX2, StartX, StartY, EndX, EndY, 1);
								EndX = DrawboxLim.eX2;
							}							
						} // end of case 2.3
						else if (EndY > StartY && StartX > EndX) { // the end point is to the left below of the start point 	 
							interceptY2 = BRTR_LineIntercept(DrawboxLim.eX1, StartX, StartY, EndX, EndY, 1); // calculate the Y interception point
							interceptY  = BRTR_LimitSanitize(1, &DrawboxLim, interceptY2, 1); // check the interception point of Y is within the window
							if(interceptY){ // is within the window
								EndX = DrawboxLim.eX1;
								EndY = interceptY2;
							} else { // interception point is outside the window; intercept the line with the Y coordinate window
								EndX = BRTR_LineIntercept(DrawboxLim.eY2, StartX, StartY, EndX, EndY, 0);
								EndY = DrawboxLim.eY2;								 
							}								
						} // end of case 2.4
						else if (EndY > StartY && StartX == EndX) { // end point is directly below the start point
							EndX = EndX;
							EndY = DrawboxLim.eY2;
						} // end of case 2.5
						else if (EndY > StartY && StartX < EndX) { // the end point is below and to the right  of the start point
							interceptY2 = BRTR_LineIntercept(DrawboxLim.eX2, StartX, StartY, EndX, EndY, 1); // find the Y interception point
							interceptY  = BRTR_LimitSanitize(1, &DrawboxLim, interceptY2, 1); // check if the point is within the window
							if (interceptY) { // is within the window
								EndX = DrawboxLim.eX2;
								EndY = interceptY2;
							} else { // is not within the window; find the interception with X
								EndX = BRTR_LineIntercept(DrawboxLim.eY2, StartX, StartY, EndX, EndY, 0);
								EndY = DrawboxLim.eY2;
							}
						} // end of case 2.6
					} // end of CASE 2
					else if (!withinWindowS && withinWindowE) { // start point is outside end point is inside the window -- CASE 3
						if(EndY <= StartY && StartX > EndX) { // the case when the end point is above (acc. to the drawbox coordinates)and to the left of the start point
							interceptY1 = BRTR_LineIntercept(DrawboxLim.eX2, StartX, StartY, EndX, EndY, 1); // calculate the Y interception point
							interceptY  = BRTR_LimitSanitize(1, &DrawboxLim, interceptY1, 1); // check if the interception point of Y is within the window
							if(interceptY){ // interception point is within the limit window
								StartX = DrawboxLim.eX2;
								StartY = interceptY1;
							}else { // interception point is outside of the limits; intercept the line with the Y coordinate window
								StartX = BRTR_LineIntercept(DrawboxLim.eY2, StartX, StartY, EndX, EndY, 0); // solve the line eqn for the X interception
								StartY = DrawboxLim.eY2;							
							}
						} // end of case 3.1
						else if (EndY <= StartY && StartX == EndX) { // end point is directly above the start point
							StartX = EndX; // x coor. doesnt change
							StartY = DrawboxLim.eY2;
						} // end of case 3.2
						else if (EndY <= StartY && StartX < EndX){ // end point to the above and right of the start point
							interceptX1 = BRTR_LineIntercept(DrawboxLim.eY2, StartX, StartY, EndX, EndY, 0); // solve the line interception for X
							interceptX  = BRTR_LimitSanitize(1, &DrawboxLim, interceptX1, 0); // check the limits at x
							if (interceptX){ // interception point is within the window
								StartY = DrawboxLim.eY2;
								StartX = interceptX1;
							} else { // interception point is not within the window; take the interception with Y
								StartY = BRTR_LineIntercept(DrawboxLim.eX1, StartX, StartY, EndX, EndY, 1);
								StartX = DrawboxLim.eX1;
							}							
						} // end of case 3.3
						else if (EndY > StartY && StartX > EndX) { // the end point is to the left below of the start point 	 
							interceptY1 = BRTR_LineIntercept(DrawboxLim.eX2, StartX, StartY, EndX, EndY, 1); // calculate the Y interception point
							interceptY  = BRTR_LimitSanitize(1, &DrawboxLim, interceptY1, 1); // check the interception point of Y is within the window
							if(interceptY){ // is within the window
								StartX = DrawboxLim.eX2;
								StartY = interceptY1;
							} else { // interception point is outside the window; intercept the line with the Y coordinate window
								StartX = BRTR_LineIntercept(DrawboxLim.eY1, StartX, StartY, EndX, EndY, 0);
								StartY = DrawboxLim.eY1;								 
							}								
						} // end of case 3.4
						else if (EndY > StartY && StartX == EndX) { // end point is directly below the start point
							StartX = StartX;
							StartY = DrawboxLim.eY1;
						} // end of case 3.5
						else if (EndY > StartY && StartX < EndX) { // the end point is below and to the right  of the start point
							interceptY1 = BRTR_LineIntercept(DrawboxLim.eX1, StartX, StartY, EndX, EndY, 1); // find the Y interception point
							interceptY  = BRTR_LimitSanitize(1, &DrawboxLim, interceptY1, 1); // check if the point is within the window
							if (interceptY) { // is within the window
								StartX = DrawboxLim.eX1;
								StartY = interceptY1;
							} else { // is not within the window; find the interception with X
								StartX = BRTR_LineIntercept(DrawboxLim.eY1, StartX, StartY, EndX, EndY, 0);
								StartY = DrawboxLim.eY1;
							}
						} // end of case 3.6
					} // end of CASE 3
					else if ((withinWindowSx ^ withinWindowSy) && (withinWindowEx ^ withinWindowEy)) { // both start & end points are partly in window so there is a possibility that the line is crosscutting the window -- CASE 4
						
						switch (ZoomArea)
						{
							case 1:
								if(withinWindowSy && withinWindowEx){
									interceptY1 = BRTR_LineIntercept(DrawboxLim.eX1, StartX, StartY, EndX, EndY, 1);
									interceptY  = BRTR_LimitSanitize(1, &DrawboxLim, interceptY1, 1);
									if(interceptY){	// a cross cut is possible									
										EndX   = BRTR_LineIntercept(DrawboxLim.eY1, StartX, StartY, EndX, EndY, 0);
										EndY   = DrawboxLim.eY1;
										StartX = DrawboxLim.eX1;
										StartY = interceptY1;
									} else { // the line would be outside the window
										return BRTR_WAR_LIMIT_VIOLATION;
									}
								} else if (withinWindowSx && withinWindowEy){
									interceptX1 = BRTR_LineIntercept(DrawboxLim.eY1, StartX, StartY, EndX, EndY, 0);
									interceptX  = BRTR_LimitSanitize(1, &DrawboxLim, interceptX1, 0);
									if (interceptX) {
										EndY   = BRTR_LineIntercept(DrawboxLim.eX1, StartX, StartY, EndX, EndY, 1);
										EndX   = DrawboxLim.eX1;
										StartX = interceptX1;
										StartY = DrawboxLim.eY1;
									} else {
										return BRTR_WAR_LIMIT_VIOLATION;
									}
								}break;	 
							
							case 2:
								if(withinWindowSy && withinWindowEx){
									interceptY1 = BRTR_LineIntercept(DrawboxLim.eX2, StartX, StartY, EndX, EndY, 1);
									interceptY  = BRTR_LimitSanitize(1, &DrawboxLim, interceptY1, 1);
									if (interceptY){
										EndX   = BRTR_LineIntercept(DrawboxLim.eY1, StartX, StartY, EndX, EndY, 0);
										EndY   = DrawboxLim.eY1;
										StartX = DrawboxLim.eX2;
										StartY = interceptY1;
									} else {
										return BRTR_WAR_LIMIT_VIOLATION;
									}
								} else if (withinWindowSx && withinWindowEy){
									interceptX1 = BRTR_LineIntercept(DrawboxLim.eY1, StartX, StartY, EndX, EndY, 0);
									interceptX  = BRTR_LimitSanitize(1, &DrawboxLim, interceptX1, 0);
									if (interceptX){
										EndY    = BRTR_LineIntercept(DrawboxLim.eX2, StartX, StartY, EndX, EndY, 1);
										EndX	= DrawboxLim.eX2;
										StartX  = interceptX1;
										StartY  = DrawboxLim.eY1;
									} else {
										return BRTR_WAR_LIMIT_VIOLATION;	
									}
								} break;
							
							case 3:
								if (withinWindowSx && withinWindowEy)
								{
									interceptX1 = BRTR_LineIntercept(DrawboxLim.eY2, StartX, StartY, EndX, EndY, 0);
									interceptX  = BRTR_LimitSanitize(1, &DrawboxLim, interceptX1, 0);
									if (interceptX){									
										EndY    = BRTR_LineIntercept(DrawboxLim.eX2, StartX, StartY, EndX, EndY, 1);
										EndX    = DrawboxLim.eX2;
										StartX  = interceptX1;
										StartY  = DrawboxLim.eY2;
									}else {
										return BRTR_WAR_LIMIT_VIOLATION;
									}
								} else if (withinWindowSy && withinWindowEx) {
									interceptY1 = BRTR_LineIntercept(DrawboxLim.eX2, StartX, StartY, EndX, EndY, 1);
									interceptY  = BRTR_LimitSanitize(1, &DrawboxLim, interceptY1, 1);
									if (interceptY) {
										EndX    = BRTR_LineIntercept(DrawboxLim.eY2, StartX, StartY, EndX, EndY, 0);
										EndY    = DrawboxLim.eY2;
										StartX  = DrawboxLim.eX2,
										StartY  = interceptY1;
									} else {
										return BRTR_WAR_LIMIT_VIOLATION;
									}
								} break;

							case 4:
								if (withinWindowSx && withinWindowEy){
									interceptX1 = BRTR_LineIntercept(DrawboxLim.eY2, StartX, StartY, EndX, EndY, 0);
									interceptX  = BRTR_LimitSanitize(1, &DrawboxLim, interceptX1, 0);
									if (interceptX){
										EndY    = BRTR_LineIntercept(DrawboxLim.eX1, StartX, StartY, EndX, EndY, 1);
										EndX	= DrawboxLim.eX1;
										StartX	= interceptX1;
										StartY	= DrawboxLim.eY2;
									} else {
										return BRTR_WAR_LIMIT_VIOLATION;
									}
								} else if (withinWindowSy && withinWindowEx){								
									interceptY1 = BRTR_LineIntercept(DrawboxLim.eX1, StartX, StartY, EndX, EndY, 1);
									interceptY	= BRTR_LimitSanitize(1, &DrawboxLim, interceptY1, 1);
									if (interceptY) {
										EndX 	= BRTR_LineIntercept(DrawboxLim.eY2, StartX, StartY, EndX, EndY, 0);
										EndY	= DrawboxLim.eY2;
										StartX	= DrawboxLim.eX1;
										StartY	= interceptY1;
									} else {
										return BRTR_WAR_LIMIT_VIOLATION;
									}
								} break;							
						} // end of the zoomed area control case machine     
					} // end of case 4
				} // end of zoomed area check
				
				scaleFactorX = Scale((REAL)pParSetup->Out.VC.DrawboxPixelsX, (DrawboxLim.eX2 - DrawboxLim.eX1)); // calculate the scale factor for X coordinate
				scaleFactorY = Scale((REAL)pParSetup->Out.VC.DrawboxPixelsY, (DrawboxLim.eY2 - DrawboxLim.eY1)); // calculate the scale factor for Y coordinate
				// --------------------
				// calculate the points and write the results
				// Drawbox coordinates always start from 0; so in the case of a zoom the modulus should be calculated
				StartX = (StartX == DrawboxLim.eX2) ? DrawboxLim.eX2 : brmfmod(StartX, (DrawboxLim.eX2 - DrawboxLim.eX1));
				EndX   = (EndX   == DrawboxLim.eX2) ? DrawboxLim.eX2 : brmfmod(EndX,   (DrawboxLim.eX2 - DrawboxLim.eX1));
				StartY = (StartY == DrawboxLim.eY2) ? DrawboxLim.eY2 : brmfmod(StartY, (DrawboxLim.eY2 - DrawboxLim.eY1));
				EndY   = (EndY   == DrawboxLim.eY2) ? DrawboxLim.eY2 : brmfmod(EndY,   (DrawboxLim.eY2 - DrawboxLim.eY1));
				scaledStartY  = StartY * scaleFactorY + border;
				scaledEndY    = EndY   * scaleFactorY + border;
				
				// --------------------
				// carry out the scaling and calculate the points [the results are in terms of drawbox coordinates]
				pPoints->X.p1 = StartX * scaleFactorX + border;
				pPoints->X.p2 = EndX   * scaleFactorX + border;
				pPoints->Y.p1 = pParSetup->Out.VC.DrawboxPixelsY - brmfloor(scaledStartY);				
				pPoints->Y.p2 = pParSetup->Out.VC.DrawboxPixelsY - brmfloor(scaledEndY); 	// substractions from the Y coordinates are done to shift the
				pPoints->X.d  = pPoints->X.p2 - pPoints->X.p1;
				pPoints->Y.d  = pPoints->Y.p2 - pPoints->Y.p1;
				// drawbox Y origin from top to bottom
				// --------------------
				// check the points -- all should be non-negative by now
				if (pPoints->X.p1 >= 0 &&
					pPoints->X.p2 >= 0 &&
					pPoints->Y.p1 >= 0 &&
					pPoints->Y.p2 >= 0)
					return 0;
				else // wrong calculation || wrong parameters
					return BRTR_ERR_WRONG_POINT_CALC;
			} else{ // return the error code from BRTR_ZoomInit
				return functionResult;
			} // end of the function if an error occurred in the BRTR_ZoomInit()
		} else { // null reference
			return BRTR_ERR_NULL_POINTER;
		} // end of reference check			
	} else { // function is not enabled
		return 0xFFFE;
	} // end of function enable check
} // end of BRTR_CalculatePoints() implementation

/*!
	\interface BRTR_ZoomInit
	\brief This function calculates the limits (in user units) of the drawbox.

	\section Description
	Depending on the zoom type the limit window is moved.
	If ZoomArea==0 then the limits are the maximums of the layout.
	If there is a zoom are defined then the window is shifted accordingly.

	\param[in]		Enable		Enable fo the function
	\param[in]		pSetup		Reference to the BRTR_PlotSetup_typ structure
	\param[in]		pLimits		Reference to the BRTR_DrawboxLimits_typ strcuture which will be filled by this function for higher control
	\param[in] 		ZoomArea	Actual requested zoom area
	\retval			0			Calculation successfull
	\retval			52001		Null reference is given to at least oone of the pointers
	\retval			65534		The function is not enabled

/* Initialize the layout limits depending on the ZoomArea */
UINT BRTR_ZoomInit(BOOL Enable, UDINT pSetup, UDINT pLimits, USINT ZoomArea){
	BRTR_Plot_Setup_typ		*pParSetup; // pointer to the setup structure
	BRTR_DrawboxLimits_typ  *pResults;  // pointer to the result structure 
	REAL layoutMaxX, layoutMaxY; 
	// -------------------------------
	// check for enable
	if(Enable){ // function is enabled
		// -------------------------------
		// check for the pointers
		if(pSetup && pLimits){ // references are not NULL
			pParSetup = pSetup;
			pResults  = pLimits; // assign the pointers
			// ---------------------
			// calculate the drawbox limits depending on the zoom area			
			switch (ZoomArea)
			{
				case 0: // No zoom
					pResults->eX1 = 0;
					pResults->eX2 = pParSetup->In.Plot.LayoutX;
					pResults->eY1 = 0;
					pResults->eY2 = pParSetup->In.Plot.LayoutY;
					break;
				
				case 1: // First coordinate area for the zoom
					pResults->eX1 = pParSetup->In.Plot.LayoutX/2;
					pResults->eX2 = pParSetup->In.Plot.LayoutX;
					pResults->eY1 = pParSetup->In.Plot.LayoutY/2;
					pResults->eY2 = pParSetup->In.Plot.LayoutY;
					break;

				case 2: // Second coordinate area for the zoom 
					pResults->eX1 = 0;
					pResults->eX2 = pParSetup->In.Plot.LayoutX/2;
					pResults->eY1 = pParSetup->In.Plot.LayoutY/2;
					pResults->eY2 = pParSetup->In.Plot.LayoutY;
					break;
				
				case 3: // Third coordinate area for the zoom
					pResults->eX1 = 0;
					pResults->eX2 = pParSetup->In.Plot.LayoutX/2;
					pResults->eY1 = 0;
					pResults->eY2 = pParSetup->In.Plot.LayoutY/2;
					break;
				
				case 4: // Forth coordinate area for the zoom
					pResults->eX1 = pParSetup->In.Plot.LayoutX/2;
					pResults->eX2 = pParSetup->In.Plot.LayoutX;
					pResults->eY1 = 0;
					pResults->eY2 = pParSetup->In.Plot.LayoutY/2;
					break;	
				
				default: // Should not happen; treat it as no zoom
					pResults->eX1 = 0;
					pResults->eX2 = pParSetup->In.Plot.LayoutX;
					pResults->eY1 = 0;
					pResults->eY2 = pParSetup->In.Plot.LayoutY;
					break;
			} // end of zoom area check
			return 0;
		} else { // null reference
			return BRTR_ERR_NULL_POINTER;
		} // end of reference check
	} else{ // function is not enabled
		return 0xFFFE; // function is not enabled
	} // end of function enable check
} // end of BRTR_ZoomInit() implementation

/*!
	\interface BRTR_LineIntercept
	\brief This function intercepts two lines
	\section Description
		This function intercepts two lines to find out where a given line would intercep the part of the coordinate system which is of interest.\r\n
		It makes use of the straight line equations.\r\n													
		In Mode=0 it solves the equation for X, therefore the X coordinate of the interception point.\r\n
		In Mode=1 it solves the equation for Y, therefore the Y coordinate of the interception point.\r\n
		Line equation from two known points: (y-y1) = m(x-x1) \r\n
		In Mode=0; p0 should be the x1 in the line equation x=x1 (perpendicular straight line)\r\n
		In Mode=1; p0 should be the y1 in the line equation y=y1 (parallel straight line)\r\n
		Start & End coordinates of the two known points of the line (which will be intercepted   
		by one of the lines above depending on the mode (y=p0 || x=p0) should be given              

	\param[in]	p0		The known coordinate of the interception point
	\param[in]	StartX	X coordinate of the start point of the known line
	\param[in]	StartY	Y coordinate of the start point of the known line
	\param[in]	EndX	X coordinate of the end point of the known line
	\param[in]	EndY	Y coordinate of the end point of the known line
	\param[in]	Mode	Mode of calculation
 */

/* Find the interception point of two lines */
REAL BRTR_LineIntercept(REAL p0, REAL StartX, REAL StartY, REAL EndX, REAL EndY, USINT Mode){
	/* --------------------------------------------------------------------------------------------	*/
	/* This function intercepts two lines to find out where a given line would intercept			*/
	/* the part of the coordinate system which is of interest.										*/
	/* It makes use of the straight line equations.													*/
	/* In Mode=0 it solves the equation for X, therefore the X coordinate of the interception point.*/
	/* In Mode=1 it solves the equation for Y, therefore the Y coordinate of the interception point.*/
	/*																								*/
	/* Line equation from two known points: (y-y1) = m(x-x1) 										*/	
	/* In Mode=0; p0 should be the x1 in the line equation x=x1 (perpendicular straight line)		*/
	/* In Mode=1; p0 should be the y1 in the line equation y=y1 (parallel straight line)			*/
	/* Start & End coordinates of the two known points of the line (which will be intercepted       */
	/* by one of the lines above depending on the mode (y=p0 || x=p0) should be given               */
	/* --------------------------------------------------------------------------------------------	*/
	REAL m = (EndY-StartY)/(EndX-StartX); // slope of the line
	REAL x,y; // points of interception, either one will be returned depending on the Mode	
	// ----------------------------
	// case machine for diff modes	
	switch (Mode)
	{
		case 0: // solve the equation for X
			x = ((p0 - StartY)/m)+StartX;
			return x;
			break;

		case 1: // solve the equation for Y
			y = (m*(p0-StartX))+StartY;
			return y;
			break;
		
		default:
			return 0;
			break;		
	} // end of the case machine for different modes     
} // end of BRTR_LineIntercept()

/*! \interface BRTR_LimitSanitize
	\brief This function checks whether a given point is within the current window limits
	\section Description
		This function takes the limits calculated by the BRTR_ZoomInit and with this it checks if a given points is within the current window.\r\n
		When Mode=0 it checks the limits for the X coordinate and with Mode=1 it does the same for the Y coordinate.
	\param[in]	Enable		Enable of the function
	\param[in]	ppResults	Reference to the BRTR_DrawboxLimits_typ structure
	\param[in]	p0			Point to be checked
	\param[in]	Mode		Mode of comparison
	\retval		0			The given point is outside the current window
	\retval		1			The given point is within the current window	
*/
/* Check if a given point is within the limits of the zoom window */
BOOL BRTR_LimitSanitize(BOOL Enable, UDINT ppResults, REAL p0, USINT Mode){
	BRTR_DrawboxLimits_typ *pResults; // pointer to the drawbox limit structure, which is calculated by BRTR_ZoomInit()
	BOOL temp=1;
	if (Enable) { // check for the enable
		if (ppResults) { // check for the reference
			pResults = ppResults; // assign the pointer
			// ----------------------
			// check for the limits			
			switch(Mode){
				case 0: // check for the X boundaries
					if (p0 < pResults->eX1 || p0 > pResults->eX2)	temp = 0; // x is outside the limits
					break;
				
				case 1: // check for the Y boundaries
					if (p0 < pResults->eY1 || p0 > pResults->eY2)	temp = 0; // y is outside the limits
					break;
				
				default: // Should not happen
					return 0;
					break;
			} // end of the case machine
			return temp;		
		} // end of reference check
	} // end of enable check			
} // end of BRTR_LimitSanitize() implementation


/* Coordinate Scaling  */
REAL Scale(REAL dbMax, REAL layoutMax){
	REAL scaleFactor=1;
	REAL dbMaxWithBorder = dbMax - 2*border;
	scaleFactor = (layoutMax == 0) ? 0 : dbMaxWithBorder/layoutMax; // calculate the scalefactor scaleFactor=0 if layoutMax=0 -> division by 0
	return scaleFactor;
} // end of Scale() implementation