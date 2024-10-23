// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestLogButton.h"
#include<Components/Button.h>
#include<Components/TextBlock.h>
#include"QuestLogWidget.h"
#include"QuestLogContentWidget.h"

void UQuestLogButton::NativeOnInitialized()
{
	Btn->OnReleased.AddDynamic(this, &UQuestLogButton::OnBtnClicked);
}

// void UQuestLogButton::NativeConstruct(){}

void UQuestLogButton::OnBtnClicked()
{
	if (IsValid(contentUI) == false) {
		UE_LOG(LogTemp, Error, TEXT("ContentUI Invalid"));
		return;
	}

	ownerUI->OnBtnClicked(contentUI);
}
