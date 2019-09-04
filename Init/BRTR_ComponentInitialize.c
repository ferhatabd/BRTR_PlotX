/********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: BRTR_ComponentInitialize.c
 * Author: abdullahoglf
 * Created: May 29, 2015
 *******************************************************************/

#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif

#include "BRTR_PlotX.h"

#ifdef __cplusplus
	};
#endif

#include "BRTR_Global.h"
#include "BRTR_ComponentInitialize.h"

plcbit 			initOK;
unsigned long 	vcSetup;
unsigned long	vcHandle;
unsigned long	pDbName;

/*!
	\interface BRTR_ComponentInitialize
	\brief This FB carries out the necessary initializations between the library itself and the VC
	\section Description
	This function block carries out the initialization of two main components
	\par 1. Drawbox Init
		It connects to the drawbox and then recieve information about its size in X and Y coordinates -> basically the pixel information.
	\par 2. Buffer Init
		It intializes the buffer by creating a data object on the target with the configured name and of the configured size.
	\param[in]	EnableComp			Enable of the FB
	\param[in]	pSetupHandle		Reference to the BRTR_PlotSetup_typ structure
	\param[out]	BufferAccessHandle	Reference to the created data object on the target
	\param[out]	Error				Error information structure
	\param[out] Status				Status of the FB
	\retval 	0					Initialization OK
	\retval		52001				Null reference is given at least to one of the pointers
	\retval		65534				FB is not enabled
	\retval		65535				FB is busy
	\retval		xxxxx				Any other value submitted to the status will be an error code from the functions/FBs used internally. In this case consult the AS Help system.
*/
void BRTR_ComponentInitialize(struct BRTR_ComponentInitialize* inst) {
	// -------------------------------------------
	// Check if the FB is enabled
	if (inst->EnableComp) // FB is enabled
	{
		// if address to the setup structure is not empty, assign the pointer and continue
		if (inst->pSetupHandle != 0) {
			pParSetupComp 	= inst->pSetupHandle; // assign the pointer
			vcSetup			= pParSetupComp;
			/* ---------------------------------------------------------------------------- */
			/*					             MAIN CONTROL SEQUENCE						    */
			/* ---------------------------------------------------------------------------- */			
			switch (inst->StepComp)
			{
				// =============================================
				case BRTR_COMPINIT_VC_INIT: // connect to the VC
				// =============================================
					if (bStart)
					{
						inst->Status = ERR_FUB_BUSY;
						if (!vcReady)
						{
							pParSetupComp->Out.VC.VcHandle = VA_Setup(1, pParSetupComp->In.VC.VisuName);
							if (pParSetupComp->Out.VC.VcHandle) // value is recieved
							{
								vcReady = TRUE;
								bStart  = FALSE;
								inst->StepComp   	= BRTR_COMPINIT_DRAWBOX_INIT;
							} // end of vcHandle check
						} // end of vcReady check
						else // vcHandle is already recieved
						{
							// Go to the next inst->StepComp 
							inst->StepComp    		= BRTR_COMPINIT_DRAWBOX_INIT;	
							bStart  				= FALSE;				
							smallStepComp			= 10;
						}
					} break; // end of start check					

				// =============================================
				case BRTR_COMPINIT_DRAWBOX_INIT: // get the drawbox sizes
				// =============================================
					if (!VA_Saccess(1, pParSetupComp->Out.VC.VcHandle)) {
						vcHandle	= pParSetupComp->Out.VC.VcHandle;
					
						if (!VA_Attach(1,vcHandle,0,pParSetupComp->In.VC.DrawboxName)) { // attach the drawbox
							pDbName = (plcstring *) &pParSetupComp->In.VC.DrawboxName;
							
							/* Result value Pixels in X ---------------------------------------------------------------------|	*/
							/* 					                                                                             |	*/
							/* Info type: 1= Pixels in X ---------------------------------------|							 |	*/
							/*															        |							 |  */
							/* VC Handle -----------------------------------| 		  		    |							 |	*/
							/*												|			        |							 |	*/
							/* Enable -------------|						|				    |							 |  */
							if (!VA_GetDisplayInfo(1, pParSetupComp->Out.VC.VcHandle, 1, (UDINT)&(pParSetupComp->Out.VC.DrawboxPixelsX))) {
								/* Result value Pixels in Y ---------------------------------------------------------------------|	*/
								/* 					                                                                             |	*/
								/* Info type: 2= Pixels in Y ---------------------------------------|							 |	*/
								/*															        |							 |  */
								/* VC Handle -----------------------------------| 		  		    |							 |	*/
								/*												|			        |							 |	*/
								/* Enable ------------|							|				    |							 |  */
								if(!VA_GetDisplayInfo(1, pParSetupComp->Out.VC.VcHandle, 2, (UDINT)&(pParSetupComp->Out.VC.DrawboxPixelsY))) {
									VA_Detach(1, pParSetupComp->Out.VC.VcHandle);
									inst->Status	= 0;
									inst->StepComp	= BRTR_COMPINIT_VC_INIT;
									initOK			= 1;
								} // pixels in Y
							} // pixels in X
						} // if (VA_Attach == 0)
						VA_Srelease(1, pParSetupComp->Out.VC.VcHandle);	
					} break;	
			} // end of the main case machine
			/* --------------------------------      END OF THE MAIN CONTROL SEQUENCE      ---------------------------- */    			
			
		} else{ // null pointer
			inst->Status = BRTR_ERR_NULL_POINTER;
		}
		
	} // end of FB enabled 
	else // FB is not enabled
	{
		inst->Status   = ERR_FUB_ENABLE_FALSE;
		bStart 		   = FALSE;
		bEnableOldComp = 0;	
	}
	
	// -----------------------------
	// Control the firstInStep flag
	if (oldStepComp != inst->StepComp)
	{
		bFirstInStepComp = TRUE;
		smallStepComp    = 0;	
	}
	else
		bFirstInStepComp = FALSE;
	
	// -----------------------------
	// Control the start flag
	if (bEnableOldComp < inst->EnableComp)
	{
		bStart = TRUE;
		inst->StepComp   = BRTR_COMPINIT_VC_INIT;
	}
	oldStepComp    = inst->StepComp;
	bEnableOldComp = inst->EnableComp;
} // end of the FB