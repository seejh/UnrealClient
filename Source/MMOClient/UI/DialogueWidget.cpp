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
	// �ٸ� ������ NativeConstruct()���� ȣ���ϸ� ����
	// ��򰡿��� �� ��ü�� UPROPERTY�� ��� �־�� �����ѵ�
	instance = Cast<UMyGameInstance>(GetGameInstance());
	
	GreetingUI->ownerUI = this;
	QuestListUI->ownerUI = this;
	DialogueUI->ownerUI = this;
	
	CloseBtn->OnReleased.AddDynamic(this, &UDialogueWidget::OnCloseBtnClicked);
}

void UDialogueWidget::Open(int32 npcId)
{
	// �ش� �ϴ� NPC ���� �ҷ����� ����Ʈ�Ϸ��ϰų� �⺻�λ�

	// ���� ����
	auto npcDataPtr = instance->_dataManager->_npcTable.Find(npcId);
	if (npcDataPtr == nullptr)
		return;

	npcData = (*npcDataPtr);

	// Ÿ��Ʋ(NPC �̸�)
	TitleTxt->SetText(FText::FromString(npcData->name));

	// �� npc�� ���� �Ϸ��� �� �ִ� ����Ʈ�� �ִ°�
	UQuest* quest = instance->_questManager->CanCompleteByNpc(npcId);
	if (quest == nullptr) {
		OpenGreetingUI();
	}
	else {
		// ������ ����Ʈ �Ϸ� ��û ������ �Ϸ� ��ȭ ����
		
		UE_LOG(LogTemp, Error, TEXT("Request to Server Complete Quest"));

		// �Ϸ� ��û
		PROTOCOL::C_CompleteQuest toPkt;
		toPkt.set_questdbid(quest->_questInfo.templateId);

		auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
		instance->_netSession->Send(sendBuffer);

		// �Ϸ� ��ȭ
		OpenDialogueUI(quest->_questData, true);
	}
	
	// ������
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
