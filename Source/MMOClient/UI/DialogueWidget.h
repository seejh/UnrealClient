// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"


class UOverlay;
class UScrollBox;
class UTextBlock;
class UButton;
UCLASS()
class MMOCLIENT_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	// 일단 NPC 번호만 받고, 해당 정보로 세팅한다고 했을 때
	void Open(int32 npcId);
	void Close();
	
	void OpenGreetingUI();
	void OpenQuestListUI();
	void OpenDialogueUI(class UQuestData* questData, bool complete = false);

	// 버튼 클릭
	UFUNCTION()
		void OnCloseBtnClicked();

public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* TitleTxt;
	UPROPERTY(meta = (BindWidget))
		UButton* CloseBtn;

public:
	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* Switcher;
	UPROPERTY(meta = (BindWidget))
		class UDialogueGreeting* GreetingUI;
	UPROPERTY(meta = (BindWidget))
		class UDialogueQuestList* QuestListUI;
	UPROPERTY(meta = (BindWidget))
		class UDialogueDialogueWidget* DialogueUI;

public:
	class UNpcData* npcData;
	class UMyGameInstance* instance;
};
