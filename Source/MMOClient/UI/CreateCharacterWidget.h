// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreateCharacterWidget.generated.h"



UCLASS()
class MMOCLIENT_API UCreateCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OnCreateBtnClicked();

	UFUNCTION(BlueprintCallable)
	void OnCancelBtnClicked();
public:
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Name;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateBtn;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelBtn;
};
