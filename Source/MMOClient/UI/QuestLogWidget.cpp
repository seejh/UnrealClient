// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestLogWidget.h"
#include<Components/ScrollBox.h>
#include<Components/WidgetSwitcher.h>
#include"QuestLogButton.h"
#include"QuestLogContentWidget.h"
#include"../MyGameInstance.h"
#include"../UIManager.h"
#include"../QuestManager.h"
#include<Components/TextBlock.h>
#include"../MyPlayerController.h"

void UQuestLogWidget::NativeConstruct()
{
	bIsFocusable = true;
}

void UQuestLogWidget::Init()
{
	// 게임 시작하고 한 번 불리는 코드로, 퀘스트 UI가 활성화될 때 퀘스트 매니저로부터 퀘스트 정보들 로드

	SetVisibility(ESlateVisibility::Hidden);

	for (auto p : Cast<UMyGameInstance>(GetGameInstance())->_questManager->_quests)
		AddQuestUI(p.Value);
}

void UQuestLogWidget::Clear()
{
	ProgressList->ClearChildren();
	CompleteList->ClearChildren();
	Switcher->ClearChildren();
}

void UQuestLogWidget::AddQuestUI(UQuest* quest)
{
	// 퀘스트 정보를 받아서 이 UI에 퀘스트에 대한 위젯들 추가(퀘스트 버튼UI, 퀘스트 내용UI)
	// 퀘스트 버튼에 UI를 담아서 해당 버튼이 눌릴 시 내부에서 UI를 조회해서 띄운다.

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	
	// 컨텐츠UI 생성
	UQuestLogContentWidget* contentWidget = Cast<UQuestLogContentWidget>(CreateWidget(GetWorld(), instance->_uiManager->QuestLogContentClass));
	contentWidget->SetContent(quest);
	// 추가
	Switcher->AddChild(contentWidget);
	
	// 버튼UI 생성
	UQuestLogButton* btnWidget = Cast<UQuestLogButton>(CreateWidget(GetWorld(), instance->_uiManager->QuestLogButtonClass));
	btnWidget->Txt->SetText(FText::FromString(quest->_questData->name));
	btnWidget->contentUI = contentWidget;
	btnWidget->ownerUI = this;
	// 추가
	if (quest->_questInfo.completed) 
		CompleteList->AddChild(btnWidget);
	else 
		ProgressList->AddChild(btnWidget);

	// 퀘스트 정보에 UI 참조
	quest->QuestBtnUI = btnWidget;
	quest->QuestContentUI = contentWidget;
}

void UQuestLogWidget::RemoveQuestUI(UQuest* quest)
{
	// UI만 삭제

	// 버튼
	if (quest->_questInfo.completed)
		CompleteList->RemoveChild(quest->QuestBtnUI);
	else
		ProgressList->RemoveChild(quest->QuestBtnUI);

	// 컨텐트
	Switcher->RemoveChild(quest->QuestContentUI);
}

void UQuestLogWidget::CompleteQuestUI(UQuest* quest)
{
	// 버튼UI : 진행중 -> 완료 UI로 이동 
	// 내용UI : 완료된 퀘스트 표기

	if (quest->_questInfo.completed) {
		// 아무리봐도 여긴데

		ProgressList->RemoveChild(quest->QuestBtnUI);
		CompleteList->AddChild(quest->QuestBtnUI);

		quest->QuestContentUI->Update();
	}
}

void UQuestLogWidget::UpdateUI(UQuest* quest)
{
	quest->QuestContentUI->Update();
}

void UQuestLogWidget::OnBtnClicked(UQuestLogContentWidget* contentWidget)
{
	// 특정 퀘스트 버튼이 눌리면 해당 버튼에서 참조하고 있는 UI를 여기로 넘겨주고 여기선 스위칭만 진행

	Switcher->SetActiveWidget(contentWidget);
}

FReply UQuestLogWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// PC의 인풋 액션을 받아서 창 종료를 하기 위한 부분

	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	if (InKeyEvent.GetKey() == EKeys::Q) 
		instance->_playerController->OpenQuest();

	return FReply::Handled();
}


