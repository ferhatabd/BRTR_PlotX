

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

const unsigned char EDGE_MASK[] = {1,2,4,8};

/*************************************************************************
****    	Internal Declarations	         		        		  ****
*************************************************************************/
		void _config(BRTR_RectX_Config_typ* _pConfig, _RectX_Internal_LineCfg_typ* _pLine, _RectX_Internal_ArcCfg_typ* _pArc);
extern	unsigned long 	vcSetup;

/*************************************************************************
****    	FB Implemetation		         	         			  ****
*************************************************************************/

/* */ 
void BRTR_RectX(struct BRTR_RectX* inst) 
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
	BRTR_RectX_Config_typ		*_pConfig;
	BRTR_RectX_Internal_typ		*_pInt;
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
		inst->Done	= 0;
		
		switch (_pInt->Step) {
			// =========================================================== //
			case RECTX_WAIT: //
			// =========================================================== //	
				if (_pInt->bStart) {
					inst->Status	= 0xffff;
					_pInt->Step		= RECTX_CFG;	
				} break;
			
			
			// =========================================================== //
			case RECTX_CFG:	//
			// =========================================================== //
				_config((BRTR_RectX_Config_typ *) _pConfig, (_RectX_Internal_LineCfg_typ *) &_pInt->lineCfg, (_RectX_Internal_ArcCfg_typ *) &_pInt->arcCfg);
				_pInt->edgeIdx		= 0;
				_pInt->Step			= RECTX_DRAW_LINE;
				break;
			
			
			// =========================================================== //
			case RECTX_DRAW_LINE: 	//
			// =========================================================== //
				if (_pInt->bFirstInStep) { // first time in the step
					for (_pInt->ii = _pInt->edgeIdx; _pInt->ii < 4; _pInt->ii++) {
						if ((_pConfig->edgeMask & EDGE_MASK[_pInt->ii]) != 0) {
							break;	
						}
					}
					//
					_pInt->edgeIdx				= _pInt->ii;
					_pInt->fb.line.Enable		= 1;
					_pInt->fb.line.pSetup		= vcSetup;
					_pInt->fb.line.StartX		= _pInt->lineCfg[_pInt->edgeIdx].startX;
					_pInt->fb.line.StartY		= _pInt->lineCfg[_pInt->edgeIdx].startY;
					_pInt->fb.line.EndX			= _pInt->lineCfg[_pInt->edgeIdx].endX;
					_pInt->fb.line.EndY			= _pInt->lineCfg[_pInt->edgeIdx].endY;
				}
				else {	// after the first time in the step

					
					// ----------------------------
					// Check for FB results
					// ----------------------------
					if (_pInt->fb.line.Status == ERR_OK) {
						_pInt->Step				= RECTX_DRAW_ARC;
					} 
					else if (_pInt->fb.line.Status != ERR_FUB_BUSY && _pInt->fb.line.Status != ERR_FUB_ENABLE_FALSE) {
						inst->Status			= _pInt->fb.line.Status;
						inst->Error				= 1;
						_pInt->Step				= RECTX_ERROR;
					}
					// ----------------------------
					
					if (_pInt->Step != RECTX_DRAW_LINE)
						_pInt->fb.line.Enable	= 0;
				} break;
			
			
			// =========================================================== //
			case RECTX_DRAW_ARC:	//
			// =========================================================== //
				if (_pInt->bFirstInStep) { // first time in the step
					_pInt->fb.arc.Enable		= 1;
					_pInt->fb.arc.CenterX		= _pInt->arcCfg[_pInt->edgeIdx].CenterX;
					_pInt->fb.arc.CenterY		= _pInt->arcCfg[_pInt->edgeIdx].CenterY;
					_pInt->fb.arc.StartX		= _pInt->arcCfg[_pInt->edgeIdx].StartX;
					_pInt->fb.arc.StartY		= _pInt->arcCfg[_pInt->edgeIdx].StartY;
					_pInt->fb.arc.EndX			= _pInt->arcCfg[_pInt->edgeIdx].EndX;
					_pInt->fb.arc.EndY			= _pInt->arcCfg[_pInt->edgeIdx].EndY;
					_pInt->fb.arc.Direction		= CW;
					_pInt->fb.arc.pSetup		= vcSetup;
				}
				else {	// after the first time in the step
					// ----------------------------
					// Check FB status
					// ----------------------------
					if (_pInt->fb.arc.Status == ERR_OK) {
						if (++_pInt->edgeIdx <= 3)
							_pInt->Step			= RECTX_DRAW_LINE;
						else {
							inst->Done			= 1;	
							_pInt->Step			= RECTX_WAIT;	
						}
						
					}
					else if (_pInt->fb.arc.Status != ERR_FUB_BUSY && _pInt->fb.arc.Status != ERR_FUB_ENABLE_FALSE) {
						inst->Status			= _pInt->fb.arc.Status;
						inst->Error				= 1;
						_pInt->Step				= RECTX_ERROR;
					}
					// ----------------------------
					
					if (_pInt->Step != RECTX_DRAW_ARC)
						_pInt->fb.arc.Enable	= 0;
				} break;
		}
	}
	else {	// FB is disabled
		memset(&inst->_Internal, 0, sizeof(inst->_Internal));	
		inst->Done		= 0;
		inst->Status	= 0xfffe;
	}
	// ----------------------------
	
	
	// ----------------------------
	// FB calls
	// ----------------------------
	BRTR_Line(&inst->_Internal.fb.line);
	BRTR_Arc(&inst->_Internal.fb.arc);
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

void _config(BRTR_RectX_Config_typ* _pConfig, _RectX_Internal_LineCfg_typ* _pLine, _RectX_Internal_ArcCfg_typ* _pArc) {
	unsigned char idx = 0;
	_PointProto_typ	p1,p2,p3,p4,pp1,pp2,pp3,pp4,o,anchor;
	float w,h,r1,r2,r3,r4;
	_RectX_Internal_LineCfg_typ	_line;
	_RectX_Internal_ArcCfg_typ	_arc;
	//
	// ----------------------------
	// Compute the edge points
	// ----------------------------
	w			= (float) _pConfig->width;
	h			= (float) _pConfig->height;
	r1			= (float) _pConfig->cornerRadius[0];
	r2			= (float) _pConfig->cornerRadius[1];
	r3			= (float) _pConfig->cornerRadius[2];
	r4			= (float) _pConfig->cornerRadius[3];
	//
	o.X			= _pConfig->x1;
	o.Y			= _pConfig->y1;
	//
	anchor.X	= _pConfig->isExternAnchorActive ? _pConfig->externAnchor.X : o.X;
	anchor.Y	= _pConfig->isExternAnchorActive ? _pConfig->externAnchor.Y : o.Y;
	//
	p1.X		= o.X - w/2;
	p1.Y		= o.Y + h/2;
	//
	p2.X		= o.X + w/2;
	p2.Y		= o.Y + h/2;
	//
	p3.X		= o.X + w/2;
	p3.Y		= o.Y - h/2;
	//
	p4.X		= o.X - w/2;
	p4.Y		= o.Y - h/2;
	
	// calculate the points taking the corner radiuses
	pp1.X		= p1.X + r3;
	pp1.Y		= p1.Y;
	//
	pp2.X		= p2.X - r1;
	pp2.Y		= p2.Y;
	//
	pp3.X		= p3.X;
	pp3.Y		= p3.Y - r2;
	//
	pp4.X  		= p4.X + r3;
	pp4.Y		= p4.Y;
	//
	
	// fill in the line config
	if ((_pConfig->edgeMask & EDGE_MASK[idx]) != 0) {
		_line.startX	= (unsigned short) pp1.X;	
		_line.startY	= (unsigned short) pp1.Y;
		_line.endX		= (unsigned short) pp2.X;
		_line.endY		= (unsigned short) pp2.Y;
		//
		_arc.CenterX	= _line.endX;
		_arc.CenterY	= _line.endY - r1;
		_arc.StartX		= _line.endX;
		_arc.StartY		= _line.endY;
		_arc.EndX		= _arc.StartX + r1;
		_arc.EndY		= _arc.StartY - r1;
		//
		_rotate((float *) &_line.startX, (float *) &_line.startY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_line.endX, (float *) &_line.endY, anchor.X, anchor.Y, _pConfig->alpha);
		//
		_rotate((float *) &_arc.StartX, (float *) &_arc.StartY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_arc.EndX, (float *) &_arc.EndY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_arc.CenterX, (float *) &_arc.CenterY, anchor.X, anchor.Y, _pConfig->alpha);
		//
		*_pArc			= _arc;
		*_pLine			= _line;
	}
	idx++; _pLine++; _pArc++;
	
	if ((_pConfig->edgeMask & EDGE_MASK[idx]) != 0) {
		_line.startX	= (unsigned short) p2.X;	
		_line.startY	= (unsigned short) p2.Y - r2;
		_line.endX		= (unsigned short) p2.X;
		_line.endY		= (unsigned short) p3.Y + r2;
		//
		_arc.CenterX	= _line.endX - r2;
		_arc.CenterY	= _line.endY;
		_arc.StartX		= _line.endX;
		_arc.StartY		= _line.endY;
		_arc.EndX		= _arc.StartX - r2;
		_arc.EndY		= _arc.StartY - r2;		
		//
		_rotate((float *) &_line.startX, (float *) &_line.startY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_line.endX, (float *) &_line.endY, anchor.X, anchor.Y, _pConfig->alpha);
		//
		_rotate((float *) &_arc.StartX, (float *) &_arc.StartY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_arc.EndX, (float *) &_arc.EndY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_arc.CenterX, (float *) &_arc.CenterY, anchor.X, anchor.Y, _pConfig->alpha);
		//
		*_pArc			= _arc;
		*_pLine			= _line;
	}
	idx++; _pLine++; _pArc++;
	
	if ((_pConfig->edgeMask & EDGE_MASK[idx]) != 0) {
		_line.startX	= (unsigned short) p3.X - r2;	
		_line.startY	= (unsigned short) p3.Y;
		_line.endX		= (unsigned short) p4.X + r3;
		_line.endY		= (unsigned short) p4.Y;
		//
		_arc.CenterX	= _line.endX;
		_arc.CenterY	= _line.endY + r3;
		_arc.StartX		= _line.endX;
		_arc.StartY		= _line.endY;
		_arc.EndX		= _arc.StartX - r3;
		_arc.EndY		= _arc.StartY + r3;
		//
		_rotate((float *) &_line.startX, (float *) &_line.startY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_line.endX, (float *) &_line.endY, anchor.X, anchor.Y, _pConfig->alpha);
		//
		_rotate((float *) &_arc.StartX, (float *) &_arc.StartY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_arc.EndX, (float *) &_arc.EndY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_arc.CenterX, (float *) &_arc.CenterY, anchor.X, anchor.Y, _pConfig->alpha);
		//
		*_pArc			= _arc;
		*_pLine			= _line;
	}
	idx++; _pLine++; _pArc++;
	
	if ((_pConfig->edgeMask & EDGE_MASK[idx]) != 0) {
		_line.startX	= (unsigned short) p4.X;	
		_line.startY	= (unsigned short) p4.Y + r4;
		_line.endX		= (unsigned short) p1.X;
		_line.endY		= (unsigned short) p1.Y-r4;
		//
		_arc.CenterX	= _line.endX + r4;
		_arc.CenterY	= _line.endY;
		_arc.StartX		= _line.endX;
		_arc.StartY		= _line.endY;
		_arc.EndX		= _arc.StartX + r4;
		_arc.EndY		= _arc.StartY + r4;
		//
		_rotate((float *) &_line.startX, (float *) &_line.startY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_line.endX, (float *) &_line.endY, anchor.X, anchor.Y, _pConfig->alpha);
		//
		_rotate((float *) &_arc.StartX, (float *) &_arc.StartY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_arc.EndX, (float *) &_arc.EndY, anchor.X, anchor.Y, _pConfig->alpha);
		_rotate((float *) &_arc.CenterX, (float *) &_arc.CenterY, anchor.X, anchor.Y, _pConfig->alpha);
		//
		*_pArc			= _arc;
		*_pLine			= _line;
	}
	// ----------------------------
	return;
}
