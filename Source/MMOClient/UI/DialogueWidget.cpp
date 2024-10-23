// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"
#include<Components/TextBlock.h>
#include<Components/ScrollBox.h>
#include<Components/Button.h>
#include<Components/WidgetSwitcher.h>
#include"../MyGameInstance.h"
#include"../DataManager.h"
#include"../Data/GameStruct.h"
#include"DialogueGreeting.h"
#include"DialogueDialogueWidget.h"
#include"DialogueQuestList.h"
#include"../MyPlayerController.h"
#include"../QuestManager.h"
#include"../ServerPacketHandler.h"
#include"../NetSession.h"

void UDialogueWidget::NativeConstruct()
{
	// 다른 위젯의 NativeConstruct()에서 호출하면 에러
	// 어딘가에서 이 객체를 UPROPERTY로 들고 있어야 가능한듯
	instance = Cast<UMyGameInstance>(GetGameInstance());
	
	GreetingUI->ownerUI = this;
	QuestListUI->ownerUI = this;
	DialogueUI->ownerUI = this;
	
	CloseBtn->OnReleased.AddDynamic(this, &UDialogueWidget::OnCloseBtnClicked);
}

void UDialogueWidget::Open(int32 npcId)
{
	// 해당 하는 NPC 정보 불러오고 퀘스트완료하거나 기본인사

	// 정보 설정
	auto npcDataPtr = instance->_dataManager->_npcTable.Find(npcId);
	if (npcDataPtr == nullptr)
		return;

	npcData = (*npcDataPtr);

	// 타이틀(NPC 이름)
	TitleTxt->SetText(FText::FromString(npcData->name));

	// 이 npc를 통해 완료할 수 있는 퀘스트가 있는가
	UQuest* quest = instance->_questManager->CanCompleteByNpc(npcId);
	if (quest == nullptr) {
		OpenGreetingUI();
	}
	else {
		// 서버에 퀘스트 완료 요청 보내고 완료 대화 진행
		
		UE_LOG(LogTemp, Error, TEXT("Request to Server Complete Quest"));

		// 완료 요청
		PROTOCOL::C_CompleteQuest toPkt;
		toPkt.set_questdbid(quest->_questInfo.templateId);

		auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
		instance->_netSession->Send(sendBuffer);

		// 완료 대화
		OpenDialogueUI(quest->_questData, true);
	}
	
	// 비저블
	SetVisibility(ESlateVisibility::Visible);
}

void UDialogueWidget::Close()
{
	SetVisibility(ESlateVisibility::Hidden);

	GreetingUI->Close();
	QuestListUI->Close();
	DialogueUI->Close();

	npcData = nullptr;
}

void UDialogueWidget::OpenGreetingUI() {
	GreetingUI->Init();
	Switcher->SetActiveWidget(GreetingUI);
}

void UDialogueWidget::OpenQuestListUI() {
	QuestListUI->Init();
	Switcher->SetActiveWidget(QuestListUI);
}

void UDialogueWidget::OpenDialogueUI(UQuestData* questData, bool complete) {
	DialogueUI->Init(questData, complete);
	Switcher->SetActiveWidget(DialogueUI);
}

void UDialogueWidget::OnCloseBtnClicked()
{
	Close();
}
