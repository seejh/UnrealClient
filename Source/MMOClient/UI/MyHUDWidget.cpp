// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUDWidget.h"

#include<Components/TextBlock.h>
#include<Components/ProgressBar.h>
#include<Components/ScrollBox.h>
#include<Components/EditableTextBox.h>
#include<Components/Button.h>

#include"../MyGameInstance.h"
#include"../MyPlayerController.h"
#include"../Data/DataManager.h"
#include"InventoryWidget.h"
#include"RespawnWidget.h"

void UMyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ChatBtn->OnReleased.AddDynamic(this, &UMyHUDWidget::OnClickedChatBtn);
}

void UMyHUDWidget::Init()
{
	// 인벤토리
	InventoryUI->SetVisibility(ESlateVisibility::Hidden);
	InventoryUI->Init();

	// 리스폰
	RespawnUI->SetVisibility(ESlateVisibility::Hidden);

	// Level
	UpdateLevel();

	// HP
	UpdateHp();

	// Exp
	UpdateExp();
}

void UMyHUDWidget::UpdateHp()
{
	//
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	//
	const int32 hp = instance->_myCharacterInfo->stat().hp();
	const int32 maxHp = instance->_myCharacterInfo->stat().maxhp();

	// UI-텍스트 적용
	FString str("HP : ");
	str.Append(FString::FromInt(hp));
	str.Append(" / ");
	str.Append(FString::FromInt(maxHp));

	HpText->SetText(FText::FromString(str));

	// UI-그래프 적용
	HPBar->SetPercent((float)hp / (float)maxHp);
}

void UMyHUDWidget::UpdateExp()
{
	//
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	
	//
	const int32 exp = instance->_myCharacterInfo->stat().exp();
	const int32 totalExp = instance->_myCharacterInfo->stat().totalexp();

	// UI-텍스트 적용
	FString str("EXP : ");
	str.Append(FString::FromInt(exp));
	str.Append(" / ");
	str.Append(FString::FromInt(totalExp));

	ExpText->SetText(FText::FromString(str));

	// UI-그래프 적용 - 현 경험치 - 전 레벨 맥스 경험치 / 현 레벨 맥스 경험치 - 전 레벨 맥스 경험치
	// 전 레벨 맥스 경험치
	int32 prevMax = instance->_dataManager->_statTable[instance->_myCharacterInfo->stat().level() - 1].totalExp;
	ExpBar->SetPercent((float)(exp - prevMax) / (float)(totalExp - prevMax));
}

void UMyHUDWidget::UpdateLevel()
{
	//
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	// UI-텍스트 적용
	FString str("LV.");
	str.Append(FString::FromInt(instance->_myCharacterInfo->stat().level()));

	LevelText->SetText(FText::FromString(str));
}

void UMyHUDWidget::AddChatMessage(FString& message)
{
	//
	UTextBlock* newTextBlock = NewObject<UTextBlock>(ChatScrollBox);
	newTextBlock->SetText(FText::FromString(message));
	
	//
	ChatScrollBox->AddChild(newTextBlock);
	ChatScrollBox->ScrollToEnd();
}

void UMyHUDWidget::OnClickedChatBtn()
{
	// 입력란 텍스트 가져옴
	FString chatMessage(ChatEditableTextBox->GetText().ToString());
	if (chatMessage.IsEmpty())
		return;

	// 패킷 송신
	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	instance->_gameController->MyPlayerChat(chatMessage);
	
	// 입력란 초기화
	ChatEditableTextBox->SetText(FText::GetEmpty());
}

void UMyHUDWidget::PlayLevelUpWidgetAnim()
{
	PlayAnimation(LevelUpWidgetAnim);
}
