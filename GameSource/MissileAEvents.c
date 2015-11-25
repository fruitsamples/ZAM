#include "GameAEvents.h"#include "ZAM.h"#include "WindowDispatch.h"#include "TankSprite.h"#include "MissileSprite.h"#include "MissileAEvents.proto.h"void InstallMissileEvents(gamePtr game){	OSErr	err;		err = AEInstallEventHandler (kZAMEventClass, kFireMissileID, AEFireMissile,(long)game, false);	if(err) {		ErrMsgCode("\pCould not install AE handler.",err);		ExitToShell();	}		err = AEInstallEventHandler (kZAMEventClass, kMoveRemoteMissileID, AEMoveRemoteMissile,(long)game, false);	if(err) {		ErrMsgCode("\pCould not install AE handler.",err);		ExitToShell();	}}	pascal OSErr AEMoveRemoteMissile (AppleEvent *theAE, AppleEvent *reply, long rfCon){	OSErr		err = 0;	long		len;	DescType	actualType;	gamePtr		game;	short		missileIndex;	fixPt		missileLoc;		game = (gamePtr)rfCon;			err = AEGetParamPtr(theAE, keyMissileNum, typeShortInteger, &actualType, 			&missileIndex, sizeof(short), &len);	if(err != noErr) {		ErrMsgCode("\p Failed: AEMoveRemoteMissile keyDir, typeShortInteger",err);	}		if(err == noErr) {		err = AEGetParamPtr(theAE, keyLocation, typefixPt, &actualType, 				&missileLoc, sizeof(short), &len);		if(err != noErr) {			ErrMsgCode("\p Failed: AEMoveRemoteMissile keyLocation, typefixPt",err);		}	}		if(err == noErr) {		RemoteMissileSpriteList[missileIndex]->remoteLoc = missileLoc;		RemoteMissileSpriteList[missileIndex]->moveTask.taskFlag = true;	}		return err;}pascal OSErr AEFireMissile (AppleEvent *theAE, AppleEvent *reply, long rfCon){	fixPt		where;	long		len;	DescType	actualType;	short		direction;	short 		missileNum;	gamePtr		game;	OSErr		err = 0;		game = (gamePtr)rfCon;#ifdef CHECK_STATE			if(err == noErr) {		if ( game->gameState != kGameInProgress ) {			err = paramErr;			ErrMsgCode("\pError - missile event rcvd in wrong state",game->gameState);		}	}#endif		if(err == noErr) {		/* get the direction of the missile */		err = AEGetParamPtr(theAE, keyMissileNum, typeShortInteger, &actualType, 				&missileNum, sizeof(short), &len);		if(err != noErr) {			ErrMsgCode("\p Failed: AEFireMissile keyDir, typeShortInteger",err);		}	}	if(err == noErr) {		/* get the direction of the missile */		err = AEGetParamPtr(theAE, keyDir, typeShortInteger, &actualType, 				&direction, sizeof(short), &len);		if(err != noErr) {			ErrMsgCode("\p Failed: AEFireMissile keyDir, typeShortInteger",err);		}	}		if(err == noErr) {		/* get the starting point of the missile */		err = AEGetParamPtr(theAE, keyLocation, typefixPt, &actualType, 				&where, sizeof(fixPt), &len);		if(err != noErr) {			ErrMsgCode("\p Failed: AEFireMissile keyLocation, typePoint",err);		}	}	if(err == noErr) {		FireMissile( direction, where.h, where.v, true, missileNum );	}	}void NetworkMoveMissile(short missileIndex){	AppleEvent		missileMovedEvt;	AppleEvent		reply;	OSErr			err;	Boolean			disposeNeeded = false;#ifdef NO_NET	return;#endif		err = AECreateAppleEvent(kZAMEventClass, kMoveRemoteMissileID, &gGame->oppAddr,				 kAnyTransactionID, gGame->gameID, &missileMovedEvt);	if(err != noErr) {		ErrMsgCode("\p Failure: NetworkMoveTank AECreateAppleEvent",err);	}			if(err == noErr) {		disposeNeeded = true;		AEPutParamPtr(&missileMovedEvt, keyMissileNum, typeShortInteger, 							&missileIndex, sizeof(short));		if(err != noErr) {			ErrMsgCode("\p Failure: AEPutParamPtr",err);		}	}		if(err == noErr) {		AEPutParamPtr(&missileMovedEvt, keyLocation, typefixPt, 							&MissileSpriteList[missileIndex]->loc, sizeof(fixPt));		if(err != noErr) {			ErrMsgCode("\p Failure: AEPutParamPtr",err);		}	}		if(err == noErr) {		err = AESend(&missileMovedEvt, &reply, kAENoReply + kAECanInteract,				 kAENormalPriority, 				 60 * 60, nil, nil);		if(err != noErr) {			ErrMsgCode("\p NetworkMoveMissile: AESend missileMovedEvt",err);		}	}	if(disposeNeeded) {		AEDisposeDesc(&NetworkMoveMissile);	}}void NetworkFireMissile(gamePtr game, short dir, Fixed h, Fixed v, short missileNum){	OSErr			err;	AppleEvent		fireMissileEvent;	AppleEvent		reply;	Boolean			disposeNeeded = false;	fixPt			where;	#ifdef NO_NET	return;#endif	err = AECreateAppleEvent(kZAMEventClass, kFireMissileID, &game->oppAddr,				 kAnyTransactionID, game->gameID, &fireMissileEvent);	if(err != noErr) {		ErrMsgCode("\p Failure: FireNetworkMissile AECreateAppleEvent",err);	}		if(err == noErr) {		disposeNeeded = true;		err = AEPutParamPtr(&fireMissileEvent, keyMissileNum, typeShortInteger, 							&missileNum, sizeof(short));		if(err != noErr) {			ErrMsgCode("\p Failure: AEPutParamPtr",err);		}	}	if(err == noErr) {		err = AEPutParamPtr(&fireMissileEvent, keyDir, typeShortInteger, 							&dir, sizeof(short));		if(err != noErr) {			ErrMsgCode("\p Failure: AEPutParamPtr",err);		}	}		if(err == noErr) {		where.h = h;		where.v = v;		err = AEPutParamPtr(&fireMissileEvent, keyLocation, typefixPt, 							&where, sizeof(fixPt) );		if(err != noErr) {			ErrMsgCode("\p Failure: AEPutParamPtr",err);		}	}		if(err == noErr) {		err = AESend(&fireMissileEvent, &reply, kAENoReply + kAECanInteract,				 kAENormalPriority, 				 60 * 60, nil, nil);		if(err != noErr) {			ErrMsgCode("\p Failure: AESend",err);		}	}			if(disposeNeeded)		AEDisposeDesc(&fireMissileEvent);}