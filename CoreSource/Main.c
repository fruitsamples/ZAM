/*	9-30-92  � Brigham Stevens	--------------------------	main program for Slim.	Change Drawing.c to effect window contents.*/#include "EventLoop.h"#include "MenuDispatch.h"#include "CoreGlobals.h"#include "ZAM.h"#include "GameSounds.h"Boolean gColorQD;void main(void){		/* Expand the heap and make the Mac go */	MaxApplZone();	InitToolBox(kNumMoreMasters);	UnloadSeg(InitToolBox);	/* Create the menus */	BuildMenuBars();	InstallAppleEvents();	InitGame();		/* Run until Quit is selected */	while(!gDone) {		MainEvent();		IdleGameWindow();	}		FreeSounds();	SendGoodBye();	KillAllXThingTasks();}