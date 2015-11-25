#include "ZAMProtos.h"#include "TankSprite.h"#include "GameSounds.h"#include "SpriteFrameRates.h"spriteLayerPtr	gTankLayer[kNumTankSprites];frameSetPtr		gTankFrameSetList[kNumDirections];spritePtr		gTankSprites[kNumTankSprites];tankInfoRec		gTankInfo[kNumTankSprites];xthing			gTankSynchTask;xthing 			gTankIncidentalSoundTask;#define kSlopTolerance 6long gDeadTime;void CheckTankDead(void)/*	This is called in the main loop to kill all sounds and play the you lose sound.*/{	tankInfoRec	*ti;		if(gDead == true) {		PlaySndAsynchChannelNow(kYouLose, kMusicChan, kHighPriority+20);		gDead = false;	}	}void TankColissionHandler( spritePtr tank, spritePtr obj, Rect *colArea)/*	The sprite manager calls this when a missile hits a tank.	This makes sure the hit was a good hit, and decrements the tank hit counter*/{	tankInfoRec	*ti;		if( (RECT_WD(*colArea) >= kCollThresh) && (RECT_HT(*colArea) >= kCollThresh) ) {		ti = (tankInfoRec*)tank->refCon;		ti->damage++;		if(ti->damage > 8) {			obj->spriteFlags |= kRemoteKilled;			ti->damage = 0;		} else			PlaySndAsynchChannel(kBadFireSnd, kFlightChan, kStdPriority);	}		}void SynchronizeTank( gamePtr game, fixPt *loc, short direction, short speed)/*	This is where the remote tank is updated from the AppleEvent*/{	spritePtr		tank;	tankInfoRec		*ti;	Boolean			reAdjustNeeded = false;		tank = gTankSprites[game->remoteTankIndex];	if(abs((loc->h - tank->loc.h)) > kSlopTolerance) {		reAdjustNeeded = true;	} else if(abs((loc->v - tank->loc.v)) > kSlopTolerance) {		reAdjustNeeded = true;	}		if(reAdjustNeeded) {		tank->remoteLoc = *loc;		tank->moveTask.taskFlag = true;	}		ti = (tankInfoRec *)tank->refCon;	if(ti->speed != speed) {		SetTankSpeed(tank, speed);	}		if(ti->dir != direction) {		ti->dir = direction;		SetSpriteFrameSet(tank,gTankFrameSetList[ti->dir]);		tank->spriteFlags |= kNeedsToBeDrawn | kNeedsToBeErased;		tank->ownerLayer->layerFlags |= kLayerDirty;		if( ti->speed != 0) {			DirectionToVelocity( ti->dir, &tank->vel);			if(ti->speed != 0) {				DirectionToVelocity(ti->dir, &tank->vel);				if(ti->speed > 1) {					tank->vel.h = FixMul(tank->vel.h,ff(ti->speed));					tank->vel.v = FixMul(tank->vel.v,ff(ti->speed));				}			} else {				tank->vel.h = 0;				tank->vel.v = 0;			}		}	}}void RotateTank(spritePtr spr, short dir){	tankInfoRec	*tInfo;	frameCellPtr		pFrameCell;		tInfo = (tankInfoRec*)spr->refCon;	if(dir) {		tInfo->dir += dir;				if(tInfo->dir >= kNumDirections) 			tInfo->dir = 0;		if(tInfo->dir < 0) 			tInfo->dir = kNumDirections -1;		SetSpriteFrameSet(spr,gTankFrameSetList[tInfo->dir]);		spr->spriteFlags |= kNeedsToBeDrawn | kNeedsToBeErased;		spr->ownerLayer->layerFlags |= kLayerDirty;	}}void SetTankSpeed(spritePtr spr, short speed){				tankInfoRec		*tInfo;	fixPt			vel;		tInfo = (tankInfoRec*)spr->refCon;		tInfo->speed += speed;	if(tInfo->speed > kMaxSpeed) 		tInfo->speed = kMaxSpeed;			if(tInfo->speed < 0) 		tInfo->speed = 0;			if(tInfo->speed != 0) {		DirectionToVelocity(tInfo->dir, &vel);				spr->vel.h += vel.h;		spr->vel.v += vel.v;			} else {		spr->vel.h = 0;		spr->vel.v = 0;	}}Boolean TankFrameTask(spritePtr spr, frameCellPtr curFrame)/*	Since this is a global frame task, curFrame is set to nil	if being called before the frame has been updated.	curFrame will be set to -1 if being called after the frame has already	been updated by the sprite manager.*/{	short		dir = 0;		if(KeyIsDown(kRotateLeftKeyCode))		dir = -1;	else if(KeyIsDown(kRotateRightKeyCode))		dir = 1;		if(dir) {		RotateTank(spr, dir);	}		return true;}Boolean TankMoveTask(spritePtr spr){	char			key;	short			speed = 0;	tankInfoRec		*tInfo;	short			dir= 0;	tInfo = (tankInfoRec*)spr->refCon;		if(KeyIsDown(kAccelerateKeyCode))		speed = 1;	else if(KeyIsDown(kDecelerateKeyCode))		speed = -1;	if(KeyIsDown(kFireKeyCode)) {		FireMissile( tInfo->dir, spr->loc.h, spr->loc.v, false,0);	}		if(speed) {		SetTankSpeed(spr, speed);	}					return true;}void LoadTankSprites(gamePtr game){	OSErr			err;	short			ref;	short			i,iconID;	frameSetPtr		missileFrameSet;			ref = OpenResFile("\pnuTankFrames.rsrc");	if(err = ResError()) {		ErrMsgCode("\pCould Not Open MissileFrames file!.",err);		ExitToShell();	}		if(err == noErr) {		for(i = 0; i < kNumDirections; i ++) {					if(err == noErr) {				iconID = kTankBaseID + i;//				err = CreateColorIconFrameSet(&gTankFrameSetList[i], iconID, kNumTankFrames);				err = CreatePICTIconFrameSet(&gTankFrameSetList[i], iconID, kNumTankFrames);				if(err != noErr) {					ErrMsgCode("\pCreateColorIconFrameSet failed.",err);				}				if(err == noErr)					SetFrameSetCTSeed(gTankFrameSetList[i],game->gameCTSeed);															}		}	}	CloseResFile(ref);			for(i = 0; i < kNumTankSprites; i++) {		err = CreateSpriteLayer(&gTankLayer[i], 							game->tween, 							game->backdrop, 							game->gameWind);		if(err != noErr) {			ErrMsgCode("\pCreateSpriteLayer failed!",err);			ExitToShell();		}				err = CreateEmptySprite(gTankLayer[i],							&gTankSprites[i],			/* returned sprite here */							kFrameTaskBeforeUpdate		/* sprite flags */ 							+ kConstrainToRect,							kTankFrameTime, 			/* time between frame change */							kTankMoveTime, 				/* time between movement */							(long)&gTankInfo[i]);				if(err != noErr) {			ErrMsgCode("\pCreateEmptySprite failed.",err);		} 				if(err == noErr) {			err = CreateEmptyFrameSet(&gTankSprites[i]->frameList,kNumTankFrames);			if(err != noErr) {				ErrMsgCode("\pCreateEmptyFrameSet failed.",err);			}		}				if(err == noErr) {			SetSpriteFrameSet(gTankSprites[i],gTankFrameSetList[4]);			gTankSprites[i]->refCon = (long)&gTankInfo[i];			gTankInfo[i].dir = 4;			gTankInfo[i].speed = 0;			gTankInfo[i].game = game;			gTankInfo[i].damage = 0;			gTankSprites[i]->vel.h = 0;			gTankSprites[i]->vel.v = 0;			gTankSprites[i]->visible = true;			gTankSprites[i]->constrainRect = game->gameArea;		}	}}void PlaceTankSprites(gamePtr game){	register short	localIndex;	register short	remoteIndex;	tankInfoRec		*ti;			localIndex = game->localTankIndex;	remoteIndex = game->remoteTankIndex;		gTankSprites[localIndex]->frameHandler = (frameProc)TankFrameTask;	gTankSprites[localIndex]->moveHandler = (moveProc)TankMoveTask;	gTankSprites[localIndex]->spriteFlags |= kNeedsToBeDrawn | kNeedsToBeErased;	gTankLayer[localIndex]->layerFlags |= kLayerDirty;	gTankSprites[remoteIndex]->spriteFlags |= kRemoteSprite | kRemoteUpdate 											| kNeedsToBeDrawn | kNeedsToBeErased;	gTankLayer[remoteIndex]->layerFlags |= kLayerDirty;	if(game->localTankIndex == 0) {		SetSpriteLoc(gTankSprites[localIndex], ff(40), ff(100));		SetSpriteLoc(gTankSprites[remoteIndex], ff(460), ff(100));	} else {		SetSpriteLoc(gTankSprites[remoteIndex], ff(40), ff(100));		SetSpriteLoc(gTankSprites[localIndex], ff(460), ff(100));	}			StartSpriteAction(gTankSprites[localIndex]);	gTankSprites[remoteIndex]->collideHandler = (collisionProc)TankColissionHandler;			#ifndef NO_NET	StartRemoteSpriteAction(gTankSprites[game->remoteTankIndex]);	StartXThing(&gTankSynchTask, kSynchTime, (updateProc)TankSynchTask, 							(long)gTankSprites[game->localTankIndex]);#endif}