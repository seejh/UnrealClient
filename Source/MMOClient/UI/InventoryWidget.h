// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

enum {
	MAX_INVEN_SIZE = 30,
};

/*------------------------------------------------------------------------
	InventoryGrid(UWrapBox)에 슬롯UI들을 담아서 사용
	슬롯UI를 미리 생성해놓고 사용, 슬롯 번호 순으로 적재
	
-------------------------------------------------------------------------*/
class UTextBlock;
class UInvenSlotWidget;
class UItem;
UCLASS()
class MMOCLIENT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 인벤토리에 슬롯UI 미리 생성, 슬롯에 c++인벤 아이템들 설정
	UFUNCTION(BlueprintCallable)
	void Init(bool isFirst);

	// 생성되어 있는 슬롯칸들은 유지하면서 내용물들만 제거
	void Clear();

	UInvenSlotWidget* GetSlotAt(int32 slot);

	// C++ 데이터 업데이트 후 해당 슬롯UI 업데이트
	void SetSlot(int32 slot);
	void UpdateSlot(int32 slot);

	void UpdateDamageText();
	void UpdateArmorText();

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWrapBox* InventoryGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DamageText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ArmorText;

public:
	class UMyGameInstance* _ownerInstance;
};
