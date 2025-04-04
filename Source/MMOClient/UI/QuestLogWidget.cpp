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
	// ���� �����ϰ� �� �� �Ҹ��� �ڵ��, ����Ʈ UI�� Ȱ��ȭ�� �� ����Ʈ �Ŵ����κ��� ����Ʈ ������ �ε�

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
	// ����Ʈ ������ �޾Ƽ� �� UI�� ����Ʈ�� ���� ������ �߰�(����Ʈ ��ưUI, ����Ʈ ����UI)
	// ����Ʈ ��ư�� UI�� ��Ƽ� �ش� ��ư�� ���� �� ���ο��� UI�� ��ȸ�ؼ� ����.

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());
	
	// ������UI ����
	UQuestLogContentWidget* contentWidget = Cast<UQuestLogContentWidget>(CreateWidget(GetWorld(), instance->_uiManager->QuestLogContentClass));
	contentWidget->SetContent(quest);
	// �߰�
	Switcher->AddChild(contentWidget);
	
	// ��ưUI ����
	UQuestLogButton* btnWidget = Cast<UQuestLogButton>(CreateWidget(GetWorld(), instance->_uiManager->QuestLogButtonClass));
	btnWidget->Txt->SetText(FText::FromString(quest->_questData->name));
	btnWidget->contentUI = contentWidget;
	btnWidget->ownerUI = this;
	// �߰�
	if (quest->_questInfo.completed) 
		CompleteList->AddChild(btnWidget);
	else 
		ProgressList->AddChild(btnWidget);

	// ����Ʈ ������ UI ����
	quest->QuestBtnUI = btnWidget;
	quest->QuestContentUI = contentWidget;
}

void UQuestLogWidget::RemoveQuestUI(UQuest* quest)
{
	// UI�� ����

	// ��ư
	if (quest->_questInfo.completed)
		CompleteList->RemoveChild(quest->QuestBtnUI);
	else
		ProgressList->RemoveChild(quest->QuestBtnUI);

	// ����Ʈ
	Switcher->RemoveChild(quest->QuestContentUI);
}

void UQuestLogWidget::CompleteQuestUI(UQuest* quest)
{
	// ��ưUI : ������ -> �Ϸ� UI�� �̵� 
	// ����UI : �Ϸ�� ����Ʈ ǥ��

	if (quest->_questInfo.completed) {
		// �ƹ������� ���䵥

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
	// Ư�� ����Ʈ ��ư�� ������ �ش� ��ư���� �����ϰ� �ִ� UI�� ����� �Ѱ��ְ� ���⼱ ����Ī�� ����

	Switcher->SetActiveWidget(contentWidget);
}

FReply UQuestLogWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// PC�� ��ǲ �׼��� �޾Ƽ� â ���Ḧ �ϱ� ���� �κ�

	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	UMyGameInstance* instance = Cast<UMyGameInstance>(GetGameInstance());

	if (InKeyEvent.GetKey() == EKeys::Q) 
		instance->_playerController->OpenQuest();

	return FReply::Handled();
}


