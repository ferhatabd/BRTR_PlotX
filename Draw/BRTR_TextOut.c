/*********************************************************************************
 * COPYRIGHT --  Bernecker + Rainer
********************************************************************************** 
* Author: 		abdullahoglf
* Created:		May 07, 2017/02:41
**********************************************************************************
* Description:
*	Outputs the requested text on a drawbox. The requested coordinate is scaled
*	to match the current drawbox coordinates.
* Parameters:
*
* History:
*
**********************************************************************************/




/*************************************************************************
****    	Includes 				           			 			  ****
*************************************************************************/
#include <bur/plctypes.h>
#include "BRTR_PlotX.h"
#include "BRTR_Global.h"
#ifdef __cplusplus
	extern "C"
	{
#endif

/*************************************************************************
****    	Internal Declarations	         		        		  ****
*************************************************************************/
extern	plcbit			initOK;
extern	unsigned long	vcHandle;
extern	unsigned long	pDbName;
extern	unsigned long 	vcSetup;


/*************************************************************************
****    	FB Implemetation		         	         			  ****
*************************************************************************/
/*  */
void BRTR_TextOut(struct BRTR_TextOut* inst)
{
	BRTR_TextOut_Config_typ		*pCfg;
	BRTR_Plot_Setup_typ			*pPar;
	BRTR_Points_typ				points;	// define the points structure for BRTR_CalculatePpints
	unsigned short				stat;	
	if (inst->Execute) {
		// ----------------------------
		// Check the global init and references
		// ----------------------------
		if (!inst	|| !initOK	|| !vcHandle || !pDbName || !vcSetup)
		{
			inst->Status 	= BRTR_ERR_NULL_POINTER;
			return;
		} else if (!inst->Config) {
			inst->Status	= BRTR_ERR_NULL_POINTER;
			return;
		}
		pCfg				= (BRTR_TextOut_Config_typ	*) inst->Config;
		pPar				= (BRTR_Plot_Setup_typ		*) vcSetup;
		inst->Status		= 0xffff;
		// ----------------------------
		
		// ----------------------------
		// Internal control structure
		// ----------------------------
		if (inst->Internal.bStart) { // check if start is commanded
			// Scale the requested points according the actual drawbox 
			stat 			= BRTR_CalculatePoints(1, vcSetup, 0, (BRTR_Points_typ *)&points, pCfg->x, pCfg->y, pCfg->x, pCfg->y);
			if (!stat) { // BRTR_CalculatePoints successful
				if (!VA_Saccess(1,vcHandle)) { // get the access semaphore
					if (!VA_Attach(1,vcHandle,0,pDbName)){
						inst->Internal.Stat 		= VA_Textout(1, vcHandle, pCfg->Font, points.X.p1, points.Y.p1,	pCfg->ForegroundColor, pCfg->BackgroundColor, &pCfg->Text);
						if (!inst->Internal.Stat) {
							inst->Status			= 0;
							inst->Internal.bStart	= 0;
							VA_Detach(1,vcHandle);
						} else { // error occured
							inst->Status			= inst->Internal.Stat;
							inst->Internal.bStart	= 0;
							VA_Detach(1,vcHandle);							
						} // end of VA_Textout()
					} // end of VA_Attach()			
					VA_Srelease(1,vcHandle); // release the semaphore
				} // if(!VA_Saccess()) 	
			} // if (!stat) -> status of the BRTR_CalculatePoints
			else { // Error occured within BRTR_CalculatePoints
				inst->Status = stat;
				return;
			}
		} // if (_Internal.bStart)
		// ----------------------------
	} else {
		inst->Status			= 0xfffe;	
		inst->Internal.bStart	= 0;
	} // if(Execute)
	
	// ----------------------------
	// Check old values
	// ----------------------------
	if (inst->Internal.OldExecute < inst->Execute) {
		inst->Internal.bStart 	= 1;
		inst->Internal.Step		= 0;
	}
	inst->Internal.OldExecute	= inst->Execute;
	// ----------------------------
} /* end of the BRTR_TextOut() implementation */

#ifdef __cplusplus
	};
#endif