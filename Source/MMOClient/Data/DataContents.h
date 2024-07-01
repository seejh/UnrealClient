// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include"../Proto/Protocol3.pb.h"

/*------------------------------------------------------------------
	DataManager���� ���Ǵ� ������Ÿ�Ե�
-------------------------------------------------------------------*/

///*------------------------------------------------------------------
//	������, ������ ����(����, ����, �Һ��)
//------------------------------------------------------------------*/
//class MMOCLIENT_API FItemData {
//public:
//	FItemData(PROTOCOL::ItemType itemType) : _itemType(itemType) {}
//public:
//	int32 _id;
//	FString _name;
//	PROTOCOL::ItemType _itemType;
//};
//
//class MMOCLIENT_API FWeaponData : public FItemData {
//public:
//	FWeaponData() : FItemData(PROTOCOL::ItemType::ITEM_TYPE_WEAPON) {}
//public:
//	PROTOCOL::WeaponType _weaponType;
//	int _damage;
//};
//
//class MMOCLIENT_API FArmorData : public FItemData {
//public:
//	FArmorData() : FItemData(PROTOCOL::ItemType::ITEM_TYPE_ARMOR) {}
//public:
//	PROTOCOL::ArmorType _armorType;
//	int _defence;
//};
//
//class MMOCLIENT_API FConsumableData : public FItemData {
//public:
//	FConsumableData() : FItemData(PROTOCOL::ItemType::ITEM_TYPE_CONSUMABLE) {}
//public:
//	PROTOCOL::ConsumableType _consumableType;
//	int _maxCount;
//};
//

/*------------------------------------------------------------------
	����, ���� ����
------------------------------------------------------------------*/
//class MMOCLIENT_API FRewardData {
//public:
//	int32 itemDropRate;
//	int32 itemId;
//	int32 count;
//};
//
//class MMOCLIENT_API FMonsterData {
//public:
//	int32 id;
//	FString name;
//	PROTOCOL::Stat stat;
//	TArray<FRewardData> rewardDatas;
//};
///*------------------------------------------------------------------
//	��ų
//------------------------------------------------------------------*/
//class MMOCLIENT_API FSkillData {
//public:
//	int id;
//	FString name;
//	float cooltime;
//	int damage;
//	PROTOCOL::SkillType skillType;
//};
//
//class MMOCLIENT_API FSkillProjectileData : public FSkillData {
//public:
//
//public:
//
//};
//
///*------------------------------------------------------------------
//	����
//------------------------------------------------------------------*/
//class MMOCLIENT_API FStatData {
//public:
//	int32 level;
//	int32 maxHp;
//	int32 attack;
//	float speed;
//	int32 totalExp;
//};



