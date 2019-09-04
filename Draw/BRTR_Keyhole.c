

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
#include "BRTR_Draw.h"

#ifdef __cplusplus
	};
#endif

/*************************************************************************
****    	Internal Declarations	         		        		  ****
*************************************************************************/
		void _configKey(BRTR_Keyhole_Config_typ* _pConfig, BRTR_RectX_Config_typ* _pRect, _RectX_Internal_ArcCfg_typ* _pArc);
extern 	unsigned long vcSetup;

/*************************************************************************
****    	FB Implemetation		         	         			  ****
*************************************************************************/

/* */ 
void BRTR_Keyhole(struct BRTR_Keyhole* inst) 
{
	// ----------------------------
	// instance sanitization
	// ----------------------------
	if (!inst)
		return;
	// ----------------------------
	
	// ----------------------------
	// Internal declarations
	// ----------------------------
	BRTR_Keyhole_Config_typ		*_pConfig;
	BRTR_Keyhole_Internal_typ	*_pInt;
	// ----------------------------
	
	
		
	// ----------------------------
	// Check the FB enable
	// ----------------------------
	if (inst->Execute) {
		// check the references
		if (!inst->Config) {
			inst->Error		= 1;
			inst->Status	= BRTR_ERR_NULL_POINTER;
			return;
		}
		
		// assign the pointers
		_pConfig			= (BRTR_RectX_Config_typ	*) 	inst->Config;
		_pInt				= (BRTR_RectX_Internal_typ	*)	&inst->_Internal;
		
		/*************************************************************************
		****		MAIN CONTROL SEQUENCE	    			        		  ****
		*************************************************************************/
		inst->Done			= 0;
		
		switch (_pInt->Step) {
			// =========================================================== //
			case KEYHOLE_WAIT:	//
			// =========================================================== //	
				if (_pInt->bStart) {
					inst->Status	= 0xffff;
					_pInt->Step		= KEYHOLE_CFG;
				} break;
			
			
			// =========================================================== //
			case KEYHOLE_CFG:	//
			// =========================================================== //
				_configKey((BRTR_Keyhole_Config_typ *) _pConfig, (BRTR_RectX_Config_typ *) &_pInt->rectCfg, (_RectX_Internal_ArcCfg_typ *) &_pInt->arcCfg);
				_pInt->Step			= KEYHOLE_DRAW_ARC;
				break;
			
			
			// =========================================================== //
			case KEYHOLE_DRAW_ARC:	//
			// =========================================================== //
				if (_pInt->bFirstInStep) { // first time in the step
					_pInt->fb.arc.CenterX	= _pInt->arcCfg.CenterX;
					_pInt->fb.arc.CenterY	= _pInt->arcCfg.CenterY;
					_pInt->fb.arc.StartX	= _pInt->arcCfg.StartX;
					_pInt->fb.arc.StartY	= _pInt->arcCfg.StartY;
					_pInt->fb.arc.EndX		= _pInt->arcCfg.EndX;
					_pInt->fb.arc.EndY		= _pInt->arcCfg.EndY;
					_pInt->fb.arc.Direction	= _pInt->arcCfg.Direction;
					_pInt->fb.arc.pSetup	= vcSetup;
					_pInt->fb.arc.Enable	= 1;
				}
				else {	// after the first time in the step
					// ----------------------------
					// Check FB status
					// ----------------------------
					if (_pInt->fb.arc.Status == ERR_OK) {
						_pInt->Step				= KEYHOLE_DRAW_RECT;
					}
					else if (_pInt->fb.arc.Status != ERR_FUB_BUSY && _pInt->fb.arc.Status != ERR_FUB_ENABLE_FALSE) {
						inst->Status			= _pInt->fb.arc.Status;
						inst->Error				= 1;
						_pInt->Step				= KEYHOLE_ERROR;
					}
					// ----------------------------
					
					if (_pInt->Step != KEYHOLE_DRAW_ARC)
						_pInt->fb.arc.Enable	= 0;
				} break;
			
			
			// =========================================================== //
			case KEYHOLE_DRAW_RECT:	//
			// =========================================================== //
				if (_pInt->bFirstInStep) { // first time in the step
					_pInt->fb.rect.Config		= &_pInt->rectCfg;
					_pInt->fb.rect.Execute		= 1;
				}
				else {	// after the first time in the step
					// ----------------------------
					// Check the fb status
					// ----------------------------
					if (_pInt->fb.rect.Done) {
						inst->Done				= 1;
						_pInt->Step				= KEYHOLE_WAIT;
					} 
					else if (_pInt->fb.rect.Error) {
						inst->Status			= _pInt->fb.rect.Status;
						inst->Error				= 1;
						_pInt->Step				= KEYHOLE_ERROR;
					}
					// ----------------------------
					
					if (_pInt->Step != KEYHOLE_DRAW_RECT)
						_pInt->fb.rect.Execute	= 0;
				} break;
		}
	}
	else { // fb is disabled
		memset(&inst->_Internal, 0, sizeof(inst->_Internal));
		inst->Done		= 0;
		inst->Status	= 0xfffe;
	}
	
	// ----------------------------
	// FB Calls
	// ----------------------------
	BRTR_Arc(&inst->_Internal.fb.arc);
	BRTR_RectX(&inst->_Internal.fb.rect);
	// ----------------------------
	
	// ----------------------------
	// Old values handling
	// ----------------------------
	inst->_Internal.bFirstInStep	= inst->_Internal.oldStep != inst->_Internal.Step;
	inst->_Internal.oldStep			= inst->_Internal.Step;
	//
	if (inst->Execute > inst->_Internal.OldExecute)
		inst->_Internal.bStart		= 1;
	inst->_Internal.OldExecute		= inst->Execute;
	// ----------------------------
}


void _configKey(BRTR_Keyhole_Config_typ* _pConfig, BRTR_RectX_Config_typ* _pRect, _RectX_Internal_ArcCfg_typ* _pArc) {
	_PointProto_typ	p1,p2,p3,ro;
	float dx,x,y,h,a,w,d,rw,rh;
	//
	x								= (float) _pConfig->x;
	y								= (float) _pConfig->y;
	a								= (float) _pConfig->alpha;
	h								= (float) _pConfig->h;
	d								= (float) _pConfig->d;
	w								= (float) _pConfig->w;
	rh								= (float) _pConfig->h;
	dx								= sqrtf((d/2)*(d/2)-(h/2)*(h/2));
	//
	p1.X							= (float) _pConfig->x;
	p1.Y							= (float) _pConfig->y;
	//
	p2.X							= p1.X - dx;
	p2.Y							= p1.Y + h/2;
	//
	p3.X							= p2.X;
	p3.Y							= p1.Y - h/2;
	//
	ro.X							= p1.X - dx - w/2 - h/4;
	ro.Y							= p1.Y;
	//
	rw								= w+h/2;
	//
	// set the arc config
	_rotate((float *) &p1.X, (float *) &p1.Y, x, y, a);
	_rotate((float *) &p2.X, (float *) &p2.Y, x, y, a);
	_rotate((float *) &p3.X, (float *) &p3.Y, x, y, a);
	_pArc->CenterX					= p1.X;
	_pArc->CenterY					= p1.Y;
	_pArc->StartX					= p2.X;
	_pArc->StartY					= p2.Y;
	_pArc->EndX						= p3.X;
	_pArc->EndY						= p3.Y;
	_pArc->Direction				= CW;
	
	// set the rect config
	_pRect->alpha					= a;
	_pRect->x1						= ro.X;
	_pRect->y1						= ro.Y;
	_pRect->width					= rw;
	_pRect->height					= h;
	_pRect->cornerRadius[2]			= h/2;
	_pRect->cornerRadius[3]			= h/2;
	_pRect->edgeMask				= 13;
	_pRect->isExternAnchorActive	= 1;
	_pRect->externAnchor.X			= p1.X;
	_pRect->externAnchor.Y			= p1.Y;
	return;
}