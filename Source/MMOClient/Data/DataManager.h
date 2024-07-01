// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStruct.h"
#include "DataManager.generated.h"

class UItemData;
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

public:
	UPROPERTY()
	TMap<int, FMonsterData> _monsterTable;
	
	UPROPERTY()
	TMap<int, FStatData> _statTable;
	
	UPROPERTY()
	TMap<int, UItemData*> _itemTable;
	
	UPROPERTY()
	TMap<int, FSkillData> _skillTable;
};
