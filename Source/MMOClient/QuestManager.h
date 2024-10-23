// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include"Data/GameStruct.h"

#include "QuestManager.generated.h"

USTRUCT(BlueprintType)
struct FQuestInfo {
	GENERATED_BODY()
public:
	UPROPERTY()
		int32 questDbId;
	UPROPERTY()
		int32 templateId;
	UPROPERTY()
		int32 playerDbId;
	UPROPERTY()
		int32 progress;
	UPROPERTY()
		bool completed;
};

UCLASS()
class UQuest : public UObject {
	GENERATED_BODY()
public:
	void Update(EQuestType& type, int32& objectiveId, int32& quantity, UMyGameInstance* instance = nullptr);
public:
	UPROPERTY()
		FQuestInfo _questInfo;
	UPROPERTY()
		UQuestData* _questData;
	UPROPERTY()
		class UQuestLogButton* QuestBtnUI;
	UPROPERTY()
		class UQuestLogContentWidget* QuestContentUI;
};

/*---------------------------------------------------------------------------
	���� �������� ����Ʈ�Ӹ��� �ƴ϶� �ذ��� ���� ����Ʈ�鵵 �����Ѵ�.
	������ ������� �ʰ� ����Ʈ �Ϸ� ����� Ȯ���ϱ� ����

	����Ʈ�Ŵ����� ��������� ���� ������ ����ƮUI�� ���ϴ� ���
	�ٸ�, ����ƮUI�� ����Ʈ �Ŵ������� �ʰ� Ȱ��ȭ�ȴ�.
	�װ� �����ؾߵȴ�.
---------------------------------------------------------------------------*/
UCLASS()
class MMOCLIENT_API UQuestManager : public UObject
{
	GENERATED_BODY()
public:
	// �߰�, ����
	void AddQuest(PROTOCOL::QuestInfo questInfo);
	void RemoveQuest(int32 questTemplateId);

	// ������ ��ȣ�ۿ� ��û(��Ŷ)
	void InteractUpdateReq(int objectId, int objectiveTemplateId);
	
	// ��Ŷ �ް� ����Ʈ �Ϸ� ó��
	void CompleteQuest(int questId);

	// ��Ŷ �ް� ����Ʈ ������Ʈ ó��
	void UpdateQuest(PROTOCOL::QuestInfo questInfo);

	// �� NPC�� ���� �������� �� �ִ� ����Ʈ�� �ִ°�
	UQuest* CanCompleteByNpc(int npcId);

	// UI �ϴ� ����
	void Clear();
	
	bool Contains(int questId);

public:
	UPROPERTY()
		TMap<int32, UQuest*> _quests;
	UMyGameInstance* _ownerInstance;
};
