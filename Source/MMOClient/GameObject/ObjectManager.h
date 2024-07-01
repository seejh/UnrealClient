// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"../Proto/Protocol3.pb.h"

class AMyCharacterBase;
class AMMOClientCharacter;
class AMyMonster;
class AProjectile;
class UMyGameInstance;
class AMyPlayerController;
class MMOCLIENT_API FObjectManager
{
public:
	bool Init(AMyPlayerController* controller);

	// 추가, 제거
	AMyCharacterBase* Add(PROTOCOL::ObjectInfo info);
	void Remove(int objectId);
	
	// 
	PROTOCOL::GameObjectType GetObjectTypeById(int objectId);
	
	AMyCharacterBase* GetObject(int objectId);
	PROTOCOL::ObjectInfo* GetObjectInfo(int objectId);
	
public:
	FCriticalSection _cs;

	UMyGameInstance* _ownerInstance;
	AMyPlayerController* _ownerController;

	// 액터 인포
	TMap<int, PROTOCOL::ObjectInfo*> _objectInfos;
	
	// 액터 - 하나로 관리하고 싶지만, 힘듬
	TMap<int, AMMOClientCharacter*> _players;
	TMap<int, AMyMonster*> _monsters;
};
