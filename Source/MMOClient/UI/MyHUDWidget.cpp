// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUDWidget.h"

#include<Components/TextBlock.h>
#include<Components/ProgressBar.h>
#include<Components/ScrollBox.h>
#include<Components/EditableTextBox.h>
#include<Components/Button.h>

#include"../MyGameInstance.h"
#include"../MyPlayerController.h"
#include"../DataManager.h"
#include"InventoryWidget.h"
#include"RespawnWidget.h"
#include"InteractWidget.h"
#include"DialogueWidget.h"
#include"QuestLogWidget.h"

void UMyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ChatBtn->OnReleased.AddDynamic(this, &UMyHUDWidget::OnClickedChatBtn);
}

void UMyHUDWidget::Init(bool isFirst)
{
	_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());

	// 인벤토리UI
	InventoryUI->Init(isFirst);

	// 상호 작용UI
	InteractUI->SetVisibility(ESlateVisibility::Hidden);

	// 리스폰UI
	RespawnUI->Init();

	// 대화창UI
	DialogueUI->SetVisibility(ESlateVisibility::Hidden);

	// 퀘스트UI
	QuestUI->Init();

	// 레벨
	UpdateLevel();

	// 체력바
	UpdateHp();

	// 경험치바
	UpdateExp();
}

void UMyHUDWidget::Clear()
{
	// 퀘스트 UI, 인벤토리 UI 정리

	// 퀘스트UI 정리
	QuestUI->Clear();

	// 인벤토리UI 정리
	InventoryUI->Clear();

	// 채팅창 정리
	ChatScrollBox->ClearChildren();
}

void UMyHUDWidget::Reset()
{
	// 리스폰 위젯
	RespawnUI->Init();

	// hp 업데이트
	UpdateHp();
}

void UMyHUDWidget::UpdateHp()
{
	// 체력 상태
	const int32 hp = _ownerInstance->_myCharacterInfo->stat().hp();
	const int32 maxHp = _ownerInstance->_myCharacterInfo->stat().maxhp();

	// UI텍스트
	FString hpStr("HP : ");
	hpStr.Append(FString::FromInt(hp)).
		Append(" / ")
		.Append(FString::FromInt(maxHp));

	HpText->SetText(FText::FromString(hpStr));

	// UIBar
	HPBar->SetPercent((float)hp / (float)maxHp);
}

void UMyHUDWidget::UpdateExp()
{
	// 경험치 상태
	const int32 exp = _ownerInstance->_myCharacterInfo->stat().exp();
	const int32 totalExp = _ownerInstance->_myCharacterInfo->stat().totalexp();

	// UI텍스트
	FString expStr("EXP : ");
	expStr.Append(FString::FromInt(exp))
		.Append(" / ")
		.Append(FString::FromInt(totalExp));

	ExpText->SetText(FText::FromString(expStr));

	// UIBar
	// 현 경험치 - 이전레벨 맥스경험치 / 현레벨 맥스경험치 - 이전레벨 맥스경험치
	
	// 전 레벨 맥스 경험치
	int32 prevMax = _ownerInstance->_dataManager->_statTable[_ownerInstance->_myCharacterInfo->stat().level() - 1].totalExp;
	
	ExpBar->SetPercent((float)(exp - prevMax) / (float)(totalExp - prevMax));
}

void UMyHUDWidget::UpdateLevel()
{
	// UI텍스트
	FString levelStr("LV.");
	levelStr.Append(FString::FromInt(_ownerInstance->_myCharacterInfo->stat().level()));

	LevelText->SetText(FText::FromString(levelStr));
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
	// 입력칸에서 작성된 텍스트 가져온다.
	FString chatMessage(ChatEditableTextBox->GetText().ToString());
	if (chatMessage.IsEmpty())
		return;

	// 패킷
	_ownerInstance->_playerController->MyPlayerChat(chatMessage);
	
	// 입력칸 초기화
	ChatEditableTextBox->SetText(FText::GetEmpty());
}

void UMyHUDWidget::PlayLevelUpWidgetAnim()
{
	PlayAnimation(LevelUpWidgetAnim);
}
