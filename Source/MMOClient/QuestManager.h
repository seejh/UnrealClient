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
	현재 진행중인 퀘스트뿐만이 아니라 해결한 역대 퀘스트들도 포함한다.
	서버에 통신하지 않고 퀘스트 완료 목록을 확인하기 위해

	퀘스트매니저의 변경사항이 있을 때마다 퀘스트UI에 콜하는 방식
	다만, 퀘스트UI가 퀘스트 매니저보다 늦게 활성화된다.
	그걸 구분해야된다.
---------------------------------------------------------------------------*/
UCLASS()
class MMOCLIENT_API UQuestManager : public UObject
{
	GENERATED_BODY()
public:
	// 추가, 제거
	void AddQuest(PROTOCOL::QuestInfo questInfo);
	void RemoveQuest(int32 questTemplateId);

	// 서버에 상호작용 요청(패킷)
	void InteractUpdateReq(int objectId, int objectiveTemplateId);
	
	// 패킷 받고 퀘스트 완료 처리
	void CompleteQuest(int questId);

	// 패킷 받고 퀘스트 업데이트 처리
	void UpdateQuest(PROTOCOL::QuestInfo questInfo);

	// 이 NPC를 통해 마무리할 수 있는 퀘스트가 있는가
	UQuest* CanCompleteByNpc(int npcId);

	// UI 싹다 삭제
	void Clear();
	
	bool Contains(int questId);

public:
	UPROPERTY()
		TMap<int32, UQuest*> _quests;
	UMyGameInstance* _ownerInstance;
};
