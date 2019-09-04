/*********************************************************************************
 * COPYRIGHT --  Bernecker + Rainer
********************************************************************************** 
* Author: 		abdullahoglf
* Created:		May 08, 2017/16:08
**********************************************************************************
* Description:
*	Sets the internal "border" variable to the requested value for later use.
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
float border = 0;


/*************************************************************************
****    	FB Implemetation		         	         			  ****
*************************************************************************/
/* Initializes the internal border thickness variable */
plcbit BRTR_InsertBorder(float inBorder)
{
	if (inBorder > 0) {
		border = inBorder;
		return 1;
	} else {
		border = 0;
		return 0;
	}
} /* end of the BRTR_InsertBorder() implementation */

#ifdef __cplusplus
	};
#endif