// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UIManager.generated.h"


/*
�����ڴ� default���� �Ѵ�.
ConstructorHelpers�� �����ڿ��� �� �� �ִ�.
*/
class UMyGameInstance;
UCLASS()
class MMOCLIENT_API UUIManager : public UObject
{
	GENERATED_BODY()
public:
	UUIManager();
	
	UUserWidget* CreateMainUI(TSubclassOf<UUserWidget> widgetClass);

public:
	TSubclassOf<UUserWidget> LoginClass;
	TSubclassOf<UUserWidget> LobbyClass;
	TSubclassOf<UUserWidget> CharacterSlotClass;
	TSubclassOf<UUserWidget> CreateCharacterClass;
	TSubclassOf<UUserWidget> InventoryClass;
	TSubclassOf<UUserWidget> InventorySlotClass;
	TSubclassOf<UUserWidget> SlotTooltipClass;
	TSubclassOf<UUserWidget> RespawnClass;
	TSubclassOf<UUserWidget> InGameUIClass;
	TSubclassOf<UUserWidget> InteractUIClass;
	TSubclassOf<UUserWidget> DialogueUIClass;
	TSubclassOf<UUserWidget> DialogueQuestListSlotClass;
	TSubclassOf<UUserWidget> ServerListClass;
	TSubclassOf<UUserWidget> ServerListSlotClass;
	TSubclassOf<UUserWidget> QuestLogButtonClass;
	TSubclassOf<UUserWidget> QuestLogContentClass;

	UPROPERTY()
	UUserWidget* _mainUI;
	
	UMyGameInstance* _ownerInstance;
};
