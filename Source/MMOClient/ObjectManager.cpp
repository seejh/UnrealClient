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
//	// ��Ʈ�ѷ� ���� ����
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
//		// �÷��̾�
//		if (info.objecttype() == PROTOCOL::GameObjectType::PLAYER) {
//
//			// ����,����     �޸� �Ҵ� -> ������ ����
//			actor = Cast<AMyCharacterBase>(FActorPool<AMMOClientCharacter>::Pop(_ownerController, _ownerController->_bpPlayer, info));
//			actorInfo = FObjectPool<PROTOCOL::ObjectInfo>::Pop();
//			actorInfo->CopyFrom(info);
//			actor->info = actorInfo;
//
//			// �߰�
//			_players.Add(info.objectid(), Cast<AMMOClientCharacter>(actor));
//			_objectInfos.Add(info.objectid(), actorInfo);
//		}
//
//		// ����
//		else if (info.objecttype() == PROTOCOL::GameObjectType::MONSTER) {
//
//			// ����,����    �޸� �Ҵ� -> ������ ����
//			actor = Cast<AMyCharacterBase>(FActorPool<AMyMonster>::Pop(_ownerController, _ownerController->_bpMonster, info));
//			actorInfo = FObjectPool<PROTOCOL::ObjectInfo>::Pop();
//			actorInfo->CopyFrom(info);
//			actor->info = actorInfo;
//
//			// ��Ͽ� �߰�
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
//	// Ÿ��
//	PROTOCOL::GameObjectType type = GetObjectTypeById(objectId);
//
//	// �÷��̾�
//	if (type == PROTOCOL::GameObjectType::PLAYER) {
//
//		// ���� ���
//		if (_players.Contains(objectId) == true) {
//			// ã�Ƽ�
//			AMMOClientCharacter* player = _players[objectId];
//			if (IsValid(player) == true) {
//				// ���� �ݳ�, ��Ͽ��� ����
//				FActorPool<AMMOClientCharacter>::Push(player);
//				_players.Remove(objectId);
//			}
//		}
//
//		// ���� ���
//		if (_objectInfos.Contains(objectId) == true) {
//			// ã�Ƽ�
//			PROTOCOL::ObjectInfo* actorInfo = _objectInfos[objectId];
//			if (actorInfo != nullptr) {
//				// ���� �ݳ�, ��Ͽ��� ����
//				FObjectPool<PROTOCOL::ObjectInfo>::Push(actorInfo);
//				_objectInfos.Remove(objectId);
//			}
//		}
//	}
//
//	// ����
//	else if (type == PROTOCOL::GameObjectType::MONSTER) {
//
//		// ���� ���
//		if (_monsters.Contains(objectId) == true) {
//			// ã�Ƽ�
//			AMyMonster* monster = _monsters[objectId];
//			if (IsValid(monster) == true) {
//				// ���� �ݳ�, ��Ͽ��� ����
//				FActorPool<AMyMonster>::Push(monster);
//				_monsters.Remove(objectId);
//			}
//		}
//
//		// ���� ���
//		if (_objectInfos.Contains(objectId) == true) {
//			// ã�Ƽ�
//			PROTOCOL::ObjectInfo* actorInfo = _objectInfos[objectId];
//			if (actorInfo != nullptr) {
//				// ���� �ݳ�, ��Ͽ��� ����
//				FObjectPool<PROTOCOL::ObjectInfo>::Push(actorInfo);
//				_objectInfos.Remove(objectId);
//			}
//		}
//	}
//}
//
//PROTOCOL::GameObjectType FObjectManager::GetObjectTypeById(int objectId)
//{
//	// Ÿ��
//	int type = (objectId >> 24) & 0x7F;
//
//	// �÷��̾�
//	if (type == PROTOCOL::GameObjectType::PLAYER)
//		return PROTOCOL::GameObjectType::PLAYER;
//	
//	// ����
//	else if (type == PROTOCOL::GameObjectType::MONSTER) 
//		return PROTOCOL::GameObjectType::MONSTER;
//	
//	// �� ��
//	else 
//		return PROTOCOL::GameObjectType::NONE;
//}
//
//AMyCharacterBase* FObjectManager::GetObject(int objectId)
//{
//	// Ÿ��
//	PROTOCOL::GameObjectType type = GetObjectTypeById(objectId);
//	
//	// �÷��̾�
//	if (type == PROTOCOL::GameObjectType::PLAYER) {
//		if (_players.Contains(objectId)) 
//			return _players[objectId];
//	}
//	// ����
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
//	// Ÿ��
//	PROTOCOL::GameObjectType type = GetObjectTypeById(objectId);
//
//	if (_objectInfos.Contains(objectId))
//		return _objectInfos[objectId];
//
//	return nullptr;
//}
