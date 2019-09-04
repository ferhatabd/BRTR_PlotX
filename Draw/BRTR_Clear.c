/********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: BRTR_Clear.c
 * Author: abdullahoglf
 * Created: June 30, 2015
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
	\interface BRTR_Clear
	\brief This FB can be used to clear the drawbox on the screen
	\param[in]	Enable	Enable of the FB
	\param[in]	pSetup	Reference to the BRTR_PlotSetup_typ structure
	\param[in]  Fill	Background color of the drawbox
	\param[in]	Border	Fore color of the drawbox	
	\param[out]	Status  Status of the FB
*/
/* Clears the drawbox */
void BRTR_Clear(struct BRTR_Clear *inst){
	// ---------------------------------
	// check for enable
	if (inst->Enable) { // FB is enabled
		inst->Status = 0xffff;
		// ---------------------------------
		// check for pointer reference
		if (inst->pSetup) { // pointer address is not NULL
			pParSetup = inst->pSetup; // assign the pointer
			if (!VA_Saccess(1, pParSetup->Out.VC.VcHandle)) { // get the access semaphore
				if (!VA_Attach(1, pParSetup->Out.VC.VcHandle, 0, pParSetup->In.VC.DrawboxName)) { // attach the drawbox
					vcStatus = VA_Rect(1, pParSetup->Out.VC.VcHandle, 0, 0, pParSetup->Out.VC.DrawboxPixelsX, pParSetup->Out.VC.DrawboxPixelsY, inst->Fill, inst->Border); 
					if (!vcStatus) { // clear done
						inst->Status 	= 0;
						VA_Detach(1,pParSetup->Out.VC.VcHandle);
					} else if (vcStatus != vaERR_BUSY) {
						inst->Status	= vcStatus;
						VA_Detach(1,pParSetup->Out.VC.VcHandle);
					}
				} // if VA_Attach == 0
				VA_Srelease(1,pParSetup->Out.VC.VcHandle); // release the semaphore
			} // if VA_Saccess == 0
		} else { // null pointer
			inst->Status = BRTR_ERR_NULL_POINTER;
		} // end of null pointer check
	} else { // FB is not enabled
		stepClear    = 0;
		inst->Status = 0xfffe;	
	} // end of FB enable check
} // end of the BRTR_Clear() implementation