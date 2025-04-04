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
#include"UI/ServerListWidget.h"
#include"UI/ServerListSlotWidget.h"

#include"MyPlayerController.h"
#include"LoginController.h"
#include"NetSession.h"
#include"ServerPacketHandler.h"
#include"DataManager.h"
#include"Data/GameStruct.h"
#include"WebManager.h"
#include"UIManager.h"
#include"ObjectsManager.h"
#include"QuestManager.h"

#include<TimerManager.h>
#include "Blueprint/WidgetLayoutLibrary.h"

//#include<MyFirebasePlug.h>
#include"FirebaseManager.h"


UMyGameInstance::UMyGameInstance()
{
}

void UMyGameInstance::Init()
{
    Super::Init();

    // false = 개발중(웹서버X)
    _isRelease = true;

    // UI 매니저
    _uiManager = NewObject<UUIManager>();
    if (IsValid(_uiManager) == false) {
        UE_LOG(LogTemp, Error, TEXT("GameInstance::Init - Invalid UIManager"));
        return;
    }
    _uiManager->_ownerInstance = this;

    // 데이터매니저 생성
    _dataManager = NewObject<UDataManager>();
    if (IsValid(_dataManager) == false || _dataManager->Init() == false) {
        UE_LOG(LogTemp, Error, TEXT("GameInstance::Init - Invalid DataManager"));
        return;
    }

    // 오브젝트 매니저
    _objectsManager = NewObject<UObjectsManager>();
    if (IsValid(_objectsManager) == false) {
        UE_LOG(LogTemp, Error, TEXT("GameInstance::Init - Invalid ObjectsManager"));
        return;
    }
    _objectsManager->_ownerInstance = this;

    // 패킷 핸들러
    _packetHandler = MakeShareable(new FServerPacketHandler());
    _packetHandler->Init();

    // 퀘스트 매니저
    _questManager = NewObject<UQuestManager>();
    _questManager->_ownerInstance = this;

    // 웹매니저
    _webManager = new FWebManager();
    _webManager->Init(this, FString("https://localhost:7187"));

    // 네트워크 세션
    _netSession = MakeShareable(new FNetSession(this));

    // 파이어베이스
    /*_firebaseManager = NewObject<UFirebaseManager>();
    if (_firebaseManager->Init(this) == false)
        return;*/
}

void UMyGameInstance::BeginDestroy()
{
    Super::BeginDestroy();

    UE_LOG(LogTemp, Error, TEXT("GameInstance BeginDestory"));

    if (_netSession)
        _netSession->Stop();
}

void UMyGameInstance::AddItem(PROTOCOL::ItemInfo itemInfo)
{
    // 일단 소비 아이템을 획득할 때 기존 소비 아이템이 있어서 새로 생성이 아니라 
    // 업데이트만 하는 경우에도 S_AddItem 패킷으로 받는다.(Add로 처리한다는 소리)
    // 클라측 인벤에서 조회해서 해당 아이템이 없으면 생성, 해당 아이템이 있으면 업데이트라는 식으로 구분

    // 아이템 데이터 조회
    auto itemDataPtr = _dataManager->_itemTable.Find(itemInfo.templateid());
    if (itemDataPtr == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::AddItem() Error - Can't Find ItemData"));
        return;
    }

    //
    UItem* item;
    int32 differ;

    // 추가할 아이템이 소모품
    if ((*itemDataPtr)->itemType == EItemType::ITEM_TYPE_CONSUMABLE) {
        // 인벤토리에 해당 소모품이 이미 있는가
        auto itemPtr = inventory.Find(itemInfo.slot());
        if (itemPtr == nullptr) {

            // 없다 => 생성, 인벤 적재
            differ = itemInfo.count();
            item = NewObject<UConsumable>();
            item->Init(this, itemInfo.templateid());
            item->SetItemDB(itemInfo);
            inventory.Add(item->itemDB.slot, item);
        }
        else {

            // 있다 => 업데이트
            item = (*itemPtr);
            differ = itemInfo.count() - item->itemDB.count;
            item->SetItemDB(itemInfo);
        }
    }

    // 추가할 아이템이 장비(무기)
    else if ((*itemDataPtr)->itemType == EItemType::ITEM_TYPE_WEAPON) {
        // 생성, 인벤 적재
        item = NewObject<UWeapon>();
        item->Init(this, itemInfo.templateid());
        item->SetItemDB(itemInfo);
        inventory.Add(item->itemDB.slot, item);
    }

    // 추가할 아이템이 장비(방어구)
    else if ((*itemDataPtr)->itemType == EItemType::ITEM_TYPE_ARMOR) {
        // 생성, 인벤 적재
        item = NewObject<UArmor>();
        item->Init(this, itemInfo.templateid());
        item->SetItemDB(itemInfo);
        inventory.Add(item->itemDB.slot, item);
    }
    else
        return;

    // UI 업데이트
    // 처음 접속할 때 UI가 올라오기 전에 아이템리스트를 받는다. 그 때를 위한 구분이며, 그 때에는 UI처리 하지않음
    if (_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_GAME) {
        // 
        UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_uiManager->_mainUI);
        if (IsValid(hudUI))
            hudUI->InventoryUI->SetSlot(item->itemDB.slot);

        // 획득 로그
        FString str;
        str.Append(FString::Printf(TEXT("아이템 ")))
            .Append(item->itemData->name)
            .Append(FString::Printf(TEXT(" 획득")));

        if ((*itemDataPtr)->itemType == EItemType::ITEM_TYPE_CONSUMABLE)
            str.Append(FString::Printf(TEXT("(%d)"), differ));

        _playerController->SystemChat(str);
    }
    else if (_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_LOBBY) {
        UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_uiManager->_mainUI);
        if (IsValid(hudUI))
            hudUI->InventoryUI->SetSlot(item->itemDB.slot);
    }
}

void UMyGameInstance::UpdateItem(PROTOCOL::ItemInfo itemInfo)
{
    // C++ 인벤에서 해당 아이템 찾음
    auto itemPtr = inventory.Find(itemInfo.slot());
    if (itemPtr == nullptr)
        return;

    // 아이템 데이터 업데이트
    (*itemPtr)->SetItemDB(itemInfo);

    // UI 업데이트
    UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_uiManager->_mainUI);
    if (IsValid(hudUI)) {

        hudUI->InventoryUI->UpdateSlot(itemInfo.slot());

    }
}

void UMyGameInstance::Handle_Login(PROTOCOL::S_LOGIN fromPkt) {
    for (int i = 0; i < fromPkt.objectinfos_size(); i++) 
        _myCharacterList.Add(fromPkt.objectinfos(i));

    _uiManager->CreateMainUI(_uiManager->LobbyClass)->AddToViewport();
}

void UMyGameInstance::Handle_CreateAccount(FCreateAccountPacketRes pkt)
{
    //_loginController->CreateAccount(pkt);
}

void UMyGameInstance::Handle_EnterRoom(PROTOCOL::ObjectInfo info)
{
    // 현재 상태 체크 : 1.로비에서 첫 진입 / 2.월드에서 재 진입
    
    // 현재 로비 (처음스폰)
    if (_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_LOGIN) {
        
        // 현재 플레이어 정보 설정
        _myCharacterInfo = new PROTOCOL::ObjectInfo();
        _myCharacterInfo->CopyFrom(info);

        // 오픈되어 있는 기존 위젯들 제거 (여기서 많이 튕겼었다. 오픈 레벨을 할 때 기존 레벨의 UI들을 모두 닫아야 된다라고 들었으나 실제 효과를 못ㅂ)
        _loginController->SetActorTickEnabled(false);
        _loginController->Destroy();
        _loginController = nullptr;
        _uiManager->_mainUI->RemoveFromParent();

        // 오픈레벨
        UGameplayStatics::OpenLevel(GetWorld(), *FString(L"/Game/NewMap"), false, "");
    }

    // 현재 월드 (리스폰)
    else {
        // 현재 플레이어 정보 설정
        UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::Handle_EnterRoom() ReSpawn"));

        // 
        _myCharacterInfo = new PROTOCOL::ObjectInfo();
        _myCharacterInfo->CopyFrom(info);
        
        // 플레이어 컨트롤러 재기동
        _playerController->Init(false);
    }
}

void UMyGameInstance::Handle_LeaveRoom(PROTOCOL::S_LEAVE_ROOM fromPkt)
{
    // UI 및 데이터 정리 (아이템, 퀘스트, 스텟), 게임 오브젝트 정리

    UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_uiManager->_mainUI);
    if (IsValid(hudUI) == false) {
        UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::Handle_LeaveRoom() Error - Invalid HudUI"));
        return;
    }

    // 아이템 정리 (c++ 데이터)
    inventory.Empty();
    
    // 퀘스트 정리 (c++ 데이터)
    _questManager->Clear();

    // UI 초기화
    hudUI->Clear();
    
    // 게임 오브젝트 정리
    _objectsManager->Clear();
}

void UMyGameInstance::Handle_ItemList(PROTOCOL::S_ITEMLIST fromPkt)
{
    for (int i = 0; i < fromPkt.items_size(); i++) 
        AddItem(fromPkt.items(i));
}

void UMyGameInstance::Handle_UseItem(PROTOCOL::S_USE_ITEM fromPkt)
{
    // 소모품 아이템을 사용했다 -> 수량 감소만, 효과는 다른 패킷

    // 아이템 탐색
    UItem* item = inventory.FindRef(fromPkt.item().slot());
    if (IsValid(item) == false || item->itemData->itemType != EItemType::ITEM_TYPE_CONSUMABLE)
        return;

    // c++ 데이터 업데이트
    item->itemDB.count = fromPkt.item().count();
    
    // UI 데이터 업데이트
    UMyHUDWidget* hudWidget = Cast<UMyHUDWidget>(_uiManager->_mainUI);
    if (IsValid(hudWidget)) 
        hudWidget->InventoryUI->UpdateSlot(item->itemDB.slot);
}

void UMyGameInstance::Handle_EquipItem(PROTOCOL::S_EQUIP_ITEM fromPkt)
{
    // 아이템 탐색
    UItem* item = inventory.FindRef(fromPkt.slot());
    if (IsValid(item) == false || item->itemData->itemType == EItemType::ITEM_TYPE_CONSUMABLE)
        return;

    // c++ 장착/해제
    item->itemDB.equipped = fromPkt.equip();

    // UI 업데이트
    UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_uiManager->_mainUI);
    if (IsValid(hudUI)) {
        hudUI->InventoryUI->UpdateSlot(item->itemDB.slot);
    }
}

/*
    HandleRemoveItem
    제거 방식 : 언리얼의 TArray는 제거(Remove)를 하면 배열 내 순서가 유지가 안됨.
    ex1) [0] 철수 [1] 영희 [2] 민수 -> 인덱스1 제거 -> [0] 철수 [1] 민수 ->
    ex2) 인덱스 0 추가 -> [0] 영희 [1] 철수 [2] 민수
    나는 배열의 순서가 곧 슬롯이다. 고로 삭제하더라도 유지해야댐

    TArray에서 TMap으로 변경
    언리얼의 TArray는 c++의 배열과 비어있는 인덱스로 접근시 문제가 생김
*/
void UMyGameInstance::Handle_RemoveItem(PROTOCOL::S_REMOVE_ITEM fromPkt)
{
    // 제거할 아이템 리스트
    for (int32 removeSlot : fromPkt.slots()) {
        UE_LOG(LogTemp, Error, TEXT("RemoveItem Slot : %d"), removeSlot);

        // c++ 제거
        inventory.Remove(removeSlot);
        auto itemPtr = inventory.Find(removeSlot);
        if (itemPtr != nullptr) {
            UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::Handle_RemoveItem() Remove After Inventory C++ No Nullptr"));
        }

        // ui 제거
        UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_uiManager->_mainUI);
        if (IsValid(hudUI)) 
            hudUI->InventoryUI->SetSlot(removeSlot);
    }
}

void UMyGameInstance::Handle_AddItem(PROTOCOL::S_ADD_ITEM fromPkt)
{
    for (int i = 0; i < fromPkt.items_size(); i++) 
        AddItem(fromPkt.items(i));
}

void UMyGameInstance::Handle_UpdateItem(PROTOCOL::S_UPDATE_ITEM fromPkt)
{
    for (int i = 0; i < fromPkt.items_size(); i++) 
        UpdateItem(fromPkt.items(i));
}

void UMyGameInstance::Handle_ChangeStat(PROTOCOL::ObjectInfo info)
{
    // Data
    _myCharacterInfo->mutable_stat()->CopyFrom(info.stat());
    
    // UI
    UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_uiManager->_mainUI);
    if (IsValid(hudUI) == false)
        return;

    // Level
    hudUI->UpdateLevel();

    // Hp
    hudUI->UpdateHp();

    // Exp
    hudUI->UpdateExp();
    
    // damage, defence - 기본 공,방
    hudUI->InventoryUI->UpdateDamageText();
    // _hudWidget->InventoryUI->UpdateArmorText();
}

void UMyGameInstance::Handle_CreatePlayer(PROTOCOL::ObjectInfo info)
{
    // TODO : 상태가 로비인지 체크
    
    // 현재 위젯 = 로비위젯
    ULobbyWidget* lobby = Cast<ULobbyWidget>(_uiManager->_mainUI);

    // 실패
    if (info.name().compare("ExistName") == 0) {
        // lobby->PopUpWidget->FailReason->SetText(FText::FromString("Fail : ExistName"));
        UE_LOG(LogTemp, Error, TEXT("CreatePlayer Failed - ExistsName"));
    }
    else if (info.name().compare("Fail") == 0) {
        // lobby->PopUpWidget->FailReason->SetText(FText::FromString("Fail"));
        UE_LOG(LogTemp, Error, TEXT("CreatePlayer Failed - Fail"));
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

void UMyGameInstance::Handle_AddExp(int32 exp)
{
    int32 difference = exp - _myCharacterInfo->stat().exp();

    // Data
    _myCharacterInfo->mutable_stat()->set_exp(exp);

    // UI
    Cast<UMyHUDWidget>(_uiManager->_mainUI)->UpdateExp();

    FString str;
    str.Append(FString::Printf(TEXT("경험치 ")))
        .Append(FString::FromInt(difference))
        .Append(FString::Printf(TEXT(" 획득")));
    
    if(IsValid(_playerController))
        _playerController->SystemChat(str);
}

void UMyGameInstance::Handle_Spawn(PROTOCOL::S_SPAWN fromPkt)
{
    if (IsValid(_playerController))
        _playerController->SpawnObject(fromPkt);
    else
        UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::Handle_Spawn() Error - Invalid GameController"));
}

void UMyGameInstance::Handle_DeSpawn(PROTOCOL::S_DESPAWN fromPkt)
{
    if (IsValid(_playerController))
        _playerController->DeSpawnObject(fromPkt);
    else
        UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::Handle_DeSpawn() Error - Invalid GameController"));
}

void UMyGameInstance::Handle_Skill(PROTOCOL::S_SKILL fromPkt)
{
    if (IsValid(_playerController))
        _playerController->Skill(fromPkt);
}

void UMyGameInstance::Handle_Chat(PROTOCOL::S_CHAT fromPkt)
{
    if (IsValid(_playerController))
        _playerController->PlayerChat(fromPkt);
}

void UMyGameInstance::Handle_ChangeHp(PROTOCOL::S_CHANGE_HP fromPkt)
{
    if (IsValid(_playerController))
        _playerController->ChangeHP(fromPkt);
}

void UMyGameInstance::Handle_Die(PROTOCOL::S_DIE fromPkt)
{
    if (IsValid(_playerController))
        _playerController->Die(fromPkt);
}

void UMyGameInstance::Handle_AddQuest(PROTOCOL::QuestInfo questInfo)
{
    UE_LOG(LogTemp, Error, TEXT("Handle_AddQuest"));
    
    _questManager->AddQuest(questInfo);
}

void UMyGameInstance::Handle_RemoveQuest(PROTOCOL::S_REMOVE_QUEST fromPkt)
{
    if (fromPkt.result() == true)
        _questManager->RemoveQuest(fromPkt.questid());
}

void UMyGameInstance::Handle_CompleteQuest(PROTOCOL::S_COMPLETE_QUEST fromPkt)
{
    if (fromPkt.result() == true)
        _questManager->CompleteQuest(fromPkt.questid());
}

void UMyGameInstance::Handle_UpdateQuest(PROTOCOL::S_UPDATE_QUEST fromPkt)
{
    _questManager->UpdateQuest(fromPkt.questinfo());
}

/*
    
*/
PROTOCOL::PFVector UMyGameInstance::FVectorToPFV(FVector fv)
{
    PROTOCOL::PFVector pfv;
    pfv.set_x(fv.X);
    pfv.set_y(fv.Y);
    pfv.set_z(fv.Z);
    
    return pfv;
}

PROTOCOL::PFVector UMyGameInstance::FRotatorToPFV(FRotator fr)
{
    PROTOCOL::PFVector pfv;
    pfv.set_x(fr.Roll);
    pfv.set_y(fr.Pitch);
    pfv.set_z(fr.Yaw);

    return pfv;
}

FVector UMyGameInstance::PFVtoFVector(PROTOCOL::PFVector pfv)
{
    return FVector(pfv.x(), pfv.y(), pfv.z());
}

FRotator UMyGameInstance::PFVtoFRoator(PROTOCOL::PFVector pfv)
{
    // pitch, yaw, roll
    return FRotator(pfv.y(), pfv.z(), pfv.x());
}



