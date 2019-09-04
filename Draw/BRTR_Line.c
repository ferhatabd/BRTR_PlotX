/********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: BRTR_Line.c
 * Author: abdullahoglf
 * Created: June 11, 2015
 *******************************************************************/

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
	\interface BRTR_Line
	\brief This function block draws a given line on the screen.

	It takes a start and an end point and then draws a line between these points.
	In doing so, it takes care of the limits on the screen, the requested zoom area etc.

	\param[in]		Enable		Enable of the FB
	\param[in]		pSetup		Reference to the BRTR_PlotSetup_typ structure
	\param[in]		ZoomArea	Requested zoom area [1,4]
	\param[in]		StartX		X coordinate of the start point
	\param[in]		StartY		Y coordinate of the start point
	\param[in]		EndX		X coordinate of the end point
	\param[in]		EndY		Y coordinate of the end point
	\param[in]		Color		Requested color code
	\param[in]		Type		Currently not used but can further be used to differentiate between the styles of G00 & G01
	\param[out]		Status		Status of the FB
*/

/* Draw a Line */
void BRTR_Line(struct BRTR_Line *inst){
	if(inst->Enable){ // FB is enabled
		inst->Status = 0xFFFF;
		if (inst->pSetup != 0) { // setup handle is not NULL
			pParSetup    = inst->pSetup; // assign the pointer		
			resultCalc = BRTR_CalculatePoints(1, inst->pSetup, inst->ZoomArea, &(inst->Points), inst->StartX, inst->StartY, inst->EndX, inst->EndY);
			if(resultCalc == 0) { // function completed w/o errors	
				if(!VA_Saccess(1, pParSetup->Out.VC.VcHandle)){
					if(!VA_Attach(1, pParSetup->Out.VC.VcHandle, 0, (UDINT)&(pParSetup->In.VC.DrawboxName))){	
						// Draw the lines			
						result = VA_Line(1, pParSetup->Out.VC.VcHandle, (UINT)inst->Points.X.p1, (UINT)inst->Points.Y.p1, (UINT)inst->Points.X.p2, (UINT)inst->Points.Y.p2, inst->Color);
						if(result == 0){
							VA_Detach(1, pParSetup->Out.VC.VcHandle);
							VA_Srelease(1, pParSetup->Out.VC.VcHandle);
							inst->Status = 0; // FB is done
						} else{ // end of drawing
							inst->Status = result; // return the error code of the VA_Line
						} // end of VA_Line error check							
					} // end of attachment
				} // end of accessing
			} else if (resultCalc == BRTR_WAR_LIMIT_VIOLATION) { // out of the limits; the higher control will skip the drawing of this point
				inst->Status = BRTR_WAR_LIMIT_VIOLATION; 
			} else { // else return the error code
				inst->Status = resultCalc; // error code from BRTR_CalculatePoints()
			} // end of result check of BRTR_CalculatePoints()
		} else { // NULL pointer
			inst->Status = BRTR_ERR_NULL_POINTER;
		} // end of pointer check	
	} else { // FB is not enabled
		inst->Status 	= 0xFFFE;
		inst->ii     	= 0;
		memset(&inst->Points, 0, sizeof(inst->Points));
		inst->StartX	= 0;
		inst->StartY	= 0;
		inst->EndX		= 0;
		inst->EndY		= 0;
	} // end of FB enable check
} // end of the BRTR_Line() implementation