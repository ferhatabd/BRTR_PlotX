(********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: BRTR_Plot
 * File: BRTR_Plot.fun
 * Author: abdullahoglf
 * Created: May 27, 2015
 ********************************************************************
 * Functions and function blocks of library BRTR_Plot
 ********************************************************************)

FUNCTION BRTR_InsertBorder : BOOL (*Inserts a border within the drawbox*)
	VAR_INPUT
		inBorder : REAL; (*Border thickness in pixels*)
	END_VAR
END_FUNCTION

{REDUND_UNREPLICABLE} FUNCTION_BLOCK BRTR_TextOut (*Outputs a text to an already initialized drawbox*)
	VAR_INPUT
		Execute : {REDUND_UNREPLICABLE} BOOL; (*Execute input*)
		Config : {REDUND_UNREPLICABLE} UDINT := 0; (*Pointer to BRTR_TextOut_Config_typ type*)
	END_VAR
	VAR_OUTPUT
		Status : {REDUND_UNREPLICABLE} UINT; (*Status of the FB*)
	END_VAR
	VAR
		Internal : {REDUND_UNREPLICABLE} BRTR_TextOut_Internal_typ; (*Internal control strucure*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_UNREPLICABLE} FUNCTION_BLOCK BRTR_ComponentInitialize (*Init the resources needed for the configured plot*)
	VAR_INPUT
		EnableComp : {REDUND_UNREPLICABLE} BOOL; (*FB enable*)
		pSetupHandle : {REDUND_UNREPLICABLE} UDINT; (*Setup structure handle*)
	END_VAR
	VAR_OUTPUT
		BufferAccessHandle : {REDUND_UNREPLICABLE} UDINT; (*Address to the created buffer *)
		Error : {REDUND_UNREPLICABLE} BRTR_ErrorInfo_typ; (*Detailed information about the error occurred*)
		Status : {REDUND_UNREPLICABLE} UINT; (*FB status output*)
	END_VAR
	VAR
		StepComp : {REDUND_UNREPLICABLE} BRTR_ComponentInitialize_enm := BRTR_COMPINIT_VC_INIT; (*Internal FB step*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_UNREPLICABLE} FUNCTION_BLOCK BRTR_Line (*Line drawing*)
	VAR_INPUT
		Enable : {REDUND_UNREPLICABLE} BOOL := FALSE; (*Enable of the FB*)
		pSetup : {REDUND_UNREPLICABLE} UDINT; (*Setup handle [BRTR_PlotSetup_typ]*)
		ZoomArea : {REDUND_UNREPLICABLE} USINT; (*Zoom area (coordinate space based)*)
		StartX : {REDUND_UNREPLICABLE} REAL; (*X coordinate of the start point*)
		StartY : {REDUND_UNREPLICABLE} REAL; (*Y coordinate of the start point*)
		EndX : {REDUND_UNREPLICABLE} REAL; (*X coordinate of the end point*)
		EndY : {REDUND_UNREPLICABLE} REAL; (*Y coordinate of the end point*)
		Color : {REDUND_UNREPLICABLE} USINT; (*Color datapoint for the line to be drawn*)
		Type : {REDUND_UNREPLICABLE} BRTR_LineType_enm := BRTR_RAPID_INTP; (*Type of the line [intended for differentiating between G00 and G01 but for now it should stay 0]*)
	END_VAR
	VAR_OUTPUT
		Status : {REDUND_UNREPLICABLE} UINT; (*Status of the FB*)
	END_VAR
	VAR
		Points : {REDUND_UNREPLICABLE} BRTR_Points_typ; (*Final points (in drawbox coordinates) after necessary scaling and shifting have been carried out*)
		ii : {REDUND_UNREPLICABLE} UDINT; (*Internal iteration variable*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_UNREPLICABLE} FUNCTION_BLOCK BRTR_Arc (*Arc drawing*) (*$GROUP=User*)
	VAR_INPUT
		Enable : {REDUND_UNREPLICABLE} BOOL; (*Enable of the FB*)
		pSetup : {REDUND_UNREPLICABLE} UDINT; (*Reference of the setup structure*)
		ZoomArea : {REDUND_UNREPLICABLE} USINT; (*Requested zoom area*)
		CenterX : {REDUND_UNREPLICABLE} REAL; (*X coordinate of the center of the arc*)
		CenterY : {REDUND_UNREPLICABLE} REAL; (*Y coordinate of the center of the arc*)
		StartX : {REDUND_UNREPLICABLE} REAL; (*X coordinate of the start of the arc*)
		StartY : {REDUND_UNREPLICABLE} REAL; (*Y coordinate of the start of the arc*)
		EndX : {REDUND_UNREPLICABLE} REAL; (*X coordinate of the end of the arc*)
		EndY : {REDUND_UNREPLICABLE} REAL; (*Y coordinate of the end of the arc*)
		Direction : {REDUND_UNREPLICABLE} BRTR_ArcDir_enm; (*Direction of the arc*)
	END_VAR
	VAR_OUTPUT
		Status : {REDUND_UNREPLICABLE} UINT; (*Status of the FB*)
	END_VAR
	VAR
		_internal : {REDUND_UNREPLICABLE} BRTR_Arc_Internal_typ;
		ii : {REDUND_UNREPLICABLE} UINT;
		step : {REDUND_UNREPLICABLE} BRTR_ArcSteps_enm;
		oldEnableArc : {REDUND_UNREPLICABLE} BOOL;
		bExecArc : {REDUND_UNREPLICABLE} BOOL;
		BRTR_Line_priv : {REDUND_UNREPLICABLE} BRTR_Line;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_UNREPLICABLE} FUNCTION_BLOCK BRTR_Clear (*Clears the drawbox area*)
	VAR_INPUT
		Enable : {REDUND_UNREPLICABLE} BOOL; (*Enable of the FB*)
		pSetup : {REDUND_UNREPLICABLE} UDINT; (*Setup handle [BRTR_PlotSetup_typ]*)
		Fill : {REDUND_UNREPLICABLE} USINT; (*Background color of the drawbox*)
		Border : {REDUND_UNREPLICABLE} USINT; (*Border color of the drawbox*)
	END_VAR
	VAR_OUTPUT
		Status : {REDUND_UNREPLICABLE} UINT; (*Status of the FB*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_UNREPLICABLE} FUNCTION_BLOCK BRTR_Rect
	VAR_INPUT
		Execute : {REDUND_UNREPLICABLE} BOOL; (*FB Execute*)
		Config : {REDUND_UNREPLICABLE} UDINT; (*Reference to the rectangle configuration structure*)
	END_VAR
	VAR_OUTPUT
		Status : {REDUND_UNREPLICABLE} UINT; (*Status of the FB*)
	END_VAR
	VAR
		Internal : {REDUND_UNREPLICABLE} BRTR_DrawRect_Internal_typ; (*Internal control structure*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} {REDUND_UNREPLICABLE} FUNCTION_BLOCK BRTR_RectX (*Draws a rectangle with corner radius choice*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Execute : {REDUND_UNREPLICABLE} BOOL;
		Config : {REDUND_UNREPLICABLE} UDINT; (*Reference to the BRTR_RectX_Cfg_typ*)
	END_VAR
	VAR_OUTPUT
		Done : {REDUND_UNREPLICABLE} BOOL;
		Status : {REDUND_UNREPLICABLE} UINT; (*FB status*)
		Error : {REDUND_UNREPLICABLE} BOOL;
	END_VAR
	VAR
		_Internal : {REDUND_UNREPLICABLE} BRTR_RectX_Internal_typ;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} {REDUND_UNREPLICABLE} FUNCTION_BLOCK BRTR_Keyhole (*Draws a keyhole shape*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Execute : {REDUND_UNREPLICABLE} BOOL;
		Config : {REDUND_UNREPLICABLE} UDINT; (*Reference to the BRTR_RectX_Cfg_typ*)
	END_VAR
	VAR_OUTPUT
		Done : {REDUND_UNREPLICABLE} BOOL;
		Status : {REDUND_UNREPLICABLE} UINT; (*FB status*)
		Error : {REDUND_UNREPLICABLE} BOOL;
	END_VAR
	VAR
		_Internal : {REDUND_UNREPLICABLE} BRTR_Keyhole_Internal_typ;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION PlotInvalidate : UINT (*Invalidates the current plot tasks and releases the resources*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
END_FUNCTION
