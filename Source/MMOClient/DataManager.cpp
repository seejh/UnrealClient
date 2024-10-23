// Fill out your copyright notice in the Description page of Project Settings.


#include "DataManager.h"
#include<JsonObjectConverter.h>

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
	
	if (LoadNpcData() == false)
		return false;

	if (LoadQuestData() == false)
		return false;

	// if (LoadNpcDialogueData() == false) return false;

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

		// ���̵�, �̸�, �ּ°��
		monsterData.id = monsterJsonValue->AsObject()->GetIntegerField("id");
		monsterData.name = monsterJsonValue->AsObject()->GetStringField("name");
		monsterData.assetPath = monsterJsonValue->AsObject()->GetStringField("asset");
		
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
		FString consumableType = consumableJsonValue->AsObject()->GetStringField("consumableType");
		if (consumableType.Compare("HpPotion") == 0)
			consumableData->consumableType = EConsumableType::CONSUMABLE_TYPE_HP_POTION;
		else if (consumableType.Compare("MpPotion") == 0)
			consumableData->consumableType = EConsumableType::CONSUMABLE_TYPE_MP_POTION;
		else
			consumableData->consumableType = EConsumableType::CONSUMABLE_TYPE_NONE;

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

// ����Ʈ
bool UDataManager::LoadQuestData()
{
	// ���� ���
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/QuestData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// ���� ���� �Ľ�
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load QuestData Error"));
		return false;
	}

	// ����Ʈ ������
	TArray<TSharedPtr<FJsonValue>> questJsonValues = jsonObj->GetArrayField(TEXT("Quests"));
	for (auto questJsonValue : questJsonValues) {
		//
		UQuestData* questData = NewObject<UQuestData>();

		//
		questData->id = questJsonValue->AsObject()->GetIntegerField("Id");
		questData->name = questJsonValue->AsObject()->GetStringField("Name");
		questData->description = questJsonValue->AsObject()->GetStringField("Description");

		FString type = questJsonValue->AsObject()->GetStringField("Type");
		if (type.Compare("Interact") == 0)
			questData->type = EQuestType::QUEST_TYPE_INTERACT;
		else if (type.Compare("Kill") == 0)
			questData->type = EQuestType::QUEST_TYPE_KILL;
		else if (type.Compare("Collect") == 0)
			questData->type = EQuestType::QUEST_TYPE_COLLECT;
		else {
			UE_LOG(LogTemp, Error, TEXT("UDataManager::LoadQuestData() Error - No QuestType - %s"), *type);
			return false;
		}

		questData->objectiveId = questJsonValue->AsObject()->GetIntegerField("ObjectiveId");
		questData->quantity = questJsonValue->AsObject()->GetIntegerField("Quantity");

		questData->QuestGiver = questJsonValue->AsObject()->GetIntegerField("QuestGiver");
		questData->QuestEnder = questJsonValue->AsObject()->GetIntegerField("QuestEnder");
		questData->dialogueId = questJsonValue->AsObject()->GetIntegerField("DialogueId");
		questData->dialogueEndId = questJsonValue->AsObject()->GetIntegerField("DialogueEndId");

		questData->rewardExp = questJsonValue->AsObject()->GetIntegerField("RewardExp");
		questData->rewardMoney = questJsonValue->AsObject()->GetIntegerField("RewardMoney");
		
		TArray<TSharedPtr<FJsonValue>> rewardJsonValues = questJsonValue->AsObject()->GetArrayField("RewardItems");
		for (auto rewardJsonValue : rewardJsonValues) {
			questData->rewardItems.Add(rewardJsonValue->AsObject()->GetIntegerField("ItemId"),
				rewardJsonValue->AsObject()->GetIntegerField("ItemQuantity"));
		}

		_questTable.Add(questData->id, questData);
	}

	return true;
}

// NPC
bool UDataManager::LoadNpcData()
{
	// ���� ���
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/NpcData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// ���� ���� �Ľ�
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load NpcData Error"));
		return false;
	}

	// NPC ������
	TArray<TSharedPtr<FJsonValue>> npcJsonValues = jsonObj->GetArrayField(TEXT("Npcs"));
	for (TSharedPtr<FJsonValue> npcJsonValue : npcJsonValues) {
		//
		UNpcData* npcData = NewObject<UNpcData>();

		//
		npcData->id = npcJsonValue->AsObject()->GetIntegerField("Id");
		npcData->name = npcJsonValue->AsObject()->GetStringField("Name");
		
		FString npcType = npcJsonValue->AsObject()->GetStringField("NpcType");
		if (npcType.Compare("Normal") == 0) {
			npcData->npcType = ENpcType::NPC_TYPE_NORMAL;
		}
		else if (npcType.Compare("Merchant") == 0) {
			npcData->npcType = ENpcType::NPC_TYPE_MERCHANT;
		}
		else {
			npcData->npcType = ENpcType::NPC_TYPE_NONE;
		}

		npcData->greeting = npcJsonValue->AsObject()->GetStringField("Greeting");

		TArray<TSharedPtr<FJsonValue>> npcQuestJsonValues = npcJsonValue->AsObject()->GetArrayField("Quests");
		for (auto npcQuestJsonValue : npcQuestJsonValues) 
			npcData->quests.Add(FCString::Atoi(*(npcQuestJsonValue->AsString())));

		TArray<TSharedPtr<FJsonValue>> dialogueJsonValues = npcJsonValue->AsObject()->GetArrayField("Dialogues");
		for (auto dialogueJsonValue : dialogueJsonValues) {
			//
			FDialogue dialogue;

			//
			dialogue.dialogueId = dialogueJsonValue->AsObject()->GetIntegerField("DialogueId");

			TArray<TSharedPtr<FJsonValue>> talkJsonValues = dialogueJsonValue->AsObject()->GetArrayField("Talks");
			for (auto talkJsonValue : talkJsonValues) 
				dialogue.Talks.Add(talkJsonValue->AsString());

			TArray<TSharedPtr<FJsonValue>> talkEndJsonValues = dialogueJsonValue->AsObject()->GetArrayField("TalkEnds");
			for (auto talkEndJsonValue : talkEndJsonValues)
				dialogue.TalkEnds.Add(talkEndJsonValue->AsString());

			npcData->dialogues.Add(dialogue.dialogueId, dialogue);
		}

		_npcTable.Add(npcData->id, npcData);
	}

	return true;
}

// ���� ��� ����
bool UDataManager::LoadNpcDialogueData()
{
	// ���� ���
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/NpcDialogueData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// ���� ���� �Ľ�
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load NpcDialogueData Error"));
		return false;
	}

	//
	TArray<TSharedPtr<FJsonValue>> npcDialogueJsonValues = jsonObj->GetArrayField(TEXT("NpcDialogues"));
	for (auto npcDialogueJsonValue : npcDialogueJsonValues) {
		//
		UNpcDialogueData* data = NewObject<UNpcDialogueData>();

		//
		data->npcId = npcDialogueJsonValue->AsObject()->GetIntegerField("NpcId");
		auto dialogueJsonValues = npcDialogueJsonValue->AsObject()->GetArrayField("Dialogues");
		for (auto dialogueJsonValue : dialogueJsonValues) {
			//
			FDialogue dialogue;

			//
			dialogue.dialogueId = dialogueJsonValue->AsObject()->GetIntegerField("DialogueId");
			
			auto talkJsonValues = dialogueJsonValue->AsObject()->GetArrayField("Talks");
			for (auto talkJsonValue : talkJsonValues) 
				dialogue.Talks.Add(talkJsonValue->AsString());

			auto talkEndJsonValues = dialogueJsonValue->AsObject()->GetArrayField("TalkEnds");
			for (auto talkEndJsonValue : talkEndJsonValues)
				dialogue.TalkEnds.Add(talkEndJsonValue->AsString());

			data->dialogues.Add(dialogue.dialogueId, dialogue);
		}

		_dialogueTable.Add(data->npcId, data);
	}

	return true;
}


