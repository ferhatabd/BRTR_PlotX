/*********************************************************************************
 * COPYRIGHT --  Bernecker + Rainer
********************************************************************************** 
* Author: 		abdullahoglf
* Created:		May 17, 2017/14:42
**********************************************************************************
* Description:
*	- Draws a rectangle on the given coordinates after scaling them according to the
*		current layout & drawbox limits.
* Parameters:
*
* History:
*
**********************************************************************************/


/*************************************************************************
****    	Includes 				           			  			  ****
*************************************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
#include <bur/plctypes.h>
#include "BRTR_PlotX.h"
#include "BRTR_Global.h"

/*************************************************************************
****    	Internal Declarations	         		        		  ****
*************************************************************************/
extern	plcbit			initOK;
extern	unsigned long	vcHandle;
extern	unsigned long 	vcSetup;
extern	unsigned long	pDbName;



/*************************************************************************
****    	FB Implemetation		         	         			  ****
*************************************************************************/
/* Draws a rectangle on the given coordinates */
void BRTR_Rect(struct BRTR_Rect* inst)
{
	BRTR_DrawRect_Config_typ	*pCfg;
	BRTR_Plot_Setup_typ			*pPar;
	BRTR_Points_typ				rectPoints;
	BRTR_Points_typ				points[2];	// set of 2 points for two lines that define the rectangle 
	unsigned short 				stat;
	
	// ----------------------------
	// Check the execute conditions
	// ----------------------------
	// Instance sanitization
	if (!inst) 
		return;
	
	// Check execute command
	if (inst->Execute) {
		// check library global values
		if (!initOK || !vcHandle || !vcSetup || !pDbName) {
			inst->Status		= BRTR_ERR_NULL_POINTER;
			return;
		}
		
		// If until here evr is ok then assign pointers
		pCfg						= (BRTR_DrawRect_Config_typ	*) inst->Config;
		pPar						= (BRTR_Plot_Setup_typ		*) vcSetup;
		inst->Status				= 0xffff;
		
		// ----------------------------
		// Internal control structure
		// ----------------------------
		if (inst->Internal.bStart) { // check if start is commanded
			// Scale the rectangle to the current draw box 
			stat = BRTR_CalculatePoints(1, vcSetup, 0, (BRTR_Points_typ *)&points[0], pCfg->x1, pCfg->y1, (pCfg->x1 + pCfg->width), pCfg->y1);
			stat = BRTR_CalculatePoints(1, vcSetup, 0, (BRTR_Points_typ *)&points[1], pCfg->x1, pCfg->y1, pCfg->x1, (pCfg->y1 - pCfg->height));			
			if (!stat) {
				// Draw procedure
				if (!VA_Saccess(1,vcHandle)) { // get the access semaphore
					if (!VA_Attach(1,vcHandle,0,pDbName)) { // attach the drawbox
						inst->Internal.Stat	= VA_Rect(1, vcHandle, points[0].X.p1, points[0].Y.p1, points[0].X.d, points[1].Y.d, pCfg->fill, pCfg->border);
						// Check the drawing results
						if (!inst->Internal.Stat) {
							inst->Status			= 0;
							inst->Internal.bStart	= 0;
							VA_Detach(1,vcHandle);
						} else { // error occured
							inst->Status			= inst->Internal.Stat;
							inst->Internal.bStart	= 0;
							VA_Detach(1,vcHandle);							
						} // end of VA_Rect()
					} // if (VA_Attach == 0)
					VA_Srelease(1,vcHandle); // release the semaphore
				} // if (VA_Saccess == 0)	
			} else { // error occured within one of the BRTR_CalculatePoints functions
				inst->Status = stat;
				return;
			} // if (stat == 0)		
		} // if (_Internal.bStart)
		// ----------------------------
	} else { // Execute is false
		inst->Status			= 0xfffe;
		inst->Internal.bStart 	= 0;
	} // if (Execute)
	
	// ----------------------------
	// Check old values
	// ----------------------------
	if (inst->Internal.OldExecute < inst->Execute) {
		inst->Internal.bStart 	= 1;
		inst->Internal.Step		= 0;
	}
	inst->Internal.OldExecute	= inst->Execute;
	// ----------------------------
} /* end of the BRTR_Rect() implementation */

#ifdef __cplusplus
	};
#endif