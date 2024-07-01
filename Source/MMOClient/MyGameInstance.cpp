// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include<Kismet/GameplayStatics.h>
#include"UI/LobbyWidget.h"
#include"UI/LoginWidget.h"
#include"UI/InventoryWidget.h"
#include"UI/InvenSlotWidget.h"
#include"UI/SlotTooltipWidget.h"
#include"UI/MyHUDWidget.h"
#include"UI/CharacterSlotWidget.h"
#include"UI/CreateCharacterWidget.h"
#include"UI/RespawnWidget.h"

#include"MyPlayerController.h"
#include"LoginController.h"
#include"NetSession.h"
#include"ServerPacketHandler.h"
#include"Data/DataManager.h"
#include"Data/GameStruct.h"
#include"GameObject/ObjectManager.h"

#include<TimerManager.h>
#include "Blueprint/WidgetLayoutLibrary.h"


UMyGameInstance::UMyGameInstance()
{
    // �κ�
    static ConstructorHelpers::FClassFinder<ULobbyWidget> LobbyWidgetAsset(TEXT("/Game/UI/WB_LobbyWidget"));
    if (LobbyWidgetAsset.Succeeded())
        LobbyWidgetClass = LobbyWidgetAsset.Class;

    // �κ� ĳ�� ����
    static ConstructorHelpers::FClassFinder<UCharacterSlotWidget> CharacterSlotWidgetAsset(TEXT("/Game/UI/WBP_CharacterSlot"));
    if (CharacterSlotWidgetAsset.Succeeded())
        CharacterSlotWidgetClass = CharacterSlotWidgetAsset.Class;

    // �κ� ĳ�� ����
    static ConstructorHelpers::FClassFinder<UCreateCharacterWidget> CreateCharacterWidgetAsset(TEXT("/Game/UI/WBP_CreateCharacter"));
    if (CreateCharacterWidgetAsset.Succeeded())
        CreateCharacterWidgetClass = CreateCharacterWidgetAsset.Class;

    // �κ��丮
    static ConstructorHelpers::FClassFinder<UInventoryWidget> InventoryWidgetAsset(TEXT("/Game/UI/WBP_Inventory"));
    if (InventoryWidgetAsset.Succeeded())
        InventoryWidgetClass = InventoryWidgetAsset.Class;

    // �κ� ����
    static ConstructorHelpers::FClassFinder<UInvenSlotWidget> InvenSlotWidgetAsset(TEXT("/Game/UI/WBP_InvenSlot"));
    if (InvenSlotWidgetAsset.Succeeded())
        InvenSlotWidgetClass = InvenSlotWidgetAsset.Class;

    // �κ� ���� ����
    static ConstructorHelpers::FClassFinder<USlotTooltipWidget> SlotTooltipWidgetAsset(TEXT("/Game/UI/WBP_SlotToolTip"));
    if (SlotTooltipWidgetAsset.Succeeded())
        SlotTooltipWidgetClass = SlotTooltipWidgetAsset.Class;

    // ������
    static ConstructorHelpers::FClassFinder<URespawnWidget> RespawnWidgetAsset(TEXT("/Game/UI/WBP_ReSpawn"));
    if (RespawnWidgetAsset.Succeeded())
        RespawnWidgetClass = RespawnWidgetAsset.Class;

    // HUD
    static ConstructorHelpers::FClassFinder<UMyHUDWidget> HudWidgetAsset(TEXT("/Game/UI/WB_MyHUDWidget"));
    if (HudWidgetAsset.Succeeded()) 
        MyHUDWidgetClass = HudWidgetAsset.Class;
}

void UMyGameInstance::Init()
{
    Super::Init();

    // �����͸Ŵ��� ����
    _dataManager = NewObject<UDataManager>();
    if (IsValid(_dataManager) == false) {
        UE_LOG(LogTemp, Error, TEXT("GameInstance::Init - Invalid DataManager"));
        return;
    }
    // �����͸Ŵ��� Init(������ ������ �ε�)
    if (_dataManager->Init() == false) {
        return;
    }

    //��Ʈ��ũ����, ��Ŷ�ڵ鷯
    _netSession = MakeShareable(new FNetSession(this));
    _packetHandler = MakeShareable(new FServerPacketHandler());
    _packetHandler->Init();

    //// ������ ����
    if (!_netSession->Connect()) 
        return;
    
    _playerState = PROTOCOL::PlayerServerState::SERVER_STATE_CONNECTED;
}

void UMyGameInstance::BeginDestroy()
{
    Super::BeginDestroy();

    UE_LOG(LogTemp, Error, TEXT("GameInstance BeginDestory"));

    if (_netSession)
        _netSession->Stop();
}

void UMyGameInstance::EnterLobby()
{
    // �� ����
    _playerState = PROTOCOL::PlayerServerState::SERVER_STATE_LOGIN;

    // �α��� -> �κ�
    // �α��� ���� ����
    _nowWidget->RemoveFromParent();
    _nowWidget = nullptr;

    // �κ� ���� ����
    _nowWidget = CreateWidget(GetWorld(), LobbyWidgetClass);
    Cast<ULobbyWidget>(_nowWidget)->Init();
    _nowWidget->AddToViewport();
}

void UMyGameInstance::TryEnterRoom()
{
    // ��Ŷ ����
    PROTOCOL::C_Enter_Room toPkt;
    toPkt.set_roomnum(1);
    toPkt.mutable_object()->set_name(_myCharacterInfo->name());

    // ��Ŷ ����
    auto sendBuffer = _packetHandler->MakeSendBuffer(toPkt);
    _netSession->Send(sendBuffer);
}

void UMyGameInstance::HandleLogin(PROTOCOL::S_Login fromPkt)
{
    if (fromPkt.success() == false) {
        UE_LOG(LogTemp, Error, TEXT("Failed Login to Server"));
        return;
    }

    UE_LOG(LogTemp, Error, TEXT("Login to Server OK"));

    // �� ĳ������Ʈ
    for (int i = 0; i < fromPkt.objectinfos_size(); i++) 
        _myCharacterList.Add(fromPkt.objectinfos(i));

    EnterLobby();
}

void UMyGameInstance::HandleEnterRoom(PROTOCOL::ObjectInfo info)
{
    // ���� ���� üũ : 1.�κ񿡼� ù ���� / 2.���忡�� �� ����
    // �� �κ�, ù ����
    if (_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_LOGIN) {
        UE_LOG(LogTemp, Error, TEXT("HandleEnterRoom - LoginState"));

        // ���� ����
        _playerState = PROTOCOL::PlayerServerState::SERVER_STATE_GAME;

        // ���� �÷��� ĳ���� �޸� �Ҵ� �� ���� ����
        _myCharacterInfo = new PROTOCOL::ObjectInfo();
        _myCharacterInfo->CopyFrom(info);

        // �κ� ���� ����
        _nowWidget->RemoveFromParent();
        _nowWidget = nullptr;

        UGameplayStatics::OpenLevel(GetWorld(), *FString(L"/Game/NewMap"), false, "");
    }

    // �� ����, �� ����
    else {
        _myCharacter->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

        // TODO : ����
        // ���� ����, hp ���� ������Ʈ, ��ġ ����
        _myCharacterInfo->CopyFrom(info);
        _hudWidget->UpdateHp();
        _myCharacter->SetActorLocation(FVector(info.pos().locationx(), info.pos().locationy(), info.pos().locationz()));

        // ������ ���� ����, �ٽ� Ȱ��ȭ, ���콺 ����
        _hudWidget->RespawnUI->SetVisibility(ESlateVisibility::Hidden);
        _hudWidget->RespawnUI->RespawnBtn->SetIsEnabled(true);
        _gameController->SetShowMouseCursor(false);
        
        // ��ġ ����ȭ Ȱ��ȭ
        _isMeAlive = true;

        // ĳ���� �Է� Ȱ��ȭ
        _myCharacter->EnableInput(_gameController);
    }
}

void UMyGameInstance::HandleItemList(PROTOCOL::S_ItemList fromPkt)
{
    
    PROTOCOL::S_AddItem testPkt;
    testPkt.mutable_items()->CopyFrom(fromPkt.items());

    for (int i = 0; i < fromPkt.items_size(); i++) {
        // ������ ������ ��ȸ
        UItemData* itemData = _dataManager->_itemTable[fromPkt.items(i).templateid()];
        if (IsValid(itemData) == false) {
            UE_LOG(LogTemp, Error, TEXT("HandleItemList Error"));
            return;
        }

        // ������ ����
        UItem* item = nullptr;
        if (itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
            item = NewObject<UConsumable>();
        }
        else if (itemData->itemType == EItemType::ITEM_TYPE_WEAPON) {
            item = NewObject<UWeapon>();
        }
        else if (itemData->itemType == EItemType::ITEM_TYPE_ARMOR) {
            item = NewObject<UArmor>();
        }
        else {
            return;
        }

        // ������ ����
        item->SetItemDB(fromPkt.items(i));
        item->Init(this, fromPkt.items(i).templateid());
        
        // c++ �κ��丮
        _inventory.Add(item);
        
        // UI
        //inventoryWidget->LoadInventory();
    }
}

void UMyGameInstance::HandleUseItem(PROTOCOL::S_UseItem fromPkt)
{
    // �κ� ������
    for (UItem* item : _inventory) {
        // �ش� ������ ã�Ƽ�
        if (item->itemDB.itemDbId == fromPkt.itemdbid()) {
            
            // c++
            // �Ҹ�ǰ
            if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
                if (--item->itemDB.count <= 0) {
                    // �׸��� ���� �������� : UI�� ����� �� �����
                    // UI �귯�� �����ǰ� ���� ��Ŭ���ϸ� ����

                    // TODO : �ش� ������ �ı�, ���� ����� �� ���ƺ���
                    _hudWidget->InventoryUI->GetSlotAt(item->itemDB.slot)->SetItem(nullptr);
                    _inventory.Remove(item);
                }
            }
            // ����
            else {
                item->itemDB.equipped = fromPkt.use();

                _hudWidget->InventoryUI->UpdateDamageText();
                _hudWidget->InventoryUI->UpdateArmorText();
            }

            // ui
            UInvenSlotWidget* slotWidget = _hudWidget->InventoryUI->GetSlotAt(item->itemDB.slot);
            slotWidget->UpdateWidget();
            slotWidget->isRequested = false;

            return;
        }
    }
}

void UMyGameInstance::AddItem(PROTOCOL::ItemInfo itemInfo)
{
    // ������ ������ ��ȸ
    // UItemData* itemData = nullptr;
    UItemData** ItemDataPtr = _dataManager->_itemTable.Find(itemInfo.templateid());
    if (ItemDataPtr == nullptr) {
        // if (IsValid((*ItemDataPtr)) == false) return;
        UE_LOG(LogTemp, Error, TEXT("Instance::AddItem() - nullptr"));
        return;
    }

    // ������
    UItem* item = nullptr;

    // �Ҹ�ǰ
    if ((*ItemDataPtr)->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
        // 
        for (UItem* invenItem : _inventory) {
            // ���� �Ҹ�ǰ ����
            if (invenItem->itemDB.templateId == itemInfo.templateid()) {
                // ������  ����
                invenItem->itemDB.count = itemInfo.count();

                // UI ������Ʈ
                if (IsValid(_hudWidget) && IsValid(_hudWidget->InventoryUI))
                    _hudWidget->InventoryUI->SetInvenSlot(invenItem);

                // 
                return;
            }
        }

        // �Ҹ�ǰ ���� ����
        item = NewObject<UConsumable>();
    }

    // ����
    else if ((*ItemDataPtr)->itemType == EItemType::ITEM_TYPE_WEAPON)
        item = NewObject<UWeapon>();
    // ��
    else if ((*ItemDataPtr)->itemType == EItemType::ITEM_TYPE_ARMOR)
        item = NewObject<UArmor>();
    else 
        return;

    // ������ ����
    item->SetItemDB(itemInfo);
    item->Init(this, itemInfo.templateid());

    // c++ �κ��� �߰�
    _inventory.Add(item);
    
    // UI ������Ʈ
    if (IsValid(_hudWidget) && IsValid(_hudWidget->InventoryUI)) 
        _hudWidget->InventoryUI->SetInvenSlot(item);
}

void UMyGameInstance::HandleChangeStat(PROTOCOL::ObjectInfo info)
{
    // Data
    _myCharacterInfo->mutable_stat()->CopyFrom(info.stat());
    
    // UI
    // Level
    _hudWidget->UpdateLevel();

    // Hp
    _hudWidget->UpdateHp();

    // Exp
    _hudWidget->UpdateExp();
    
    // damage, defence - �⺻ ��,��
    _hudWidget->InventoryUI->UpdateDamageText();
    // _hudWidget->InventoryUI->UpdateArmorText();
}

void UMyGameInstance::HandleCreatePlayer(PROTOCOL::ObjectInfo info)
{
    // TODO : ���°� �κ����� üũ
    
    // ���� ���� = �κ�����
    ULobbyWidget* lobby = Cast<ULobbyWidget>(_nowWidget);

    // ����
    if (info.name().compare("ExistName") == 0) {
        lobby->PopUpWidget->FailReason->SetText(FText::FromString("Fail : ExistName"));
    }
    else if (info.name().compare("Fail") == 0) {
        lobby->PopUpWidget->FailReason->SetText(FText::FromString("Fail"));
    }
    // ����
    else {
        // ĳ���� ����Ʈ�� �߰�
        _myCharacterList.Add(info);

        // �˾� �ݰ�
        lobby->PopUpWidget->RemoveFromParent();
        lobby->PopUpWidget = nullptr;

        // Lobby ������Ʈ
        lobby->AddSlotList(info);
    }
}

void UMyGameInstance::HandleAddExp(int32 exp)
{
    // Data
    _myCharacterInfo->mutable_stat()->set_exp(exp);

    // UI
    _hudWidget->UpdateExp();
}
