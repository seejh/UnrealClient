// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueGreeting.generated.h"

/**
 * 
 */
UCLASS()
class MMOCLIENT_API UDialogueGreeting : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void OnNextBtnClicked();

	void Init();
	void Close();
public:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Txt;
	UPROPERTY(meta = (BindWidget))
		class UButton* NextBtn;
public:
	class UDialogueWidget* ownerUI;
};
