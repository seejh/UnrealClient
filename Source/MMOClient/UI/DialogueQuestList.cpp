// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueQuestList.h"

#include<Components/TextBlock.h>
#include<Components/Button.h>
#include<Components/ScrollBox.h>
#include"DialogueWidget.h"
#include"../Data/GameStruct.h"
#include"../MyGameInstance.h"
#include"../DataManager.h"
#include"DialogueQuestListSlot.h"
#include"../UIManager.h"
#include"DialogueDialogueWidget.h"
#include"../QuestManager.h"

void UDialogueQuestList::NativeConstruct()
{
}

void UDialogueQuestList::Init()
{
	// 제약 사항들을 확인 후 위배되지 않는 퀘스트들만 추가(퀘스트매니저에서 이미 보유, 퀘스트의 제한사항)
	// 현재는 일단 퀘스트 자체의 제약은 체크 안하고, 해당 퀘스트를 중복해서 표기만 못하게

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// 현재 상호작용하고 있는 NPC의 퀘스트 목록
	for (int questId : ownerUI->npcData->quests) {

		// 퀘스트 매니저에서 보유 여부(내가 이미 소유하거나, 꺤 적이 있음)
		if (instance->_questManager->Contains(questId) == true)
			continue;

		// 퀘스트의 제한사항
		auto questDataPtr = instance->_dataManager->_questTable.Find(questId);
		if (questDataPtr == nullptr)
			continue;

		UQuestData* questData = (*questDataPtr);
		// TODO : 

		// 추가 (제약사항 위배 안됨)
		UDialogueQuestListSlot* slotWidget = Cast<UDialogueQuestListSlot>(CreateWidget(GetWorld(), instance->_uiManager->DialogueQuestListSlotClass));
		if (IsValid(slotWidget) == false)
			continue;

		slotWidget->Init(this, questData);

		QuestLists->AddChild(slotWidget);
	}

}

void UDialogueQuestList::Close()
{
	QuestLists->ClearChildren();
}

void UDialogueQuestList::OnSlotClicked(UQuestData* questData)
{
	ownerUI->OpenDialogueUI(questData);
}
