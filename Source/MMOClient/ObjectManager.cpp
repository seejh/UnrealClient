// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectManager.h"

#include"GameObject/ActorPool.h"
#include"GameObject/ObjectPool.h"
#include"GameObject/MyCharacterBase.h"
#include"GameObject/MyMonster.h"

#include"MyPlayerController.h"
//
//bool FObjectManager::Init(AMyPlayerController* controller)
//{
//	// 
//	if (!controller) {
//		UE_LOG(LogTemp, Error, TEXT("ObjectManager::Init Error : INVALID Owner Controller"));
//		return false;
//	}
//
//	// 컨트롤러 참조 설정
//	_ownerController = controller;
//	
//	return true;
//}
//
//AMyCharacterBase* FObjectManager::Add(PROTOCOL::ObjectInfo info)
//{
//	AMyCharacterBase* actor = nullptr;
//	PROTOCOL::ObjectInfo* actorInfo = nullptr;
//
//	if (IsValid(_ownerController)) {
//
//		// 플레이어
//		if (info.objecttype() == PROTOCOL::GameObjectType::PLAYER) {
//
//			// 액터,인포     메모리 할당 -> 데이터 설정
//			actor = Cast<AMyCharacterBase>(FActorPool<AMMOClientCharacter>::Pop(_ownerController, _ownerController->_bpPlayer, info));
//			actorInfo = FObjectPool<PROTOCOL::ObjectInfo>::Pop();
//			actorInfo->CopyFrom(info);
//			actor->info = actorInfo;
//
//			// 추가
//			_players.Add(info.objectid(), Cast<AMMOClientCharacter>(actor));
//			_objectInfos.Add(info.objectid(), actorInfo);
//		}
//
//		// 몬스터
//		else if (info.objecttype() == PROTOCOL::GameObjectType::MONSTER) {
//
//			// 액터,인포    메모리 할당 -> 데이터 설정
//			actor = Cast<AMyCharacterBase>(FActorPool<AMyMonster>::Pop(_ownerController, _ownerController->_bpMonster, info));
//			actorInfo = FObjectPool<PROTOCOL::ObjectInfo>::Pop();
//			actorInfo->CopyFrom(info);
//			actor->info = actorInfo;
//
//			// 목록에 추가
//			_monsters.Add(info.objectid(), Cast<AMyMonster>(actor));
//			_objectInfos.Add(info.objectid(), actorInfo);
//		}
//	}
//	
//	return actor;
//}
//
//void FObjectManager::Remove(int objectId)
//{
//	// 타입
//	PROTOCOL::GameObjectType type = GetObjectTypeById(objectId);
//
//	// 플레이어
//	if (type == PROTOCOL::GameObjectType::PLAYER) {
//
//		// 액터 목록
//		if (_players.Contains(objectId) == true) {
//			// 찾아서
//			AMMOClientCharacter* player = _players[objectId];
//			if (IsValid(player) == true) {
//				// 액터 반납, 목록에서 제거
//				FActorPool<AMMOClientCharacter>::Push(player);
//				_players.Remove(objectId);
//			}
//		}
//
//		// 인포 목록
//		if (_objectInfos.Contains(objectId) == true) {
//			// 찾아서
//			PROTOCOL::ObjectInfo* actorInfo = _objectInfos[objectId];
//			if (actorInfo != nullptr) {
//				// 인포 반납, 목록에서 제거
//				FObjectPool<PROTOCOL::ObjectInfo>::Push(actorInfo);
//				_objectInfos.Remove(objectId);
//			}
//		}
//	}
//
//	// 몬스터
//	else if (type == PROTOCOL::GameObjectType::MONSTER) {
//
//		// 액터 목록
//		if (_monsters.Contains(objectId) == true) {
//			// 찾아서
//			AMyMonster* monster = _monsters[objectId];
//			if (IsValid(monster) == true) {
//				// 액터 반납, 목록에서 제거
//				FActorPool<AMyMonster>::Push(monster);
//				_monsters.Remove(objectId);
//			}
//		}
//
//		// 인포 목록
//		if (_objectInfos.Contains(objectId) == true) {
//			// 찾아서
//			PROTOCOL::ObjectInfo* actorInfo = _objectInfos[objectId];
//			if (actorInfo != nullptr) {
//				// 인포 반납, 목록에서 제거
//				FObjectPool<PROTOCOL::ObjectInfo>::Push(actorInfo);
//				_objectInfos.Remove(objectId);
//			}
//		}
//	}
//}
//
//PROTOCOL::GameObjectType FObjectManager::GetObjectTypeById(int objectId)
//{
//	// 타입
//	int type = (objectId >> 24) & 0x7F;
//
//	// 플레이어
//	if (type == PROTOCOL::GameObjectType::PLAYER)
//		return PROTOCOL::GameObjectType::PLAYER;
//	
//	// 몬스터
//	else if (type == PROTOCOL::GameObjectType::MONSTER) 
//		return PROTOCOL::GameObjectType::MONSTER;
//	
//	// 그 외
//	else 
//		return PROTOCOL::GameObjectType::NONE;
//}
//
//AMyCharacterBase* FObjectManager::GetObject(int objectId)
//{
//	// 타입
//	PROTOCOL::GameObjectType type = GetObjectTypeById(objectId);
//	
//	// 플레이어
//	if (type == PROTOCOL::GameObjectType::PLAYER) {
//		if (_players.Contains(objectId)) 
//			return _players[objectId];
//	}
//	// 몬스터
//	else if (type == PROTOCOL::GameObjectType::MONSTER) {
//		if (_monsters.Contains(objectId))
//			return _monsters[objectId];
//	}
//
//	return nullptr;
//}
//
//PROTOCOL::ObjectInfo* FObjectManager::GetObjectInfo(int objectId)
//{
//	// 타입
//	PROTOCOL::GameObjectType type = GetObjectTypeById(objectId);
//
//	if (_objectInfos.Contains(objectId))
//		return _objectInfos[objectId];
//
//	return nullptr;
//}
