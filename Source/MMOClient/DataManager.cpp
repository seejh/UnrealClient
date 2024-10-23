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

	// 파일 로드
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
	// 파일 경로
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/MonsterData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// 파일 열고 파싱
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load MonsterData Error"));
		return false;
	}

	// 몬스터데이터
	TArray<TSharedPtr<FJsonValue>> monsterJsonValues = jsonObj->GetArrayField(TEXT("monsters"));
	for (TSharedPtr<FJsonValue> monsterJsonValue : monsterJsonValues) {
		FMonsterData monsterData;

		// 아이디, 이름, 애셋경로
		monsterData.id = monsterJsonValue->AsObject()->GetIntegerField("id");
		monsterData.name = monsterJsonValue->AsObject()->GetStringField("name");
		monsterData.assetPath = monsterJsonValue->AsObject()->GetStringField("asset");
		
		// 스텟
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

		// 보상
		TArray<TSharedPtr<FJsonValue>> rewardValues = monsterJsonValue->AsObject()->GetArrayField(TEXT("rewards"));
		for (TSharedPtr<FJsonValue> rewardValue : rewardValues) {
			FRewardData rewardData;
			
			// 드랍율, 아이템 아이디, 수량
			rewardData.itemDropRate = rewardValue->AsObject()->GetIntegerField("percent");
			rewardData.itemId = rewardValue->AsObject()->GetIntegerField("itemid");
			rewardData.count = rewardValue->AsObject()->GetIntegerField("count");

			// 
			monsterData.rewardDatas.Add(rewardData);
		}

		// 몬스터 테이블에 적재
		_monsterTable.Add(monsterData.id, monsterData);
	}
	
	return true;
}

bool UDataManager::LoadStatData()
{
	// 파일 경로
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/StatData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// 파일 열고 파싱
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load StatData Error"));
		return false;
	}

	// 스탯데이터
	TArray<TSharedPtr<FJsonValue>> statJsonValues = jsonObj->GetArrayField(TEXT("stats"));
	for (TSharedPtr<FJsonValue> statJsonValue : statJsonValues) {
		//
		FStatData statData;

		// 레벨, 최대체력, 공격력, 속도, 경험치량
		statData.level = statJsonValue->AsObject()->GetIntegerField("level");
		statData.maxHp = statJsonValue->AsObject()->GetIntegerField("maxHp");
		statData.damage = statJsonValue->AsObject()->GetIntegerField("damage");
		statData.speed = statJsonValue->AsObject()->GetNumberField("speed");
		statData.totalExp = statJsonValue->AsObject()->GetIntegerField("totalExp");

		// 스텟 테이블에 적재
		_statTable.Add(statData.level, statData);
	}

	return true;
}

// 아이템
bool UDataManager::LoadItemData()
{
	// 파일 경로
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/ItemData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// 파일 로드
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load ItemData Error"));
		return false;
	}

	// 무기
	TArray<TSharedPtr<FJsonValue>> weaponJsonValues = jsonObj->GetArrayField(TEXT("weapons"));
	for (TSharedPtr<FJsonValue> weaponJsonValue : weaponJsonValues) {
		
		//
		UWeaponData* weaponData = NewObject<UWeaponData>();
		
		// id, 이름, 설명
		weaponData->id = weaponJsonValue->AsObject()->GetIntegerField("id");
		weaponData->name = weaponJsonValue->AsObject()->GetStringField("name");
		weaponData->description = weaponJsonValue->AsObject()->GetStringField("description");

		// 섬네일
		FString iconPath = weaponJsonValue->AsObject()->GetStringField("iconPath");
		weaponData->thumbnail = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *iconPath));

		// 세부 정보 (무기타입, 공격력)
		weaponJsonValue->AsObject()->GetStringField("weaponType");
		weaponData->damage = weaponJsonValue->AsObject()->GetIntegerField("damage");
		
		// 아이템 테이블에 적재
		_itemTable.Add(weaponData->id, weaponData);
	}

	// 방어구
	TArray<TSharedPtr<FJsonValue>> armorJsonValues = jsonObj->GetArrayField(TEXT("armors"));
	for (TSharedPtr<FJsonValue> armorJsonValue : armorJsonValues) {
		
		//
		UArmorData* armorData = NewObject<UArmorData>();

		// id, 이름, 설명
		armorData->id = armorJsonValue->AsObject()->GetIntegerField("id");
		armorData->name = armorJsonValue->AsObject()->GetStringField("name");
		armorData->description = armorJsonValue->AsObject()->GetStringField("description");

		// 섬네일
		FString iconPath = armorJsonValue->AsObject()->GetStringField("iconPath");
		armorData->thumbnail = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *iconPath));

		// 세부정보 (아머타입, 방어력)
		armorJsonValue->AsObject()->GetStringField("armorType");
		armorData->defence = armorJsonValue->AsObject()->GetIntegerField("defence");
		
		// 아이템 테이블에 적재
		_itemTable.Add(armorData->id, armorData);
	}

	// 소비류
	TArray<TSharedPtr<FJsonValue>> consumableJsonValues = jsonObj->GetArrayField(TEXT("consumables"));
	for (TSharedPtr<FJsonValue> consumableJsonValue : consumableJsonValues) {

		// 
		UConsumableData* consumableData = NewObject<UConsumableData>();

		// id, 이름, 설명
		consumableData->id = consumableJsonValue->AsObject()->GetIntegerField("id");
		consumableData->name = consumableJsonValue->AsObject()->GetStringField("name");
		consumableData->description = consumableJsonValue->AsObject()->GetStringField("description");

		// 섬네일
		FString iconPath = consumableJsonValue->AsObject()->GetStringField("iconPath");
		consumableData->thumbnail = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *iconPath));
		
		// 세부정보 (소비류 타입, 최대 개수, 회복력)
		FString consumableType = consumableJsonValue->AsObject()->GetStringField("consumableType");
		if (consumableType.Compare("HpPotion") == 0)
			consumableData->consumableType = EConsumableType::CONSUMABLE_TYPE_HP_POTION;
		else if (consumableType.Compare("MpPotion") == 0)
			consumableData->consumableType = EConsumableType::CONSUMABLE_TYPE_MP_POTION;
		else
			consumableData->consumableType = EConsumableType::CONSUMABLE_TYPE_NONE;

		consumableData->maxCount = 100;
		consumableData->recovery = consumableJsonValue->AsObject()->GetIntegerField("recovery");

		// 아이템 테이블에 적재
		_itemTable.Add(consumableData->id, consumableData);
	}

	return true;
}

bool UDataManager::LoadSkillData()
{
	// 파일 경로
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/SkillData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// 파일 열고 파싱
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load SkillData Error"));
		return false;
	}

	// 스킬 데이터
	TArray<TSharedPtr<FJsonValue>> skillJsonValues = jsonObj->GetArrayField(TEXT("skills"));
	for (TSharedPtr<FJsonValue> skillJsonValue : skillJsonValues) {
		//
		FSkillData skillData;

		// 아이디, 명칭, 쿨타임, 공격력
		skillData.id = skillJsonValue->AsObject()->GetIntegerField("id");
		skillData.name = skillJsonValue->AsObject()->GetStringField("name");
		skillData.coolTime = skillJsonValue->AsObject()->GetNumberField("cooldown");
		skillData.damage = skillJsonValue->AsObject()->GetIntegerField("damage");
		
		// 스킬타입
		FString skillType = skillJsonValue->AsObject()->GetStringField("skillType");
		
		// 스킬타입 - 기본공격
		if (skillType.Compare("SkillAuto") == 0) {
			skillData.skillType = ESkillType::SKILL_TYPE_AUTO;
		}
		// 스킬타입 - 투사체
		else if (skillType.Compare("SkillProjectile") == 0) {
			skillData.skillType = ESkillType::SKILL_TYPE_PROJECTILE;
		}
		// 스킬타입 - NONE
		else {
			skillData.skillType = ESkillType::SKILL_TYPE_NONE;
		}

		// 스킬 테이블에 적재
		_skillTable.Add(skillData.id, skillData);
	}

	return true;
}

// 퀘스트
bool UDataManager::LoadQuestData()
{
	// 파일 경로
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/QuestData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// 파일 열고 파싱
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load QuestData Error"));
		return false;
	}

	// 퀘스트 데이터
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
	// 파일 경로
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/NpcData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// 파일 열고 파싱
	if (LoadFile(path, jsonObj) == false) {
		UE_LOG(LogTemp, Error, TEXT("Load NpcData Error"));
		return false;
	}

	// NPC 데이터
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

// 현재 사용 안함
bool UDataManager::LoadNpcDialogueData()
{
	// 파일 경로
	FString path = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Source/MMOClient/Data/NpcDialogueData.json"));
	TSharedPtr<FJsonObject> jsonObj;

	// 파일 열고 파싱
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


