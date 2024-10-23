// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include"Data/GameStruct.h"

#include "DataManager.generated.h"



UCLASS()
class MMOCLIENT_API UDataManager : public UObject
{
	GENERATED_BODY()
public:
	bool Init();
	bool LoadFile(FString filePath, TSharedPtr<FJsonObject>& jsonObj);

	bool LoadMonsterData();
	bool LoadStatData();
	bool LoadItemData();
	bool LoadSkillData();
	bool LoadQuestData();
	bool LoadNpcData();
	bool LoadNpcDialogueData();

public:
	UPROPERTY()
		TMap<int32, FMonsterData> _monsterTable;
	UPROPERTY()
		TMap<int32, FStatData> _statTable;
	UPROPERTY()
		TMap<int32, UItemData*> _itemTable;
	UPROPERTY()
		TMap<int32, FSkillData> _skillTable;
	UPROPERTY()
		TMap<int32, UQuestData*> _questTable;
	UPROPERTY()
		TMap<int32, UNpcData*> _npcTable;
	UPROPERTY()
		TMap<int32, UNpcDialogueData*> _dialogueTable;
};
