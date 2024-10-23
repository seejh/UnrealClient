// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include"Proto/Protocol3.pb.h"

#include "ObjectsManager.generated.h"


/*
	현재 게임동기화 방식을 위해서 플레이어들을 따로 관리할 필요가 있다.

*/
class AMMOClientCharacter;
class AMyCharacterBase;
class UMyGameInstance;
UCLASS()
class MMOCLIENT_API UObjectsManager : public UObject
{
	GENERATED_BODY()
public:
	UObjectsManager();

	AMyCharacterBase* Add(PROTOCOL::ObjectInfo info);
	void Remove(int objectId);

	PROTOCOL::GameObjectType GetTypeById(int objectId);
	PROTOCOL::ObjectInfo* GetObjectInfoById(int objectId);
	AMyCharacterBase* GetObjectById(int objectId);

	// 모든 오브젝트와 정보 삭제
	void Clear();
public:
	TMap<int, PROTOCOL::ObjectInfo> _objectInfos;
	TMap<int, AMyCharacterBase*> _objects;
	TMap<int, AMMOClientCharacter*> _playerObjects;
	

	UMyGameInstance* _ownerInstance;

	// 
	TSubclassOf<UObject> _bpPlayer;
	TSubclassOf<UObject> _bpMonster;
	TSubclassOf<UObject> _bpBlackCrunch;
	TSubclassOf<UObject> _bpNpc;

	// 
	USkeletalMesh* _crunchMesh;
	USkeletalMesh* _blackCrunchMesh;
};
