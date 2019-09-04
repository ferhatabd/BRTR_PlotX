(********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: BRTR_Plot.typ
 * Author: abdullahoglf
 * Created: May 27, 2015
 ********************************************************************
 * Data types of library BRTR_Plot
 ********************************************************************)

TYPE
	BRTR_ErrorInfo_typ : 	STRUCT 
		Text : ARRAY[0..3]OF STRING[80];
	END_STRUCT;
	BRTR_ToPlot_Type_enm : 
		(
		PLOT_TYPE_NONE := 0, (*Nothing to draw*)
		PLOT_TYPE_LINE := 5, (*A line must be drawn*)
		PLOT_TYPE_ARC := 10, (*An arc must be drawn*)
		PLOT_TYPE_RECT := 15, (*A rectangle must be drawn*)
		PLOT_TYPE_POCKET := 17, (*A rectangle with the specified corner radius and maskse must be drawn*)
		PLOT_TYPE_SLOT := 18,
		PLOT_TYPE_HOLE := 19,
		PLOT_TYPE_KEYHOLE := 20, (*Keyhole - specific for MAC140*)
		PLOT_TYPE_TEXT := 21 (*A text should be output to the drawbox*)
		);
END_TYPE

(*Plot configuration*)

TYPE
	_PlotCfg_Arc_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		Center : {REDUND_UNREPLICABLE} _PointProto_typ;
		Start : {REDUND_UNREPLICABLE} _PointProto_typ;
		End : {REDUND_UNREPLICABLE} _PointProto_typ;
		Color : {REDUND_UNREPLICABLE} USINT;
	END_STRUCT;
	_PlotCfg_Line_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		Start : {REDUND_UNREPLICABLE} _PointProto_typ;
		End : {REDUND_UNREPLICABLE} _PointProto_typ;
		Color : {REDUND_UNREPLICABLE} USINT;
	END_STRUCT;
	_PointProto_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		X : {REDUND_UNREPLICABLE} REAL;
		Y : {REDUND_UNREPLICABLE} REAL;
	END_STRUCT;
	BRTR_PlotCfg_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		Type : {REDUND_UNREPLICABLE} BRTR_ToPlot_Type_enm;
		Text : {REDUND_UNREPLICABLE} BRTR_TextOut_Config_typ;
		Rect : {REDUND_UNREPLICABLE} BRTR_DrawRect_Config_typ;
		Slot : {REDUND_UNREPLICABLE} BRTR_RectX_Config_typ;
		Pocket : {REDUND_UNREPLICABLE} BRTR_RectX_Config_typ;
		Hole : {REDUND_UNREPLICABLE} _PlotCfg_Arc_typ;
		Keyhole : {REDUND_UNREPLICABLE} BRTR_Keyhole_Config_typ;
		Arc : {REDUND_UNREPLICABLE} _PlotCfg_Arc_typ;
		Line : {REDUND_UNREPLICABLE} _PlotCfg_Line_typ;
	END_STRUCT;
END_TYPE

(**)
(*==============================================*)
(*MAIN SETUP STRUCTURE*)

TYPE
	BRTR_Plot_Setup_typ : 	STRUCT 
		In : BRTR_Plot_Setup_In_typ; (*Inputs which the FBs needs to operate*)
		Out : BRTR_Plot_Setup_Out_typ; (*Outputs that will be found by the FBs*)
	END_STRUCT;
END_TYPE

(**)
(*==============================================*)
(*INPUT STRUCTURE*)

TYPE
	BRTR_Plot_Setup_In_typ : 	STRUCT 
		Plot : BRTR_Plot_Setup_In_Plot_typ; (*Plot related parameters that should be given to the FBs*)
		VC : BRTR_Plot_Setup_In_VC_typ; (*Visual Components related parameters that should be given to the FBs*)
		CNC : BRTR_Plot_Setup_In_CNC_typ; (*Parameters related with the CNC structure*)
	END_STRUCT;
	BRTR_Plot_Setup_In_Plot_typ : 	STRUCT 
		LayoutX : REAL;
		LayoutY : REAL;
		MaxX : {REDUND_UNREPLICABLE} REAL;
		MaxY : {REDUND_UNREPLICABLE} REAL;
		ArcSteps : UDINT; (*Total steps in which the arcs should be drawn*)
		BufferName : STRING[11]; (*Name of the buffer that will be created for the plotting process*)
		NoOfEntries : DINT; (*Max number of entries that will be inside the buffer*)
	END_STRUCT;
END_TYPE

(**)
(*VC related inputs*)

TYPE
	BRTR_Plot_Setup_In_VC_typ : 	STRUCT 
		VisuName : STRING[11]; (*Name of the actual Visualization object*)
		DrawboxBackgroundColor : USINT;
		DrawboxName : STRING[250]; (*Full path to the Drawbox including its name*)
	END_STRUCT;
END_TYPE

(**)
(*CNC related inputs*)

TYPE
	BRTR_Plot_Setup_In_CNC_typ : 	STRUCT 
		Feedrate : REAL; (*Feedrate of the current block which is being processed*)
		CycleTime : REAL; (*Cycle time of the TC under which the plot program is running*)
		UnitFactor : UDINT; (*Unitfactor of the program*)
		uHandle : UDINT; (*Address to the current CNC structure*)
	END_STRUCT;
END_TYPE

(**)
(*==============================================*)
(*OUTPUTS STRUCTURE*)

TYPE
	BRTR_Plot_Setup_Out_typ : 	STRUCT 
		Plot : BRTR_Plot_Setup_Out_Plot_typ; (*Plotting process related parameters*)
		VC : BRTR_Plot_Setup_Out_VC_typ; (*Visual Components related parameters that are found by the FBs*)
		CNC : BRTR_Plot_Setup_Out_CNC_typ; (*Parameters related with the CNC structure*)
	END_STRUCT;
END_TYPE

(**)
(*Outputs related to the Plotting process*)

TYPE
	BRTR_Plot_Setup_Out_Plot_typ : 	STRUCT 
		MemBlockHandle : UDINT;
		BufferAccessHandle : UDINT; (*Access to the plot data buffer*)
		BufferSize : UDINT; (*Size of the buffer that is created*)
	END_STRUCT;
END_TYPE

(**)
(*VC related outputs*)

TYPE
	BRTR_Plot_Setup_Out_VC_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		VcHandle : {REDUND_UNREPLICABLE} UDINT; (*Handle to the VC object*)
		DrawboxPixelsX : {REDUND_UNREPLICABLE} UDINT; (*Amount of the pixels the drawbox has on X coordinate*)
		DrawboxPixelsY : {REDUND_UNREPLICABLE} UDINT; (*Amount of the pixels the drawbox has on Y coordinate*)
	END_STRUCT;
END_TYPE

(**)
(*CNC related outputs*)

TYPE
	BRTR_Plot_Setup_Out_CNC_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		not_used : {REDUND_UNREPLICABLE} USINT;
	END_STRUCT;
END_TYPE

(**)
(*=============================================================*)
(*ENUMERATIONS*)
(*=============================================================*)
(**)
(*BRTR_ComponentInitialize ( ) *)

TYPE
	BRTR_ComponentInitialize_enm : 
		(
		BRTR_COMPINIT_VC_INIT := 0,
		BRTR_COMPINIT_DRAWBOX_INIT,
		BRTR_COMPINIT_BUFFER_INIT,
		BRTR_COMPINIT_ERROR
		);
END_TYPE

(**)

TYPE
	BRTR_BufferHandler_enm : 
		(
		BRTR_BFR_HANDLER_WAIT_ENABLE := 0,
		BRTR_BFR_HANDLER_RESET,
		BRTR_BFR_HANDLER_WAIT_START,
		BRTR_BFR_HANDLER_LOAD,
		BRTR_BFR_HANDLER_CNC_SIMU_SW_ON,
		BRTR_BFR_HANDLER_CNC_START,
		BRTR_BFR_HANDLER_CNC_STOP,
		BRTR_BFR_HANDLER_CNC_SIMU_SW_OFF,
		BRTR_BFR_HANDLER_ERROR
		);
END_TYPE

(*BRTR_BufferHandler ( )*)
(**)
(*Type of the line that is drawn*)

TYPE
	BRTR_LineType_enm : 
		(
		BRTR_RAPID_INTP := 0, (*G00*)
		BRTR_LINEAR_INTP (*G01*)
		);
END_TYPE

(**)
(*Direction of the rotation of a given arc*)

TYPE
	BRTR_ArcDir_enm : 
		(
		CW := 0, (*Clockwise*)
		CCW (*Counter clockwise*)
		);
END_TYPE

(**)

TYPE
	BRTR_ArcSteps_enm : 
		(
		ARC_WAIT := 0,
		ARC_CFG,
		ARC_DRAW,
		ARC_CHECK,
		ARC_ERROR := 255
		);
END_TYPE

(**)
(*==============================================================*)
(*INTERNAL USAGE*)
(*==============================================================*)

TYPE
	BRTR_Points_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		X : {REDUND_UNREPLICABLE} BRTR_Points_Details_typ;
		Y : {REDUND_UNREPLICABLE} BRTR_Points_Details_typ;
	END_STRUCT;
	BRTR_Points_Details_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		d : {REDUND_UNREPLICABLE} REAL; (*Length of a step*)
		p1 : {REDUND_UNREPLICABLE} REAL; (*Start point*)
		p2 : {REDUND_UNREPLICABLE} REAL; (*End point*)
	END_STRUCT;
	BRTR_DrawboxLimits_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		eX1 : {REDUND_UNREPLICABLE} REAL; (*Left limit for X-axis*)
		eX2 : {REDUND_UNREPLICABLE} REAL; (*Right limit for X-axis*)
		eY1 : {REDUND_UNREPLICABLE} REAL; (*Top limit for Y-axis*)
		eY2 : {REDUND_UNREPLICABLE} REAL; (*Bottom limit for Y-axis*)
	END_STRUCT;
	BRTR_CalcSegmentsResult_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		dX : {REDUND_UNREPLICABLE} DINT; (*Iteration on X coordinate at each cycle*)
		dY : {REDUND_UNREPLICABLE} DINT; (*Iteration on Y coordinate at each cycle*)
		dXfinal : {REDUND_UNREPLICABLE} DINT;
		dYfinal : {REDUND_UNREPLICABLE} DINT;
		SegmentCounts : {REDUND_UNREPLICABLE} UDINT; (*Total number of segments to be drawn*)
	END_STRUCT;
	BRTR_Arc_Internal_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		angCurrent : {REDUND_UNREPLICABLE} LREAL;
		angStart : {REDUND_UNREPLICABLE} LREAL;
		angEnd : {REDUND_UNREPLICABLE} LREAL;
		dtArc : {REDUND_UNREPLICABLE} LREAL;
		angOfRotation : {REDUND_UNREPLICABLE} LREAL;
		ax1 : {REDUND_UNREPLICABLE} REAL;
		ay1 : {REDUND_UNREPLICABLE} REAL;
		ax2 : {REDUND_UNREPLICABLE} REAL;
		ay2 : {REDUND_UNREPLICABLE} REAL;
		r : {REDUND_UNREPLICABLE} REAL;
		qs : {REDUND_UNREPLICABLE} USINT;
		qe : {REDUND_UNREPLICABLE} USINT;
	END_STRUCT;
END_TYPE

(**)
(*==============================================================*)
(*BRTR_Text types*)
(*==============================================================*)

TYPE
	BRTR_TextOut_Config_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		Font : {REDUND_UNREPLICABLE} UINT; (*Index of the Font to be used (same index from the VC)*)
		x : {REDUND_UNREPLICABLE} UINT; (*x coordinate of the text output*)
		y : {REDUND_UNREPLICABLE} UINT; (*y coordinate of the text output*)
		ForegroundColor : {REDUND_UNREPLICABLE} USINT; (*Color of the text*)
		BackgroundColor : {REDUND_UNREPLICABLE} USINT; (*Color of the background*)
		Text : {REDUND_UNREPLICABLE} STRING[80]; (*Text to be output*)
	END_STRUCT;
	BRTR_TextOut_Internal_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		Stat : {REDUND_UNREPLICABLE} UINT;
		OldExecute : {REDUND_UNREPLICABLE} BOOL;
		Step : {REDUND_UNREPLICABLE} USINT;
		bStart : {REDUND_UNREPLICABLE} BOOL;
	END_STRUCT;
END_TYPE

(**)
(*==============================================================*)
(*BRTR_Rect types*)
(*==============================================================*)

TYPE
	BRTR_DrawRect_Internal_typ : 	STRUCT 
		Stat : {REDUND_UNREPLICABLE} UINT;
		OldExecute : {REDUND_UNREPLICABLE} BOOL;
		Step : {REDUND_UNREPLICABLE} USINT;
		bStart : {REDUND_UNREPLICABLE} BOOL;
	END_STRUCT;
	BRTR_DrawRect_Config_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		x1 : {REDUND_UNREPLICABLE} UINT; (*X coordinate of the upper left corner of the rectangle*)
		y1 : {REDUND_UNREPLICABLE} UINT; (*Y coordinate of the upper left corner of the rectangle*)
		width : {REDUND_UNREPLICABLE} UINT; (*Width of the rectangle*)
		height : {REDUND_UNREPLICABLE} UINT; (*Heigth of the rectangle*)
		fill : {REDUND_UNREPLICABLE} USINT; (*Fill color of the rectangle*)
		border : {REDUND_UNREPLICABLE} USINT; (*Border color of the rectangle*)
	END_STRUCT;
END_TYPE

(**)
(*==============================================================*)
(*BRTR_RectX types*)
(*==============================================================*)

TYPE
	BRTR_RectX_Steps_enm : 
		(
		RECTX_WAIT := 0,
		RECTX_CFG,
		RECTX_DRAW_LINE,
		RECTX_DRAW_ARC,
		RECTX_ERROR := 255
		);
	_RectX_Internal_ArcCfg_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		CenterX : {REDUND_UNREPLICABLE} REAL;
		CenterY : {REDUND_UNREPLICABLE} REAL;
		StartX : {REDUND_UNREPLICABLE} REAL;
		StartY : {REDUND_UNREPLICABLE} REAL;
		EndX : {REDUND_UNREPLICABLE} REAL;
		EndY : {REDUND_UNREPLICABLE} REAL;
		Direction : {REDUND_UNREPLICABLE} BRTR_ArcDir_enm;
	END_STRUCT;
	_RectX_Internal_LineCfg_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		endX : {REDUND_UNREPLICABLE} REAL;
		endY : {REDUND_UNREPLICABLE} REAL;
		startY : {REDUND_UNREPLICABLE} REAL;
		startX : {REDUND_UNREPLICABLE} REAL;
	END_STRUCT;
	_RectX_Internal_FB_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		arc : {REDUND_UNREPLICABLE} BRTR_Arc;
		line : {REDUND_UNREPLICABLE} BRTR_Line;
	END_STRUCT;
	BRTR_RectX_Internal_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		fb : {REDUND_UNREPLICABLE} _RectX_Internal_FB_typ;
		lineCfg : {REDUND_UNREPLICABLE} ARRAY[0..3]OF _RectX_Internal_LineCfg_typ;
		arcCfg : {REDUND_UNREPLICABLE} ARRAY[0..3]OF _RectX_Internal_ArcCfg_typ;
		oldStep : {REDUND_UNREPLICABLE} BRTR_RectX_Steps_enm;
		Step : {REDUND_UNREPLICABLE} BRTR_RectX_Steps_enm;
		drawArc : {REDUND_UNREPLICABLE} BOOL;
		drawLine : {REDUND_UNREPLICABLE} BOOL;
		edgeIdx : {REDUND_UNREPLICABLE} USINT;
		ii : {REDUND_UNREPLICABLE} USINT;
		Stat : {REDUND_UNREPLICABLE} UINT;
		OldExecute : {REDUND_UNREPLICABLE} BOOL;
		bFirstInStep : {REDUND_UNREPLICABLE} BOOL;
		bStart : {REDUND_UNREPLICABLE} BOOL;
	END_STRUCT;
	BRTR_RectX_Config_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		alpha : {REDUND_UNREPLICABLE} REAL; (*Positive rotation of the shape*)
		externAnchor : {REDUND_UNREPLICABLE} _PointProto_typ; (*External anchor position (instead of the shape center) for shape rotations*)
		x1 : {REDUND_UNREPLICABLE} UINT; (*X coordinate of the rect center*)
		y1 : {REDUND_UNREPLICABLE} UINT; (*Y coordinate of the rect center*)
		width : {REDUND_UNREPLICABLE} UINT; (*Width of the rectangle*)
		height : {REDUND_UNREPLICABLE} UINT; (*Heigth of the rectangle*)
		fill : {REDUND_UNREPLICABLE} USINT; (*Fill color of the rectangle*)
		border : {REDUND_UNREPLICABLE} USINT; (*Border color of the rectangle*)
		cornerRadius : {REDUND_UNREPLICABLE} ARRAY[0..3]OF UINT; (*Corner radius*)
		edgeMask : {REDUND_UNREPLICABLE} USINT; (*Mask for corners to be radiused - currently not used*)
		isExternAnchorActive : {REDUND_UNREPLICABLE} BOOL; (*If set the shape rotations will be done with respect to the externAnchor*)
	END_STRUCT;
END_TYPE

(*==============================================================*)
(**)
(*==============================================================*)
(*BRTR_Keyhole types*)
(*==============================================================*)

TYPE
	BRTR_Keyhole_Steps_enm : 
		(
		KEYHOLE_WAIT := 0,
		KEYHOLE_CFG,
		KEYHOLE_DRAW_RECT,
		KEYHOLE_DRAW_ARC,
		KEYHOLE_ERROR := 255
		);
	_Keyhole_Internal_ArcCfg_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		CenterX : {REDUND_UNREPLICABLE} REAL;
		CenterY : {REDUND_UNREPLICABLE} REAL;
		StartX : {REDUND_UNREPLICABLE} REAL;
		StartY : {REDUND_UNREPLICABLE} REAL;
		EndX : {REDUND_UNREPLICABLE} REAL;
		EndY : {REDUND_UNREPLICABLE} REAL;
		Direction : {REDUND_UNREPLICABLE} BRTR_ArcDir_enm;
	END_STRUCT;
	_Keyhole_Internal_FB_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		arc : {REDUND_UNREPLICABLE} BRTR_Arc;
		rect : {REDUND_UNREPLICABLE} BRTR_RectX;
	END_STRUCT;
	BRTR_Keyhole_Internal_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		fb : {REDUND_UNREPLICABLE} _Keyhole_Internal_FB_typ;
		rectCfg : {REDUND_UNREPLICABLE} BRTR_RectX_Config_typ;
		arcCfg : {REDUND_UNREPLICABLE} _RectX_Internal_ArcCfg_typ;
		oldStep : {REDUND_UNREPLICABLE} BRTR_Keyhole_Steps_enm;
		Step : {REDUND_UNREPLICABLE} BRTR_Keyhole_Steps_enm;
		ii : {REDUND_UNREPLICABLE} USINT;
		Stat : {REDUND_UNREPLICABLE} UINT;
		OldExecute : {REDUND_UNREPLICABLE} BOOL;
		bFirstInStep : {REDUND_UNREPLICABLE} BOOL;
		bStart : {REDUND_UNREPLICABLE} BOOL;
	END_STRUCT;
	BRTR_Keyhole_Config_typ : {REDUND_UNREPLICABLE} 	STRUCT 
		alpha : {REDUND_UNREPLICABLE} REAL;
		x : {REDUND_UNREPLICABLE} UINT; (*X coordinate of the keyhole hole center*)
		d : {REDUND_UNREPLICABLE} UINT;
		h : {REDUND_UNREPLICABLE} UINT;
		w : {REDUND_UNREPLICABLE} UINT;
		y : {REDUND_UNREPLICABLE} UINT; (*Y coordinate of the keyhole hole center*)
	END_STRUCT;
END_TYPE

(*==============================================================*)
