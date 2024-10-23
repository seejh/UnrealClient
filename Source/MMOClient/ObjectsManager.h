// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include"Proto/Protocol3.pb.h"

#include "ObjectsManager.generated.h"


/*
	���� ���ӵ���ȭ ����� ���ؼ� �÷��̾���� ���� ������ �ʿ䰡 �ִ�.

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

	// ��� ������Ʈ�� ���� ����
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
