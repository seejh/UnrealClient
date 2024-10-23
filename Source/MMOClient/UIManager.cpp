// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"

#include"MyGameInstance.h"
#include"UI/LobbyWidget.h"
#include"UI/CharacterSlotWidget.h"
#include"UI/CreateCharacterWidget.h"
#include"UI/InventoryWidget.h"
#include"UI/InvenSlotWidget.h"
#include"UI/SlotTooltipWidget.h"
#include"UI/RespawnWidget.h"
#include"UI/MyHUDWidget.h"
#include"UI/ServerListWidget.h"
#include"UI/ServerListSlotWidget.h"
#include"UI/InteractWidget.h"
#include"UI/DialogueWidget.h"
#include"UI/DialogueQuestListSlot.h"
#include"UI/QuestLogButton.h"
#include"UI/QuestLogContentWidget.h"

UUIManager::UUIManager() {
    // 로비
    static ConstructorHelpers::FClassFinder<ULobbyWidget> LobbyWidgetAsset(TEXT("/Game/UI/WB_LobbyWidget"));
    if (LobbyWidgetAsset.Succeeded())
        LobbyClass = LobbyWidgetAsset.Class;
    
    // 로비 캐릭 슬롯
    static ConstructorHelpers::FClassFinder<UCharacterSlotWidget> CharacterSlotWidgetAsset(TEXT("/Game/UI/WBP_CharacterSlot"));
    if (CharacterSlotWidgetAsset.Succeeded())
        CharacterSlotClass = CharacterSlotWidgetAsset.Class;
    
    // 로비 캐릭 생성
    static ConstructorHelpers::FClassFinder<UCreateCharacterWidget> CreateCharacterWidgetAsset(TEXT("/Game/UI/WBP_CreateCharacter"));
    if (CreateCharacterWidgetAsset.Succeeded())
        CreateCharacterClass = CreateCharacterWidgetAsset.Class;
    
    // 인벤토리
    static ConstructorHelpers::FClassFinder<UInventoryWidget> InventoryWidgetAsset(TEXT("/Game/UI/WBP_Inventory"));
    if (InventoryWidgetAsset.Succeeded())
        InventoryClass = InventoryWidgetAsset.Class;
    
    // 인벤 슬롯
    static ConstructorHelpers::FClassFinder<UInvenSlotWidget> InvenSlotWidgetAsset(TEXT("/Game/UI/WBP_InvenSlot"));
    if (InvenSlotWidgetAsset.Succeeded())
        InventorySlotClass = InvenSlotWidgetAsset.Class;
    
    // 인벤 슬롯 툴팁
    static ConstructorHelpers::FClassFinder<USlotTooltipWidget> SlotTooltipWidgetAsset(TEXT("/Game/UI/WBP_SlotToolTip"));
    if (SlotTooltipWidgetAsset.Succeeded())
        SlotTooltipClass = SlotTooltipWidgetAsset.Class;
    
    // 리스폰
    static ConstructorHelpers::FClassFinder<URespawnWidget> RespawnWidgetAsset(TEXT("/Game/UI/WBP_ReSpawn"));
    if (RespawnWidgetAsset.Succeeded())
        RespawnClass = RespawnWidgetAsset.Class;
    
    // HUD
    static ConstructorHelpers::FClassFinder<UMyHUDWidget> HudWidgetAsset(TEXT("/Game/UI/WB_MyHUDWidget"));
    if (HudWidgetAsset.Succeeded())
        InGameUIClass = HudWidgetAsset.Class;
    
    // 상호작용
    static ConstructorHelpers::FClassFinder<UInteractWidget> InteractWidgetAsset(TEXT("/Game/UI/WB_Interact"));
    if (InteractWidgetAsset.Succeeded())
        InteractUIClass = InteractWidgetAsset.Class;

    // 대화창
    static ConstructorHelpers::FClassFinder<UDialogueWidget> DialogueWidgetAsset(TEXT("/Game/UI/WB_Dialogue"));
    if (DialogueWidgetAsset.Succeeded())
        DialogueUIClass = DialogueWidgetAsset.Class;

    //
    static ConstructorHelpers::FClassFinder<UDialogueQuestListSlot> DialogueQuestListSlotAsset(TEXT("/Game/UI/WB_DialogueQuestListSlot"));
    if (DialogueQuestListSlotAsset.Succeeded())
        DialogueQuestListSlotClass = DialogueQuestListSlotAsset.Class;

    static ConstructorHelpers::FClassFinder<UServerListWidget> ServerListWidgetAsset(TEXT("/Game/UI/WB_ServerList"));
    if (ServerListWidgetAsset.Succeeded())
        ServerListClass = ServerListWidgetAsset.Class;
    
    static ConstructorHelpers::FClassFinder<UServerListSlotWidget> ServerListSlotWidgetAsset(TEXT("/Game/UI/WB_ServerListSlot"));
    if (ServerListSlotWidgetAsset.Succeeded())
        ServerListSlotClass = ServerListSlotWidgetAsset.Class;

    static ConstructorHelpers::FClassFinder<UQuestLogButton> QuestButtonAsset(TEXT("/Game/UI/WB_QuestLogButton"));
    if (QuestButtonAsset.Succeeded())
        QuestLogButtonClass = QuestButtonAsset.Class;

    static ConstructorHelpers::FClassFinder<UQuestLogContentWidget> QuestContentAsset(TEXT("/Game/UI/WB_QuestLogContent"));
    if (QuestContentAsset.Succeeded())
        QuestLogContentClass = QuestContentAsset.Class;
}

UUserWidget* UUIManager::CreateMainUI(TSubclassOf<UUserWidget> widgetClass)
{
    if (IsValid(_mainUI))
        _mainUI->RemoveFromParent();

    _mainUI = CreateWidget(_ownerInstance->GetWorld(), widgetClass);

    return _mainUI;
}
