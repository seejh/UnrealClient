// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvenSlotWidget.generated.h"


/*------------------------------------------------------------------------------
	UMyGameInstance�� �ִ� C++ �κ��丮�� �� �ִ� ������(UItem*)�� �����ϸ�
	�ش� ������ �ܺο��� ���� �� �� ������ ������Ʈ(UI) �ϴ� ������� ���
------------------------------------------------------------------------------*/
class UItem;
class UInventoryWidget;
UCLASS()
class MMOCLIENT_API UInvenSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// ���������̳�����, �����ε�������, ������������, �⺻����������
	void Init(UInventoryWidget* owner, int32 SlotIndex);
	void SetItem();

	// Ÿ�̸� �׳� Ȱ�� ���ϴ°ɷ�
	void SetRequestTimer(bool flag);
	void SetUseTimer();

	UFUNCTION(BlueprintCallable)
	void UpdateUI();

	UFUNCTION(BlueprintCallable)
	void RequestUseItem();

public:
	//UPROPERTY(BlueprintReadWrite)
	UItem* item = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* slotThumbnail = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* slotStatus;

	UPROPERTY(meta = (BindWidget))
	class UButton* tooltipBtn;

	UPROPERTY()
	class USlotTooltipWidget* slotTooltipUI;

	UPROPERTY()
	int32 slotIndex;

	UPROPERTY()
	class UTexture2D* emptySlotTexture;

public:
	// 
	UInventoryWidget* _ownerWidget;

	// Ÿ�̸�
	FTimerHandle useTimer;
	FTimerHandle requestTimer;
	bool isRequested;
	bool isUseTime = true;

	static FString NoneThumbnailPath;
};
