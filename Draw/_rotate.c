

/*************************************************************************
****    	Includes 				           			 			  ****
*************************************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
#include <bur/plctypes.h>
#include "BRTR_PlotX.h"
#include "BRTR_Global.h"


#ifdef __cplusplus
	};
#endif

/* */
void _rotate(float* x, float* y, float o_x, float o_y, float a) {
	double _x,_y,__x,__y;
	
	if (a==0)
		return;
	
	float _a 	= 0.0174532925 * a;
	
	float _sin 	= sin(_a);
	float _cos 	= cos(_a);
	__x			= (float) (*x);
	__y			= (float) (*y);
	
	//
	_x			= _cos * (__x-o_x) - _sin * (__y-o_y) + o_x;
	_y			= _sin * (__x-o_x) + _cos * (__y-o_y) + o_y;
	//
	*x			= (float) _x;
	*y			= (float) _y;
	return;
}