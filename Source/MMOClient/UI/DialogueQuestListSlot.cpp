// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueQuestListSlot.h"
#include"../Data/GameStruct.h"
#include<Components/Button.h>
#include<Components/TextBlock.h>
#include"DialogueQuestList.h"
#include"../MyGameInstance.h"
#include"../DataManager.h"


void UDialogueQuestListSlot::NativeConstruct()
{
	Btn->OnReleased.AddDynamic(this, &UDialogueQuestListSlot::OnBtnClicked);
}

void UDialogueQuestListSlot::Init(UDialogueQuestList* OwnerUI, UQuestData* QuestData)
{
	ownerUI = OwnerUI;
	questData = QuestData;

	Txt->SetText(FText::FromString(questData->name));
}

void UDialogueQuestListSlot::OnBtnClicked()
{
	ownerUI->OnSlotClicked(questData);
}
