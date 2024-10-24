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

	UFUNCTION()
		void OnClickedChatBtn();
	UFUNCTION()
		void PlayLevelUpWidgetAnim();

	void Init(bool isFirst);

	// 퀘스트UI와 인벤토리 UI 정리 - 일단 사용안함
	void Clear();

	// 
	void Reset();

	// 체력UI 업데이트
	void UpdateHp();
	
	// 경험치UI 업데이트
	void UpdateExp();
	
	// 레벨UI 업데이트
	void UpdateLevel();
	
	// 채팅UI 업데이트
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

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UInteractWidget* InteractUI;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UDialogueWidget* DialogueUI;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UQuestLogWidget* QuestUI;

	UPROPERTY(meta = (BindWidget))
		class UButton* ChatBtn;
public:
	class UMyGameInstance* _ownerInstance;
};
