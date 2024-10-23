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
	// ���� ���׵��� Ȯ�� �� ������� �ʴ� ����Ʈ�鸸 �߰�(����Ʈ�Ŵ������� �̹� ����, ����Ʈ�� ���ѻ���)
	// ����� �ϴ� ����Ʈ ��ü�� ������ üũ ���ϰ�, �ش� ����Ʈ�� �ߺ��ؼ� ǥ�⸸ ���ϰ�

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// ���� ��ȣ�ۿ��ϰ� �ִ� NPC�� ����Ʈ ���
	for (int questId : ownerUI->npcData->quests) {

		// ����Ʈ �Ŵ������� ���� ����(���� �̹� �����ϰų�, �� ���� ����)
		if (instance->_questManager->Contains(questId) == true)
			continue;

		// ����Ʈ�� ���ѻ���
		auto questDataPtr = instance->_dataManager->_questTable.Find(questId);
		if (questDataPtr == nullptr)
			continue;

		UQuestData* questData = (*questDataPtr);
		// TODO : 

		// �߰� (������� ���� �ȵ�)
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
