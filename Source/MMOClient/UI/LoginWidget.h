// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"



#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class MMOCLIENT_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
		void OnClickedLogin();
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UEditableTextBox* IdEditableTextBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UEditableTextBox* PwEditableTextBox;
	UPROPERTY(meta = (BindWidget))
		class UButton* LoginBtn;

public:

public:
	FTimerHandle _handle;
};
