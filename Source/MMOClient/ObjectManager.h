// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"Proto/Protocol3.pb.h"

class AMyCharacterBase;
class AMMOClientCharacter;
class AMyMonster;
class AProjectile;
class UMyGameInstance;
class AMyPlayerController;
//class MMOCLIENT_API FObjectManager
//{
//public:
//	bool Init(AMyPlayerController* controller);
//
//	// 추가, 제거
//	AMyCharacterBase* Add(PROTOCOL::ObjectInfo info);
//	void Remove(int objectId);
//	
//	// 
//	PROTOCOL::GameObjectType GetObjectTypeById(int objectId);
//	
//	AMyCharacterBase* GetObject(int objectId);
//	PROTOCOL::ObjectInfo* GetObjectInfo(int objectId);
//	
//public:
//	UMyGameInstance* _ownerInstance;
//	AMyPlayerController* _ownerController;
//
//	TMap<int, PROTOCOL::ObjectInfo*> _objectInfos;
//	TMap<int, AMMOClientCharacter*> _players;
//	TMap<int, AMyMonster*> _monsters;
//};
