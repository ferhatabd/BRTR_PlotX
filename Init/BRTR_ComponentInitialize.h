/********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: BRTR_ComponentInitialize.h
 * Author: abdullahoglf
 * Created: June 01, 2015
 *******************************************************************/

#include <bur/plctypes.h>

// ---------------------------------
// Local variable declarations
BRTR_Plot_Setup_typ* 			    pParSetupComp; // declaration of the pointer to the Parameter Setup structure
BRTR_ComponentInitialize_enm 	    oldStepComp; // declaration of the main case machine inst->Step variable
BOOL 								bFirstInStepComp, bEnableOldComp, bStart, vcReady; // flag which indicates the connection to the VC is ready
UINT    							vcStatusDisp, vcStatusAtt, vcStatusAcc; // status values recieved from various visapi functions/FBs
USINT      				  		    smallStepComp; // inst->Step value for local case machines	
UDINT 								uDatObjIdent; // data object ident