// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include<Components/Button.h>
#include<Components/TextBlock.h>
#include"../Proto/Protocol3.pb.h"

#include "Blueprint/UserWidget.h"
#include "CharacterSlotWidget.generated.h"


class ULobbyWidget;
UCLASS()
class MMOCLIENT_API UCharacterSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	void Init(ULobbyWidget* lobbyWidget, PROTOCOL::ObjectInfo info, int32 slotNumber);

	UFUNCTION(BlueprintCallable)
	void OnEnterBtnClicked();
public:
	UPROPERTY(meta = (BindWidget))
	UButton* EnterBtn;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Level;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Name;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Damage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HP;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EXP;
public:
	ULobbyWidget* ownerLobbyWidget;
	int32 slotNum = -1;
};
