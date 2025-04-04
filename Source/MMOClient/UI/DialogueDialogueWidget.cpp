// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueDialogueWidget.h"
#include"DialogueWidget.h"
#include<Components/Button.h>
#include<Components/TextBlock.h>
#include"../Data/GameStruct.h"
#include"../MyGameInstance.h"
#include"../ServerPacketHandler.h"
#include"../NetSession.h"
#include"../DataManager.h"

void UDialogueDialogueWidget::NativeConstruct()
{
	AcceptBtn->OnReleased.AddDynamic(this, &UDialogueDialogueWidget::OnAcceptBtnClicked);
	NextBtn->OnReleased.AddDynamic(this, &UDialogueDialogueWidget::OnNextBtnClicked);
}

void UDialogueDialogueWidget::Init(UQuestData* QuestData, bool complete)
{
	// 퀘스트데이터 포인터를 내부적으로 들고있고, UI에 대화설정

	// 퀘스트 데이터
	questData = QuestData;
	
	

	if (complete) {
		// 퀘스트 완료 대사 - 

		// npc엔더의 대사 조회
		UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
		auto npcDataPtr = instance->_dataManager->_npcTable.Find(questData->QuestEnder);
		if (npcDataPtr == nullptr)
			return;
		
		auto dialoguePtr = (*npcDataPtr)->dialogues.Find(questData->dialogueEndId);
		if (dialoguePtr == nullptr)
			return;

		AcceptBtn->SetVisibility(ESlateVisibility::Hidden);
		NextBtn->SetVisibility(ESlateVisibility::Visible);
		DialogueTxt->SetText(FText::FromString(dialoguePtr->Talks[0]));
	}
	else {
		// 퀘스트 요청 대사
		
		// 퀘스트 데이터에서 대화 정보 추출
		auto dialoguePtr = ownerUI->npcData->dialogues.Find(questData->dialogueId);
		if (dialoguePtr == nullptr)
			return;

		AcceptBtn->SetVisibility(ESlateVisibility::Visible);
		NextBtn->SetVisibility(ESlateVisibility::Hidden);
		DialogueTxt->SetText(FText::FromString(*dialoguePtr->Talks[0]));
	}
}

void UDialogueDialogueWidget::Close()
{
	questData = nullptr;
}

void UDialogueDialogueWidget::OnAcceptBtnClicked()
{
	// 서버에 퀘스트 추가 요청 패킷 보내고 대화 UI 전체 종료

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// 패킷 전송
	PROTOCOL::C_ADD_QUEST toPkt;
	toPkt.mutable_quest()->set_templateid(questData->id);

	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);

	// UI 전체 종료
	ownerUI->Close();
}

void UDialogueDialogueWidget::OnNextBtnClicked()
{
	ownerUI->Close();
}
