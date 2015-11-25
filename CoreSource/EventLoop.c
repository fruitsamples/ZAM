/**/#include "CoreGlobals.h"Boolean		gDone;Boolean		gBackgroundFlag;EventRecord gEvent;long GetBackTime(void);void InitEventGlobals(void){	gDone = false;	gBackgroundFlag = false;}void DoCloseWindow(WindowPtr theWindow){	if(TrackGoAway(theWindow,gEvent.where)) {		CloseGameWindow(theWindow,GetWRefCon(theWindow));	}}void DoClickInContent(WindowPtr theWindow){	if(theWindow!=FrontWindow()) {		SelectWindow(theWindow);	} else {		ClickGameWindow(theWindow,GetWRefCon(theWindow));	}}void DoDragWindow(WindowPtr theWindow){	DragWindow(theWindow,gEvent.where,&screenBits.bounds);}void DoGrowWindow(WindowPtr theWindow){	long	newSize;	int		newHeight,newWidth;	Rect	growLimitSizes;		SetPort(theWindow);	InvalRect(&theWindow->portRect);		growLimitSizes.top = 40;			/* min height */	growLimitSizes.bottom = 32767;		/* max height */	growLimitSizes.left = 40; 			/* min width */	growLimitSizes.right = 32767;		/* max width */		newSize = GrowWindow(theWindow,gEvent.where,&growLimitSizes);	newHeight = HiWord(newSize);	newWidth = LoWord(newSize);	SizeWindow(theWindow,newWidth,newHeight,TRUE);}void DoZoom(WindowPtr theWindow, int part){	GrafPtr savePort;		GetPort(&savePort);	SetPort(theWindow);		if(TrackBox(theWindow,gEvent.where,part)) {		ZoomWindow(theWindow,part,true);	}		SetPort(savePort);}void DoMenu(long msel){	int item,menu;	item = msel;	menu = msel >> kLow16Bits;		MenuDispatch(menu, item);		HiliteMenu(0);						/* remove menu title hiliting */}void DoKey(void){	WindowPtr	fWindow;			if((gEvent.modifiers & cmdKey) == FALSE) {		fWindow = FrontWindow();		if( fWindow != nil)			KeyGameWindow(fWindow,GetWRefCon(fWindow));	} else {		AdjustMenus();		DoMenu(MenuKey(gEvent.message & charCodeMask));		}}void DoUpdate(void){	WindowPtr	updateWindow;	GrafPtr		savePort;			GetPort(&savePort);						/* save current port */		updateWindow=(WindowPtr)gEvent.message;	/* get windowPtr from event msg */	SetPort(updateWindow);		BeginUpdate(updateWindow);							UpdateGameWindow(updateWindow,GetWRefCon(updateWindow));		DrawControls(updateWindow);				/* draw any controls in the window */	EndUpdate(updateWindow);			SetPort(savePort);}void ActivateWindow(WindowRecord	*newFrontWindow){	/* This window is now active.  Controls should be enabled, etc. */}void DeactivateWindow(WindowRecord	*newBehindWindow){	/* 		do anyting necessary to deactivate your windows here.		controls should be dimmed, etc.	*/}void DoActivate(void){	if(gEvent.modifiers & activeFlag)		ActivateWindow((WindowRecord *)gEvent.message);	else		DeactivateWindow((WindowRecord *)gEvent.message);}void DoOSEvent(void){	if( (gEvent.message >> kLow24Bits) == suspendResumeMessage)		gBackgroundFlag = !(gEvent.message & resumeFlag);}void DoClick(void){	WindowPtr	theWindow;	short		part;		part = FindWindow(gEvent.where, &theWindow);		switch(part) {		case inDesk:				break;		case inMenuBar:		AdjustMenus();							DoMenu(MenuSelect(gEvent.where));		break;		case inSysWindow:	SystemClick(&gEvent,theWindow);		break;		case inContent:		DoClickInContent(theWindow);		break;		case inDrag:		DoDragWindow(theWindow);		break;		case inGrow:		DoGrowWindow(theWindow);		break;		case inGoAway:		DoCloseWindow(theWindow);		break;		case inZoomIn:		case inZoomOut:		DoZoom(theWindow,part);		break;		default:					break;	}}void MainEvent(void){	if(WaitNextEvent(everyEvent,&gEvent,0,nil)) {		switch(gEvent.what) {			case nullEvent:							break;			case mouseDown:			DoClick();						break;			case mouseUp:		 											break;			case keyDown:			DoKey();							break;			case keyUp:			 											break;			case autoKey:			DoKey();							break;			case updateEvt:			DoUpdate();						break;			case diskEvt:		 											break;			case activateEvt:		DoActivate();						break;			case networkEvt:												break;			case driverEvt:		 											break;			case app1Evt:		 											break;			case app2Evt:		 											break;			case app3Evt:		 											break;			case osEvt:				DoOSEvent();						break;			case kHighLevelEvent:	DoHighLevelEvent(&gEvent);			default:														break;		}	}}