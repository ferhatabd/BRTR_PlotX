/*********************************************************************************
 * Copyright: B&R Industrial Automation GmbH
 * Author:    abdullahoglf
 * Created:   2019/08:21 
 *********************************************************************************/ 

#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
#include "BRTR_PlotX.h"
#ifdef __cplusplus
	};
#endif

extern unsigned long vcHandle;

/* Invalidates the current plot tasks and releases the resources */
unsigned short PlotInvalidate(void)
{
	unsigned short statDetach	= 0xfffff;
	unsigned short statRelease 	= 0xfffff;
	//
	// check the vcHandle
	//
	if (!vcHandle) 
		return BRTR_ERR_NULL_POINTER;
	else { 
		//
		// release the resources
		//
		statDetach = VA_Detach(1,vcHandle);
		
		if (!statDetach || statDetach == vaERR_NO_ACCESS)
		{ 
			statRelease			= VA_Srelease(1,vcHandle);	
		}
		
		return statRelease;
	}
}
