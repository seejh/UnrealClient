// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MyHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class MMOCLIENT_API UMyHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
		void OnClickedChatBtn();
	UFUNCTION(BlueprintCallable)
		void PlayLevelUpWidgetAnim();

	void Init();
	void UpdateHp();
	void UpdateExp();
	void UpdateLevel();
	void AddChatMessage(FString& message);

public:
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
		class UWidgetAnimation* LevelUpWidgetAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* LevelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* HPBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* HpText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* ExpBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ExpText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UScrollBox* ChatScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UEditableTextBox* ChatEditableTextBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class URespawnWidget* RespawnUI;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UInventoryWidget* InventoryUI;

	UPROPERTY(meta = (BindWidget))
		class UButton* ChatBtn;
};
