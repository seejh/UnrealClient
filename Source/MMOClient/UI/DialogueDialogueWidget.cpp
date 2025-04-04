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
	// ����Ʈ������ �����͸� ���������� ����ְ�, UI�� ��ȭ����

	// ����Ʈ ������
	questData = QuestData;
	
	

	if (complete) {
		// ����Ʈ �Ϸ� ��� - 

		// npc������ ��� ��ȸ
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
		// ����Ʈ ��û ���
		
		// ����Ʈ �����Ϳ��� ��ȭ ���� ����
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
	// ������ ����Ʈ �߰� ��û ��Ŷ ������ ��ȭ UI ��ü ����

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// ��Ŷ ����
	PROTOCOL::C_ADD_QUEST toPkt;
	toPkt.mutable_quest()->set_templateid(questData->id);

	auto sendBuffer = instance->_packetHandler->MakeSendBuffer(toPkt);
	instance->_netSession->Send(sendBuffer);

	// UI ��ü ����
	ownerUI->Close();
}

void UDialogueDialogueWidget::OnNextBtnClicked()
{
	ownerUI->Close();
}
