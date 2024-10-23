// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestLogContentWidget.h"
#include<Components/TextBlock.h>
#include<Components/Button.h>
#include"../Data/GameStruct.h"
#include"../QuestManager.h"
#include"../MyGameInstance.h"
#include"../DataManager.h"
#include"../ServerPacketHandler.h"
#include"../NetSession.h"

void UQuestLogContentWidget::NativeConstruct()
{
	CancelBtn->OnReleased.AddDynamic(this, &UQuestLogContentWidget::OnCancelBtnClicked);
}

void UQuestLogContentWidget::OnCancelBtnClicked()
{
	// 퀘스트 취소 요청

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	
	// 패킷
	PROTOCOL::C_RemoveQuest toPkt;
	toPkt.set_questdbid(quest->_questInfo.templateId);
	
	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);
}

void UQuestLogContentWidget::SetContent(UQuest* _quest)
{
	// 퀘스트 설정 후 해당 퀘스트 정보로 위젯 설정

	quest = _quest;

	Update();
}

void UQuestLogContentWidget::Update()
{
	// 퀘스트 정보로 위젯 설정

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	DescriptionTxt->SetText(FText::FromString(quest->_questData->description));
	
	FString str;

	// 완료
	if (quest->_questInfo.completed == true) {
		ProgressTxt->SetText(FText::FromString(FString::Printf(TEXT("완료된 퀘스트입니다."))));
		CancelBtn->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	switch (quest->_questData->type) {
	case EQuestType::QUEST_TYPE_INTERACT:
	{
		auto npcDataPtr = instance->_dataManager->_npcTable.Find(quest->_questData->objectiveId);
		if (npcDataPtr == nullptr)
			return;

		str.Append(FString(TEXT("NPC ")))
			.Append((*npcDataPtr)->name)
			.Append(FString::Printf(TEXT("와(과) 대화하기 [")))
			.Append(FString::FromInt(quest->_questInfo.progress))
			.Append("/")
			.Append(FString::FromInt(quest->_questData->quantity))
			.Append("]");
	}
	break;

	case EQuestType::QUEST_TYPE_KILL:
	{
		auto monsterDataPtr = instance->_dataManager->_monsterTable.Find(quest->_questData->objectiveId);
		if (monsterDataPtr == nullptr)
			return;
		
		str.Append(FString::Printf(TEXT("몬스터 ")))
			.Append(monsterDataPtr->name)
			.Append(FString::Printf(TEXT(" 처치하기 [")))
			.Append(FString::FromInt(quest->_questInfo.progress))
			.Append("/")
			.Append(FString::FromInt(quest->_questData->quantity))
			.Append("]");
	}
	break;

	case EQuestType::QUEST_TYPE_COLLECT:
	{
		auto itemDataPtr = instance->_dataManager->_itemTable.Find(quest->_questData->objectiveId);
		if (itemDataPtr == nullptr) 
			return;

		str.Append("아이템 ")
			.Append((*itemDataPtr)->name)
			.Append(FString::Printf(TEXT(" 수집하기 [")))
			.Append(FString::FromInt(quest->_questInfo.progress))
			.Append("/")
			.Append(FString::FromInt(quest->_questData->quantity))
			.Append("]");
	}
	break;

	default:
		break;
	}

	ProgressTxt->SetText(FText::FromString(str));
}

