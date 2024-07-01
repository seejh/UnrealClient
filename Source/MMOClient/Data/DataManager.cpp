// Fill out your copyright notice in the Description page of Project Settings.


#include "DataManager.h"
#include<JsonObjectConverter.h>

#include"GameStruct.h"

bool UDataManager::Init()
{
	if (LoadMonsterData() == false) 
		return false;

	if (LoadStatData() == false)
		return false;
	
	if (LoadItemData() == false)
		return false;
	
	if (LoadSkillData() == false)
		return false;

	UE_LOG(LogTemp, Error, TEXT("FDataManager - Data Load OK!"));
	
	return true;
}

bool UDataManager::LoadFile(FString filePath, TSharedPtr<FJsonObject>& jsonObj)
{
	FString jsonString;

	// ���� �ε�
	if (FFileHelper::LoadFileToString(jsonString, *filePath) == false) {
		UE_LOG(LogTemp, Error, TEXT("Json OpenAndParse Error : Invalid FilePath"));
		return false;
	}

	// Json Reader
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(jsonString);
	
	// Deserialize
	if (FJsonSerializer::Deserialize(reader, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("JsonReader Deserialize Error"));
		return false;
	}

	if (jsonObj.IsValid() == false) {
		UE_LOG(LogTemp, Error, TEXT("JsonObject Error : Invalid"));
		return false;
	}

	return true;
}

bool UDataManager::LoadMonsterData()
{
	// ���� ���
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/MonsterData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// ���� ���� �Ľ�
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load MonsterData Error"));
		return false;
	}

	// ���͵�����
	TArray<TSharedPtr<FJsonValue>> monsterJsonValues = jsonObj->GetArrayField(TEXT("monsters"));
	for (TSharedPtr<FJsonValue> monsterJsonValue : monsterJsonValues) {
		FMonsterData monsterData;

		// ���̵�, �̸�
		monsterJsonValue->TryGetNumber(monsterData.id);
		monsterJsonValue->TryGetString(monsterData.name);

		// ����
		TSharedPtr<FJsonObject> statJsonObject = monsterJsonValue->AsObject()->GetObjectField("stat");
		monsterData.statData.level = statJsonObject->GetIntegerField("level");
		monsterData.statData.totalExp = statJsonObject->GetIntegerField("totalexp");
		monsterData.statData.maxHp = statJsonObject->GetIntegerField("maxhp");
		monsterData.statData.damage = statJsonObject->GetIntegerField("damage");
		monsterData.statData.speed = statJsonObject->GetIntegerField("speed");
		monsterData.statData.attackDistance = statJsonObject->GetIntegerField("attackdistance");
		monsterData.statData.noticeDistance = statJsonObject->GetIntegerField("noticedistance");
		monsterData.statData.returnDistance = statJsonObject->GetIntegerField("returndistance");
		monsterData.statData.attackCoolTime = statJsonObject->GetIntegerField("attackcooltime");

		// ����
		TArray<TSharedPtr<FJsonValue>> rewardValues = monsterJsonValue->AsObject()->GetArrayField(TEXT("rewards"));
		for (TSharedPtr<FJsonValue> rewardValue : rewardValues) {
			FRewardData rewardData;
			
			// �����, ������ ���̵�, ����
			rewardData.itemDropRate = rewardValue->AsObject()->GetIntegerField("percent");
			rewardData.itemId = rewardValue->AsObject()->GetIntegerField("itemid");
			rewardData.count = rewardValue->AsObject()->GetIntegerField("count");

			// 
			monsterData.rewardDatas.Add(rewardData);
		}

		// ���� ���̺� ����
		_monsterTable.Add(monsterData.id, monsterData);
	}
	
	return true;
}

bool UDataManager::LoadStatData()
{
	// ���� ���
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/StatData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// ���� ���� �Ľ�
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load StatData Error"));
		return false;
	}

	// ���ȵ�����
	TArray<TSharedPtr<FJsonValue>> statJsonValues = jsonObj->GetArrayField(TEXT("stats"));
	for (TSharedPtr<FJsonValue> statJsonValue : statJsonValues) {
		//
		FStatData statData;

		// ����, �ִ�ü��, ���ݷ�, �ӵ�, ����ġ��
		statData.level = statJsonValue->AsObject()->GetIntegerField("level");
		statData.maxHp = statJsonValue->AsObject()->GetIntegerField("maxHp");
		statData.damage = statJsonValue->AsObject()->GetIntegerField("damage");
		statData.speed = statJsonValue->AsObject()->GetNumberField("speed");
		statData.totalExp = statJsonValue->AsObject()->GetIntegerField("totalExp");

		// ���� ���̺� ����
		_statTable.Add(statData.level, statData);
	}

	return true;
}

// ������
bool UDataManager::LoadItemData()
{
	// ���� ���
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/ItemData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// ���� �ε�
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load ItemData Error"));
		return false;
	}

	// ����
	TArray<TSharedPtr<FJsonValue>> weaponJsonValues = jsonObj->GetArrayField(TEXT("weapons"));
	for (TSharedPtr<FJsonValue> weaponJsonValue : weaponJsonValues) {
		
		//
		UWeaponData* weaponData = NewObject<UWeaponData>();
		
		// id, �̸�, ����
		weaponData->id = weaponJsonValue->AsObject()->GetIntegerField("id");
		weaponData->name = weaponJsonValue->AsObject()->GetStringField("name");
		weaponData->description = weaponJsonValue->AsObject()->GetStringField("description");

		// ������
		FString iconPath = weaponJsonValue->AsObject()->GetStringField("iconPath");
		weaponData->thumbnail = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *iconPath));

		// ���� ���� (����Ÿ��, ���ݷ�)
		weaponJsonValue->AsObject()->GetStringField("weaponType");
		weaponData->damage = weaponJsonValue->AsObject()->GetIntegerField("damage");
		
		// ������ ���̺� ����
		_itemTable.Add(weaponData->id, weaponData);
	}

	// ��
	TArray<TSharedPtr<FJsonValue>> armorJsonValues = jsonObj->GetArrayField(TEXT("armors"));
	for (TSharedPtr<FJsonValue> armorJsonValue : armorJsonValues) {
		
		//
		UArmorData* armorData = NewObject<UArmorData>();

		// id, �̸�, ����
		armorData->id = armorJsonValue->AsObject()->GetIntegerField("id");
		armorData->name = armorJsonValue->AsObject()->GetStringField("name");
		armorData->description = armorJsonValue->AsObject()->GetStringField("description");

		// ������
		FString iconPath = armorJsonValue->AsObject()->GetStringField("iconPath");
		armorData->thumbnail = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *iconPath));

		// �������� (�Ƹ�Ÿ��, ����)
		armorJsonValue->AsObject()->GetStringField("armorType");
		armorData->defence = armorJsonValue->AsObject()->GetIntegerField("defence");
		
		// ������ ���̺� ����
		_itemTable.Add(armorData->id, armorData);
	}

	// �Һ��
	TArray<TSharedPtr<FJsonValue>> consumableJsonValues = jsonObj->GetArrayField(TEXT("consumables"));
	for (TSharedPtr<FJsonValue> consumableJsonValue : consumableJsonValues) {

		// 
		UConsumableData* consumableData = NewObject<UConsumableData>();

		// id, �̸�, ����
		consumableData->id = consumableJsonValue->AsObject()->GetIntegerField("id");
		consumableData->name = consumableJsonValue->AsObject()->GetStringField("name");
		consumableData->description = consumableJsonValue->AsObject()->GetStringField("description");

		// ������
		FString iconPath = consumableJsonValue->AsObject()->GetStringField("iconPath");
		consumableData->thumbnail = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *iconPath));
		
		// �������� (�Һ�� Ÿ��, �ִ� ����, ȸ����)
		consumableJsonValue->AsObject()->GetStringField("consumableType");
		consumableData->maxCount = 100;
		consumableData->recovery = consumableJsonValue->AsObject()->GetIntegerField("recovery");

		// ������ ���̺� ����
		_itemTable.Add(consumableData->id, consumableData);
	}

	return true;
}

bool UDataManager::LoadSkillData()
{
	// ���� ���
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/SkillData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// ���� ���� �Ľ�
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load SkillData Error"));
		return false;
	}

	// ��ų ������
	TArray<TSharedPtr<FJsonValue>> skillJsonValues = jsonObj->GetArrayField(TEXT("skills"));
	for (TSharedPtr<FJsonValue> skillJsonValue : skillJsonValues) {
		//
		FSkillData skillData;

		// ���̵�, ��Ī, ��Ÿ��, ���ݷ�
		skillData.id = skillJsonValue->AsObject()->GetIntegerField("id");
		skillData.name = skillJsonValue->AsObject()->GetStringField("name");
		skillData.coolTime = skillJsonValue->AsObject()->GetNumberField("cooldown");
		skillData.damage = skillJsonValue->AsObject()->GetIntegerField("damage");
		
		// ��ųŸ��
		FString skillType = skillJsonValue->AsObject()->GetStringField("skillType");
		
		// ��ųŸ�� - �⺻����
		if (skillType.Compare("SkillAuto") == 0) {
			skillData.skillType = ESkillType::SKILL_TYPE_AUTO;
		}
		// ��ųŸ�� - ����ü
		else if (skillType.Compare("SkillProjectile") == 0) {
			skillData.skillType = ESkillType::SKILL_TYPE_PROJECTILE;
		}
		// ��ųŸ�� - NONE
		else {
			skillData.skillType = ESkillType::SKILL_TYPE_NONE;
		}

		// ��ų ���̺� ����
		_skillTable.Add(skillData.id, skillData);
	}

	return true;
}


