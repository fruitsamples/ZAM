/*//	GrafUtils////	Graphics utility routines//	By Brigham Stevens//	Apple Computer Developer Technical Support//	� 1992 Apple Computer, Inc.//*/#include "GrafUtils.h"#include "CoreAssertion.h"#include "ZAMProtos.h"#define MAX_SAVEPORT_NEST 100/* if this assertion fails, bump up the maximum stack size */#define CHECK_STACK()  ASSERT((__GStack != 0) && (__GStack < MAX_SAVEPORT_NEST))static GDHandle	 __savgd[MAX_SAVEPORT_NEST];static CGrafPtr	 __savegw[MAX_SAVEPORT_NEST];static GWorldPtr __lockPort[MAX_SAVEPORT_NEST];static short	 __GStack = 1;/*	PreserveGraf - saves the current grafics environment	and sets the new one to the passed in GWorld,	which it locks the pixels on for you	Also returns the PixMapHandle		RestoreGraf - restores the grafics environment saved with PreserveGraf		These calls are nestable.*/PixMapHandle PreserveGraf(GWorldPtr newPort){	PixMapHandle	result;		CHECK_STACK();		GetGWorld(&__savegw[__GStack],&__savgd[__GStack]);	SetGWorld(newPort,nil);		result = GetGWorldPixMap(newPort);	LockPixels(result);	__lockPort[__GStack] = newPort;	++__GStack;		/* bump to empty cell */		return result;	}void RestoreGraf(void){		/* subtract down to most recently used entry */	--__GStack;			CHECK_STACK();	SetGWorld(__savegw[__GStack],__savgd[__GStack]);	UnlockPixels(GetGWorldPixMap(__lockPort[__GStack]));}/*		Creates a GWorld from the picture passed*/GWorldPtr PictureToGWorld(PicHandle	pict, int gdepth){	WindowPtr		saveWindow;	GDHandle		saveDevice;	Rect			gwFrame;	GWorldPtr 		gw;		short			err;		// get size of picture and translate to origin of 0,0	gwFrame = (**pict).picFrame;	OffsetRect( &gwFrame, -gwFrame.left, -gwFrame.top );	err = NewGWorld(&gw ,gdepth,&gwFrame,GetCTable(gdepth),nil,0);	if(err == noErr) {		GetGWorld(&saveWindow,&saveDevice);		SetGWorld(gw,nil);				if(LockPixels (GetGWorldPixMap (gw)))		{			EraseRect(&gwFrame);			DrawPicture(pict,&gwFrame);			UnlockPixels(GetGWorldPixMap(gw));		}	}	SetGWorld(saveWindow,saveDevice);	return gw;}void MyUnionRect(Rect *r1, Rect *r2, Rect *dest)/*	If one of the passed rects is empty	this one will return the other rect,	instead of making a big damn one from 0,0	I should have re-written this for speed,	but this is alpha, and it was faster to do it this way	in terms of having something to show.  However, 	I would get around to re-writting this in a comercial 	product.*/{	if(EmptyRect(r1)) {		*dest = *r2;	} else if(EmptyRect(r2)) {		*dest = *r1;	} else {		UnionRect(r1,r2,dest);	}}