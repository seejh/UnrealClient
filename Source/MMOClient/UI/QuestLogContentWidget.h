// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestLogContentWidget.generated.h"

class UTextBlock;
class UQuest;
UCLASS()
class MMOCLIENT_API UQuestLogContentWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCancelBtnClicked();

	void SetContent(UQuest* _quest);
	void Update();
public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* DescriptionTxt;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ProgressTxt;
	UPROPERTY(meta = (BindWidget))
		class UButton* CancelBtn;
public:
	UQuest* quest;
};
