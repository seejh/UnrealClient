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
    // 로비
    static ConstructorHelpers::FClassFinder<ULobbyWidget> LobbyWidgetAsset(TEXT("/Game/UI/WB_LobbyWidget"));
    if (LobbyWidgetAsset.Succeeded())
        LobbyWidgetClass = LobbyWidgetAsset.Class;

    // 로비 캐릭 슬롯
    static ConstructorHelpers::FClassFinder<UCharacterSlotWidget> CharacterSlotWidgetAsset(TEXT("/Game/UI/WBP_CharacterSlot"));
    if (CharacterSlotWidgetAsset.Succeeded())
        CharacterSlotWidgetClass = CharacterSlotWidgetAsset.Class;

    // 로비 캐릭 생성
    static ConstructorHelpers::FClassFinder<UCreateCharacterWidget> CreateCharacterWidgetAsset(TEXT("/Game/UI/WBP_CreateCharacter"));
    if (CreateCharacterWidgetAsset.Succeeded())
        CreateCharacterWidgetClass = CreateCharacterWidgetAsset.Class;

    // 인벤토리
    static ConstructorHelpers::FClassFinder<UInventoryWidget> InventoryWidgetAsset(TEXT("/Game/UI/WBP_Inventory"));
    if (InventoryWidgetAsset.Succeeded())
        InventoryWidgetClass = InventoryWidgetAsset.Class;

    // 인벤 슬롯
    static ConstructorHelpers::FClassFinder<UInvenSlotWidget> InvenSlotWidgetAsset(TEXT("/Game/UI/WBP_InvenSlot"));
    if (InvenSlotWidgetAsset.Succeeded())
        InvenSlotWidgetClass = InvenSlotWidgetAsset.Class;

    // 인벤 슬롯 툴팁
    static ConstructorHelpers::FClassFinder<USlotTooltipWidget> SlotTooltipWidgetAsset(TEXT("/Game/UI/WBP_SlotToolTip"));
    if (SlotTooltipWidgetAsset.Succeeded())
        SlotTooltipWidgetClass = SlotTooltipWidgetAsset.Class;

    // 리스폰
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

    // 데이터매니저 생성
    _dataManager = NewObject<UDataManager>();
    if (IsValid(_dataManager) == false) {
        UE_LOG(LogTemp, Error, TEXT("GameInstance::Init - Invalid DataManager"));
        return;
    }
    // 데이터매니저 Init(아이템 데이터 로드)
    if (_dataManager->Init() == false) {
        return;
    }

    //네트워크세션, 패킷핸들러
    _netSession = MakeShareable(new FNetSession(this));
    _packetHandler = MakeShareable(new FServerPacketHandler());
    _packetHandler->Init();

    //// 서버와 연결
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
    // 현 상태
    _playerState = PROTOCOL::PlayerServerState::SERVER_STATE_LOGIN;

    // 로그인 -> 로비
    // 로그인 위젯 해제
    _nowWidget->RemoveFromParent();
    _nowWidget = nullptr;

    // 로비 위젯 생성
    _nowWidget = CreateWidget(GetWorld(), LobbyWidgetClass);
    Cast<ULobbyWidget>(_nowWidget)->Init();
    _nowWidget->AddToViewport();
}

void UMyGameInstance::TryEnterRoom()
{
    // 패킷 생성
    PROTOCOL::C_Enter_Room toPkt;
    toPkt.set_roomnum(1);
    toPkt.mutable_object()->set_name(_myCharacterInfo->name());

    // 패킷 전송
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

    // 내 캐릭리스트
    for (int i = 0; i < fromPkt.objectinfos_size(); i++) 
        _myCharacterList.Add(fromPkt.objectinfos(i));

    EnterLobby();
}

void UMyGameInstance::HandleEnterRoom(PROTOCOL::ObjectInfo info)
{
    // 현재 상태 체크 : 1.로비에서 첫 진입 / 2.월드에서 재 진입
    // 현 로비, 첫 진입
    if (_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_LOGIN) {
        UE_LOG(LogTemp, Error, TEXT("HandleEnterRoom - LoginState"));

        // 상태 변경
        _playerState = PROTOCOL::PlayerServerState::SERVER_STATE_GAME;

        // 현재 플레이 캐릭터 메모리 할당 및 인포 복사
        _myCharacterInfo = new PROTOCOL::ObjectInfo();
        _myCharacterInfo->CopyFrom(info);

        // 로비 위젯 제거
        _nowWidget->RemoveFromParent();
        _nowWidget = nullptr;

        UGameplayStatics::OpenLevel(GetWorld(), *FString(L"/Game/NewMap"), false, "");
    }

    // 현 월드, 재 진입
    else {
        _myCharacter->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

        // TODO : 수정
        // 인포 복사, hp 위젯 업데이트, 위치 설정
        _myCharacterInfo->CopyFrom(info);
        _hudWidget->UpdateHp();
        _myCharacter->SetActorLocation(FVector(info.pos().locationx(), info.pos().locationy(), info.pos().locationz()));

        // 리스폰 위젯 숨김, 다시 활성화, 마우스 제거
        _hudWidget->RespawnUI->SetVisibility(ESlateVisibility::Hidden);
        _hudWidget->RespawnUI->RespawnBtn->SetIsEnabled(true);
        _gameController->SetShowMouseCursor(false);
        
        // 위치 동기화 활성화
        _isMeAlive = true;

        // 캐릭터 입력 활성화
        _myCharacter->EnableInput(_gameController);
    }
}

void UMyGameInstance::HandleItemList(PROTOCOL::S_ItemList fromPkt)
{
    
    PROTOCOL::S_AddItem testPkt;
    testPkt.mutable_items()->CopyFrom(fromPkt.items());

    for (int i = 0; i < fromPkt.items_size(); i++) {
        // 아이템 데이터 조회
        UItemData* itemData = _dataManager->_itemTable[fromPkt.items(i).templateid()];
        if (IsValid(itemData) == false) {
            UE_LOG(LogTemp, Error, TEXT("HandleItemList Error"));
            return;
        }

        // 아이템 생성
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

        // 아이템 설정
        item->SetItemDB(fromPkt.items(i));
        item->Init(this, fromPkt.items(i).templateid());
        
        // c++ 인벤토리
        _inventory.Add(item);
        
        // UI
        //inventoryWidget->LoadInventory();
    }
}

void UMyGameInstance::HandleUseItem(PROTOCOL::S_UseItem fromPkt)
{
    // 인벤 뒤져서
    for (UItem* item : _inventory) {
        // 해당 아이템 찾아서
        if (item->itemDB.itemDbId == fromPkt.itemdbid()) {
            
            // c++
            // 소모품
            if (item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
                if (--item->itemDB.count <= 0) {
                    // 그리고 현재 문제있음 : UI가 제대로 안 비워짐
                    // UI 브러쉬 삭제되고 나서 우클릭하면 에러

                    // TODO : 해당 아이템 파기, 따로 묶어내는 게 좋아보임
                    _hudWidget->InventoryUI->GetSlotAt(item->itemDB.slot)->SetItem(nullptr);
                    _inventory.Remove(item);
                }
            }
            // 장비류
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
    // 아이템 데이터 조회
    // UItemData* itemData = nullptr;
    UItemData** ItemDataPtr = _dataManager->_itemTable.Find(itemInfo.templateid());
    if (ItemDataPtr == nullptr) {
        // if (IsValid((*ItemDataPtr)) == false) return;
        UE_LOG(LogTemp, Error, TEXT("Instance::AddItem() - nullptr"));
        return;
    }

    // 아이템
    UItem* item = nullptr;

    // 소모품
    if ((*ItemDataPtr)->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
        // 
        for (UItem* invenItem : _inventory) {
            // 동일 소모품 존재
            if (invenItem->itemDB.templateId == itemInfo.templateid()) {
                // 수량만  변경
                invenItem->itemDB.count = itemInfo.count();

                // UI 업데이트
                if (IsValid(_hudWidget) && IsValid(_hudWidget->InventoryUI))
                    _hudWidget->InventoryUI->SetInvenSlot(invenItem);

                // 
                return;
            }
        }

        // 소모품 새로 생성
        item = NewObject<UConsumable>();
    }

    // 무기
    else if ((*ItemDataPtr)->itemType == EItemType::ITEM_TYPE_WEAPON)
        item = NewObject<UWeapon>();
    // 방어구
    else if ((*ItemDataPtr)->itemType == EItemType::ITEM_TYPE_ARMOR)
        item = NewObject<UArmor>();
    else 
        return;

    // 아이템 설정
    item->SetItemDB(itemInfo);
    item->Init(this, itemInfo.templateid());

    // c++ 인벤에 추가
    _inventory.Add(item);
    
    // UI 업데이트
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
    
    // damage, defence - 기본 공,방
    _hudWidget->InventoryUI->UpdateDamageText();
    // _hudWidget->InventoryUI->UpdateArmorText();
}

void UMyGameInstance::HandleCreatePlayer(PROTOCOL::ObjectInfo info)
{
    // TODO : 상태가 로비인지 체크
    
    // 현재 위젯 = 로비위젯
    ULobbyWidget* lobby = Cast<ULobbyWidget>(_nowWidget);

    // 실패
    if (info.name().compare("ExistName") == 0) {
        lobby->PopUpWidget->FailReason->SetText(FText::FromString("Fail : ExistName"));
    }
    else if (info.name().compare("Fail") == 0) {
        lobby->PopUpWidget->FailReason->SetText(FText::FromString("Fail"));
    }
    // 성공
    else {
        // 캐릭터 리스트에 추가
        _myCharacterList.Add(info);

        // 팝업 닫고
        lobby->PopUpWidget->RemoveFromParent();
        lobby->PopUpWidget = nullptr;

        // Lobby 업데이트
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
