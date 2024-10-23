// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGreeting.h"
#include<Components/TextBlock.h>
#include<Components/Button.h>
#include"DialogueWidget.h"
#include"DialogueQuestList.h"
#include"../Data/GameStruct.h"


void UDialogueGreeting::NativeConstruct()
{
	NextBtn->OnReleased.AddDynamic(this, &UDialogueGreeting::OnNextBtnClicked);
}

void UDialogueGreeting::OnNextBtnClicked()
{
	ownerUI->OpenQuestListUI();
}

void UDialogueGreeting::Init()
{
	Txt->SetText(FText::FromString(ownerUI->npcData->greeting));
}

void UDialogueGreeting::Close()
{
}
