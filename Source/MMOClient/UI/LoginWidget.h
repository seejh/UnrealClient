// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"../Data/GameStruct.h"
#include "LoginWidget.generated.h"

class UServerListWidget;
class UEditableTextBox;
class UButton;
UCLASS()
class MMOCLIENT_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
		void OnClickedLogin();
	void CloseServerList();
	void OpenServerList();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* IdEditableTextBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* PwEditableTextBox;
	UPROPERTY(meta = (BindWidget))
		UButton* LoginBtn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UServerListWidget* serverListPopUp;
public:
	FTimerHandle _handle;
};
