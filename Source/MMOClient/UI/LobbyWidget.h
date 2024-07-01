// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"../Proto/Protocol3.pb.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"


class UButton;
UCLASS()
class MMOCLIENT_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void Init();
	void AddSlotList(PROTOCOL::ObjectInfo info);

	UFUNCTION(BlueprintCallable)
	void OnSlotClicked(int32 slotNum);

	UFUNCTION(BlueprintCallable)
	void OnCreateBtnClicked();
	
public:
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* CharacterSlotList;

	UPROPERTY(meta = (BindWidget))
	UButton* CreateBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelBtn;

	UPROPERTY()
	class UCreateCharacterWidget* PopUpWidget;
public:
	bool isRequested = false;
	FTimerHandle _handle;
};

