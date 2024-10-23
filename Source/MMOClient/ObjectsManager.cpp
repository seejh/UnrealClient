// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectsManager.h"
#include"DataManager.h"
#include"MyGameInstance.h"
#include"GameObject/MMOClientCharacter.h"
#include"GameObject/MyMonster.h"
#include"GameObject/MyNpc.h"

UObjectsManager::UObjectsManager()
{
	// ĳ����
	UObject* cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Blueprint'/Game/BP_Player.BP_Player'"));
	if (IsValid(cls) == false) {
		UE_LOG(LogTemp, Error, TEXT("BP_Player Load Failed"));
		return;
	}
	UBlueprint* bp = Cast<UBlueprint>(cls);
	_bpPlayer = (UClass*)bp->GeneratedClass;

	// ����
	UObject* cls2 = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Blueprint'/Game/BP_Monster.BP_Monster'"));
	if (IsValid(cls2) == false) {
		UE_LOG(LogTemp, Error, TEXT("BP_Monster Load Failed"));
		return;
	}
	UBlueprint* bp2 = Cast<UBlueprint>(cls2);
	_bpMonster = (UClass*)bp2->GeneratedClass;

	// ����
	UObject* cls3 = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Blueprint'/Game/BP_BlackCrunch.BP_BlackCrunch'"));
	if (IsValid(cls3) == false) {
		UE_LOG(LogTemp, Error, TEXT("BP_BlackCrunch Load Failed"));
		return;
	}
	UBlueprint* bp3 = Cast<UBlueprint>(cls3);
	_bpBlackCrunch = (UClass*)bp3->GeneratedClass;

	// NPC
	UObject* cls4 = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Blueprint'/Game/BP_Npc.BP_Npc'"));
	if (IsValid(cls4) == false) {
		UE_LOG(LogTemp, Error, TEXT("BP_Npc Load Failed"));
		return;
	}
	UBlueprint* bp4 = Cast<UBlueprint>(cls4);
	_bpNpc = (UClass*)bp4->GeneratedClass;



	// ���̷�Ż �޽�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> BlackCrunchAsset(TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/Skins/Tier_3/BlackSite/Meshes/Crunch_Black_Site"));
	if (BlackCrunchAsset.Succeeded())
		_blackCrunchMesh = BlackCrunchAsset.Object;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> CrunchAsset(TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/Meshes/Crunch"));
	if (CrunchAsset.Succeeded())
		_crunchMesh = CrunchAsset.Object;
}

AMyCharacterBase* UObjectsManager::Add(PROTOCOL::ObjectInfo info)
{
	FVector loc(info.pos().locationx(), info.pos().locationy(), info.pos().locationz());
	FRotator rot(info.pos().rotationpitch(), info.pos().rotationyaw(), info.pos().rotationroll());
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// �÷��̾�
	if (info.objecttype() == PROTOCOL::GameObjectType::PLAYER) {
		AMMOClientCharacter* player = _ownerInstance->GetWorld()->SpawnActor<AMMOClientCharacter>(_bpPlayer, loc, rot, param);
		player->_objectId = info.objectid();

		_playerObjects.Add(info.objectid(), player);
		_objectInfos.Add(info.objectid(), info);

		player->SpawnDefaultController();

		return player;
	}

	// ����
	else if (info.objecttype() == PROTOCOL::GameObjectType::MONSTER) {
		AMyMonster* monster;

		if (info.name().compare("Crunch") == 0) {
			monster = _ownerInstance->GetWorld()->SpawnActor<AMyMonster>(_bpMonster, loc, rot, param);
		}
		else if (info.name().compare("BlackCrunch") == 0) {
			monster = _ownerInstance->GetWorld()->SpawnActor<AMyMonster>(_bpBlackCrunch, loc, rot, param);
		}
		else {
			monster = _ownerInstance->GetWorld()->SpawnActor<AMyMonster>(_bpMonster, loc, rot, param);
		}
		monster->_objectId = info.objectid();

		_objects.Add(info.objectid(), monster);
		_objectInfos.Add(info.objectid(), info);

		return monster;
	}

	// NPC
	else if (info.objecttype() == PROTOCOL::GameObjectType::NPC) {
		AMyNpc* npc;

		npc = _ownerInstance->GetWorld()->SpawnActor<AMyNpc>(_bpNpc, loc, rot, param);
		npc->_objectId = info.objectid();

		_objects.Add(info.objectid(), npc);
		_objectInfos.Add(info.objectid(), info);

		return npc;
	}
	
	return nullptr;
}

void UObjectsManager::Remove(int objectId)
{
	switch (GetTypeById(objectId)) {
	
	case PROTOCOL::GameObjectType::PLAYER:
	{
		auto playerObjectPtr = _playerObjects.Find(objectId);
		if (playerObjectPtr) {
			if ((*playerObjectPtr)->Destroy()) {
				UE_LOG(LogTemp, Error, TEXT("UObjectManager::Remove() - PLAYER-%d REMOVED SUCCEED"), objectId);

				//
				_playerObjects.Remove(objectId);
				_objectInfos.Remove(objectId);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("UObjectManager::Remove() - PLAYER-%d REMOVED FAILED"), objectId);
			}
		}
	}
	break;
	 
	case PROTOCOL::GameObjectType::MONSTER:
	{
		auto monsterObjectPtr = _objects.Find(objectId);
		if (monsterObjectPtr) {
			if ((*monsterObjectPtr)->Destroy()) {
				UE_LOG(LogTemp, Error, TEXT("UObjectManager::Remove() - MONSTER-%d REMOVED SUCCEED"), objectId);

				//
				_objects.Remove(objectId);
				_objectInfos.Remove(objectId);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("UObjectManager::Remove() - MONSTER-%d REMOVED FAILED"), objectId);
			}
		}
	}
	break;
	
	case PROTOCOL::GameObjectType::NPC:
	{
		auto npcObjectPtr = _objects.Find(objectId);
		if (npcObjectPtr) {
			if ((*npcObjectPtr)->Destroy()) {
				UE_LOG(LogTemp, Error, TEXT("UObjectManager::Remove() - NPC-%d REMOVED SUCCEED"), objectId);

				//
				_objects.Remove(objectId);
				_objectInfos.Remove(objectId);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("UObjectManager::Remove() - NPC-%d REMOVED FAILED"), objectId);
			}
		}
	}
	break;
	 
	default:
		UE_LOG(LogTemp, Error, TEXT("UObjectMAnager::Remove() - Default Error"));
	}
}

PROTOCOL::GameObjectType UObjectsManager::GetTypeById(int objectId)
{
	int type = (objectId >> 24) & 0x7F;
	
	// �÷��̾�
	if (type == PROTOCOL::GameObjectType::PLAYER)
		return PROTOCOL::GameObjectType::PLAYER;

	// ����
	else if (type == PROTOCOL::GameObjectType::MONSTER)
		return PROTOCOL::GameObjectType::MONSTER;

	// NPC
	else if (type == PROTOCOL::GameObjectType::NPC)
		return PROTOCOL::GameObjectType::NPC;

	// �� ��
	else
		return PROTOCOL::GameObjectType::NONE;
}

PROTOCOL::ObjectInfo* UObjectsManager::GetObjectInfoById(int objectId)
{
	return _objectInfos.Find(objectId);
}

AMyCharacterBase* UObjectsManager::GetObjectById(int objectId)
{
	switch (GetTypeById(objectId)) {
	case PROTOCOL::GameObjectType::PLAYER:
	{
		auto objectPtr = _playerObjects.Find(objectId);
		if (objectPtr == nullptr)
			return nullptr;

		return (*objectPtr);
	}
	break;

	case PROTOCOL::GameObjectType::MONSTER:
	{
		auto objectPtr = _objects.Find(objectId);
		if (objectPtr == nullptr)
			return nullptr;

		return (*objectPtr);
	}
	break;

	case PROTOCOL::GameObjectType::NPC:
	{
		auto objectPtr = _objects.Find(objectId);
		if (objectPtr == nullptr)
			return nullptr;

		return (*objectPtr);
	}
	break;

	default:
		return nullptr;
	}
}

void UObjectsManager::Clear()
{
	// ������Ʈ ����, �÷��̾������Ʈ, �� �� ������Ʈ�� ����

	// ������Ʈ ����
	_objectInfos.Empty();
	
	// ������Ʈ (ĳ����, ������ ����)
	for (auto p : _objects) 
		p.Value->Destroy();
	_objects.Empty();

	// �÷��̾� ������Ʈ (ĳ����, ������ ����)
	for (auto p : _playerObjects)
		p.Value->Destroy();
	_playerObjects.Empty();
}
