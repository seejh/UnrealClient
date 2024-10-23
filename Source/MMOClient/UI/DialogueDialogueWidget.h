// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueDialogueWidget.generated.h"

class UTextBlock;
class UButton;
class UQuestData;
UCLASS()
class MMOCLIENT_API UDialogueDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void Init(UQuestData* QuestData, bool complete = false);
	void Close();
	
	UFUNCTION()
	void OnAcceptBtnClicked();
	UFUNCTION()
	void OnNextBtnClicked();

public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* DialogueTxt;
	UPROPERTY(meta = (BindWidget))
		UButton* AcceptBtn;
	UPROPERTY(meta = (BindWidget))
		UButton* NextBtn;
public:
	class UDialogueWidget* ownerUI;
	UQuestData* questData;
};
