// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include"GameObject/MMOClientCharacter.h"
#include"MyGameInstance.h"
#include"ServerPacketHandler.h"
#include"NetSession.h"
#include"Buffer.h"
#include"ThreadHandler.h"
#include"GameObject/MyMonster.h"
#include"ObjectManager.h"
#include"GameObject/MyCharacterBase.h"
#include"GameObject/MyNpc.h"
#include"MyAIController.h"
#include"UI/InventoryWidget.h"
#include"UI/RespawnWidget.h"
#include"DataManager.h"
#include"UIManager.h"
#include"ObjectsManager.h"
#include"UI/QuestLogWidget.h"

#include<Kismet/GameplayStatics.h>
#include<Blueprint/WidgetBlueprintLibrary.h>
#include "GameFramework/Character.h"
/*---------------------------------------------------------------------------------------------
	MyPlayerController
---------------------------------------------------------------------------------------------*/
AMyPlayerController::AMyPlayerController()
{
}

void AMyPlayerController::Init(bool isFirst)
{
	UE_LOG(LogTemp, Error, TEXT("PC::Init"));

	AMMOClientCharacter* myPlayer = nullptr;

	if (isFirst) {
		// 게임인스턴스와 서로 참조 설정
		_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
		_ownerInstance->_playerController = this;

		// HUD UI 시작
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->CreateMainUI(_ownerInstance->_uiManager->InGameUIClass));
		if (IsValid(hudUI)) {
			hudUI->Init(true);
			hudUI->AddToViewport();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::BeginPlay() Error - Invalid InGameUI"));
			return;
		}

		// 스폰되는 기본 캐릭터 찾아서 제거
		myPlayer = Cast<AMMOClientCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (IsValid(myPlayer) == false) {
			UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::BeginPlay() Error - Invalid PlayerCharacter"));
			return;
		}
		myPlayer->Destroy();
	}
	else {
		// HUD UI 재기동
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI))
			hudUI->Reset();
	}
	
	// 오브젝트 매니저에서 내 캐릭터 생성
	myPlayer = Cast<AMMOClientCharacter>(_ownerInstance->_objectsManager->Add(*(_ownerInstance->_myCharacterInfo)));
	myPlayer->_objectId = _ownerInstance->_myCharacterInfo->objectid();

	// 게임인스턴스의 내 정보와 참조 연결
	_ownerInstance->_myCharacter = myPlayer;
	_ownerInstance->_myCharacterInfo = _ownerInstance->_objectsManager->GetObjectInfoById(myPlayer->_objectId);

	// 컨트롤러 빙의
	Possess(myPlayer);

	// 플레이어 게임 스테이트 변경 (퀘스트매니저문제로 여기서 처리)
	_ownerInstance->_playerState = PROTOCOL::PlayerServerState::SERVER_STATE_GAME;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("PC::BeginPlay"));

	Init(true);
	//// Owner 게임인스턴스 설정
	//_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
	//_ownerInstance->_playerController = this;

	//// 플레이어 게임 스테이트(UMyGameInstance::Handle_EnterRoom에서 이리로 이동, 퀘스트매니저 문제로)
	//_ownerInstance->_playerState = PROTOCOL::PlayerServerState::SERVER_STATE_GAME;

	//// HUD
	//UMyHUDWidget* ui = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->CreateMainUI(_ownerInstance->_uiManager->InGameUIClass));
	//if (IsValid(ui)) {
	//	ui->Init(true);
	//	ui->AddToViewport();
	//}
	//else {
	//	UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::BeginPlay() Error - Invalid InGameUI"));
	//	return;
	//}
	//
	//// 스폰되는 기본 캐릭터 찾아서 제거
	//AMMOClientCharacter* myPlayer = Cast<AMMOClientCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	//if (IsValid(myPlayer) == false) {
	//	UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::BeginPlay() Error - Invalid PlayerCharacter"));
	//	return;
	//}
	//myPlayer->Destroy();

	//// 오브젝트 매니저를 통해서 새로 생성
	//myPlayer = Cast<AMMOClientCharacter>(_ownerInstance->_objectsManager->Add(*(_ownerInstance->_myCharacterInfo)));
	//myPlayer->_objectId = _ownerInstance->_myCharacterInfo->objectid();

	//// 게임인스턴스에 내 정보에 포인터 연결
	//_ownerInstance->_myCharacter = myPlayer;
	//_ownerInstance->_myCharacterInfo = _ownerInstance->_objectsManager->GetObjectInfoById(myPlayer->_objectId);

	//// 컨트롤러 빙의
	//Possess(myPlayer);

	
	// 틱 가동
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	
	// 0.2f 주기로 서버로 내 위치 정보를 보내서 동기화
	GetWorldTimerManager().SetTimer(_timerHandle_myPosSend, this, &AMyPlayerController::SendMyPos, 0.2f, true, 2.0f);
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// 쌓인 패킷 처리
	if (!_ownerInstance->_packetHandler->_packetQueue.IsEmpty() /* &&
		_ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_GAME*/) {
		
		if (!_ownerInstance->_packetHandler->IsPacketQueueEmpty()) {
			TQueue<TFunction<void()>, EQueueMode::Spsc> q;
			_ownerInstance->_packetHandler->FlushPacket(q);

			while (!q.IsEmpty()) {
				TFunction<void()> f;
				q.Dequeue(f);
				f();
			}
		}
	}

	// 이동동기화 
	// 플레이어
	for (auto p : _ownerInstance->_objectsManager->_playerObjects) {
		// 내 캐릭 패스
		if (p.Key == _ownerInstance->_myCharacterInfo->objectid())
			continue;

		PROTOCOL::ObjectInfo* info = _ownerInstance->_objectsManager->GetObjectInfoById(p.Key);
		if (info == nullptr)
			continue;

		FVector vel(info->pos().velocityx(), info->pos().velocityy(), info->pos().velocityz());
		FRotator rot(info->pos().rotationpitch(), info->pos().rotationyaw(), info->pos().rotationroll());
		float speed = FVector::DotProduct(vel, rot.Vector());

		FVector loc(info->pos().locationx(), info->pos().locationy(), info->pos().locationz());
		FVector direction = (loc - p.Value->GetActorLocation()).GetSafeNormal();

		p.Value->AddMovementInput(direction * speed);
	}
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UE_LOG(LogTemp, Error, TEXT("PC::EndPlay"));
}

void AMyPlayerController::ShowMouseCursor(bool isShow)
{
	if (isShow == true) {
		SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_UIOnly(this);
		//UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this);
	}
	else {
		// SetShowMouseCursor(false);
		//UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this);
	}
}

void AMyPlayerController::SpawnObject(PROTOCOL::S_Spawn& spawnPkt)
{
	for (int i = 0; i < spawnPkt.object_size(); i++) {
		AMyCharacterBase* actor;

		// 본인 스폰
		if (spawnPkt.object(i).objectid() == _ownerInstance->_myCharacterInfo->objectid()) {
			UE_LOG(LogTemp, Error, TEXT("MyCharacter-%d Spawned"), spawnPkt.object(i).objectid());

			Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->UpdateHp();
			return;
		}

		// 타인 플레이어
		else if (spawnPkt.object(i).objecttype() == PROTOCOL::GameObjectType::PLAYER) {
			UE_LOG(LogTemp, Error, TEXT("Player-%d Spawned"), spawnPkt.object(i).objectid());
			
			actor = Cast<AMyCharacterBase>(_ownerInstance->_objectsManager->Add(spawnPkt.object(i)));
			actor->SpawnDefaultController(); // 일단 플레이어만 스폰 컨트롤러
		}
		// 몬스터
		else if (spawnPkt.object(i).objecttype() == PROTOCOL::GameObjectType::MONSTER) {
			UE_LOG(LogTemp, Error, TEXT("Monster-%d Spawned"), spawnPkt.object(i).objectid());

			actor = Cast<AMyCharacterBase>(_ownerInstance->_objectsManager->Add(spawnPkt.object(i)));
			//actor->SetActorActive(true);
			
			//UE_LOG(LogTemp, Error, TEXT("radius - %f"), monster->GetSimpleCollisionRadius());
			/*
			if (spawnPkt.object(i).name().compare("Crunch") == 0) {
				monster->GetMesh()->SetSkeletalMesh(_crunchMesh);
			}
			else {
				monster->GetMesh()->SetSkeletalMesh(_blackCrunchMesh);
			}*/
		}

		// NPC
		else if (spawnPkt.object(i).objecttype() == PROTOCOL::GameObjectType::NPC) {
			UE_LOG(LogTemp, Error, TEXT("Npc-%d Spawned"), spawnPkt.object(i).objectid());

			actor = Cast<AMyNpc>(_ownerInstance->_objectsManager->Add(spawnPkt.object(i)));
		}

		// 그 외
		else {
			return;
		}

		// 현재 이 부분이 문제라고해서 일단 주석처리
		// 문제가 있긴하네
		actor->UpdateHP();
	}
}

void AMyPlayerController::DeSpawnObject(PROTOCOL::S_DeSpawn& despawnPkt)
{
	for (int objectId : despawnPkt.objectids()) 
		_ownerInstance->_objectsManager->Remove(objectId);
}

void AMyPlayerController::MoveUpdate(PROTOCOL::ObjectInfo info)
{
	// 일단 내 캐릭터 이동이라면 패스
	if (info.objectid() == _ownerInstance->_myCharacterInfo->objectid())
		return;

	// 타입 확인
	PROTOCOL::GameObjectType type = _ownerInstance->_objectsManager->GetTypeById(info.objectid());
	// 플레이어
	if (type == PROTOCOL::GameObjectType::PLAYER) {

		auto objectInfoPtr = _ownerInstance->_objectsManager->_objectInfos.Find(info.objectid());
		if (objectInfoPtr == nullptr)
			return;

		// 오브젝트 인포 갱신
		objectInfoPtr->mutable_pos()->set_locationx(info.pos().locationx());
		objectInfoPtr->mutable_pos()->set_locationy(info.pos().locationy());
		objectInfoPtr->mutable_pos()->set_locationz(info.pos().locationz());

		objectInfoPtr->mutable_pos()->set_rotationpitch(info.pos().rotationpitch());
		objectInfoPtr->mutable_pos()->set_rotationyaw(info.pos().rotationyaw());
		objectInfoPtr->mutable_pos()->set_rotationroll(info.pos().rotationroll());

		objectInfoPtr->mutable_pos()->set_velocityx(info.pos().velocityx());
		objectInfoPtr->mutable_pos()->set_velocityy(info.pos().velocityy());
		objectInfoPtr->mutable_pos()->set_velocityz(info.pos().velocityz());
	}
	// 몬스터
	else if (type == PROTOCOL::GameObjectType::MONSTER) {
		auto actorPtr = _ownerInstance->_objectsManager->_objects.Find(info.objectid());
		auto objectInfoPtr = _ownerInstance->_objectsManager->_objectInfos.Find(info.objectid());
		if (objectInfoPtr == nullptr || actorPtr == nullptr)
			return;

		// 오브젝트 인포 갱신
		objectInfoPtr->mutable_pos()->set_locationx(info.pos().locationx());
		objectInfoPtr->mutable_pos()->set_locationy(info.pos().locationy());

		// 오브젝트 갱신
		AMyMonster* monster = Cast<AMyMonster>((*actorPtr));
		if (IsValid(monster))
			monster->MoveToLocation(FVector(info.pos().locationx(), info.pos().locationy(), 0));
		else
			UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::MoveUpdate() Error - Invalid Monster"));
	}

	/*FVector loc(info.pos().locationx(), info.pos().locationy(), info.pos().locationz());
	FRotator rot(info.pos().rotationpitch(), info.pos().rotationyaw(), info.pos().rotationroll());
	FVector vel(info.pos().velocityx(), info.pos().velocityy(), info.pos().velocityz());
	*/
	/*
	UCharacterMovementComponent* movementComponent = Cast<UCharacterMovementComponent>(characterBase->GetMovementComponent());

	float speed = FVector::DotProduct(vel, rot.Vector());
	FVector direction = (loc - character->GetActorLocation()).GetSafeNormal();

	character->SetActorRotation(rot);
	character->AddMovementInput(direction, speed);
	*/
}

void AMyPlayerController::ChangeHP(PROTOCOL::S_ChangeHp fromPkt)
{
	// 데이터 조회
	PROTOCOL::ObjectInfo* info = _ownerInstance->_objectsManager->GetObjectInfoById(fromPkt.object());
	if (info == nullptr)
		return;

	// C++
	info->mutable_stat()->set_hp(fromPkt.hp());

	// UI
	AMyCharacterBase* object = _ownerInstance->_objectsManager->GetObjectById(fromPkt.object());
	if (object == nullptr)
		return;

	if (object->_objectId == _ownerInstance->_myCharacterInfo->objectid()) {
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI))
			hudUI->UpdateHp();
	}
	else
		object->UpdateHP();
}

void AMyPlayerController::Skill(PROTOCOL::S_Skill& skillPkt)
{
	// 공격자
	if (_ownerInstance->_myCharacterInfo->objectid() != skillPkt.attacker()) {
		auto attackerObjectPtr = _ownerInstance->_objectsManager->GetObjectById(skillPkt.attacker());
		if (attackerObjectPtr)
			attackerObjectPtr->OnAttack();
	}

	// 피해자
	for (int i = 0; i < skillPkt.victims_size(); i++) {
		auto victimObjectPtr = _ownerInstance->_objectsManager->GetObjectById(skillPkt.victims(i));
		if (victimObjectPtr) 
			victimObjectPtr->OnAttacked();
	}
}

void AMyPlayerController::Die(PROTOCOL::S_Die fromPkt)
{
	// 해당 오브젝트 조회
	AMyCharacterBase* object = _ownerInstance->_objectsManager->GetObjectById(fromPkt.victim());
	if (object == nullptr)
		return;

	// 죽음 처리
	object->OnDead();

	// 그 오브젝트가 혹시 나라면
	if (object->_objectId == _ownerInstance->_myCharacterInfo->objectid()) {
		// 입력 해제
		_ownerInstance->_myCharacter->DisableInput(this);

		// 부활 위젯
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI)) {
			hudUI->RespawnUI->SetVisibility(ESlateVisibility::Visible);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, hudUI->RespawnUI);
		}

		// 본인 위치 동기화 정지
		// GetWorld()->GetTimerManager().ClearTimer(_timerHandle_myPosSend);
		_ownerInstance->_playerState = PROTOCOL::SERVER_STATE_LOBBY;
	}
}

void AMyPlayerController::PlayerChat(PROTOCOL::S_Chat fromPkt)
{
	// Chat Format
	FString chat("[");
	chat.Append(fromPkt.object().name().c_str());
	chat.Append("] : ");
	chat.Append(fromPkt.text().c_str());

	Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->AddChatMessage(chat);
}

void AMyPlayerController::SystemChat(FString chatMessage)
{
	
	UMyHUDWidget* ui = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
	if (IsValid(ui))
		ui->AddChatMessage(chatMessage);
}

void AMyPlayerController::SendMyPos()
{
	if (IsValid(_ownerInstance->_myCharacter) && _ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_GAME) {
		// 현재 렌더링 위치
		FVector loc = _ownerInstance->_myCharacter->GetActorLocation();
		FRotator rot = _ownerInstance->_myCharacter->GetActorRotation();
		FVector vel = _ownerInstance->_myCharacter->GetVelocity();

		// 렌더링 위치를 실제 위치로 지정
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_locationx(loc.X);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_locationy(loc.Y);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_locationz(loc.Z);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_rotationpitch(rot.Pitch);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_rotationyaw(rot.Yaw);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_rotationroll(rot.Roll);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_velocityx(vel.X);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_velocityy(vel.Y);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_velocityz(vel.Z);

		// 패킷 전송
		PROTOCOL::C_Move toPkt;
		toPkt.mutable_object()->CopyFrom(*(_ownerInstance->_myCharacterInfo));

		auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(toPkt);
		_ownerInstance->_netSession->Send(sendBuffer);
	}
}

void AMyPlayerController::MyPlayerAttack(TArray<int32>& playerArray)
{
	// 스킬패킷
	PROTOCOL::C_Skill toPkt;

	// 공격자, 사용 스킬, 피해자
	toPkt.set_attacker(_ownerInstance->_myCharacterInfo->objectid());
	toPkt.set_skillid(PROTOCOL::SkillType::SKILL_AUTO);
	for (int objectId : playerArray) 
		toPkt.add_victims(objectId);

	// 패킷 전송
	auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(toPkt);
	_ownerInstance->_netSession->Send(sendBuffer);
}


void AMyPlayerController::MyPlayerChat(FString& chatMessage)
{
	// 패킷 생성
	PROTOCOL::C_Chat pkt;
	pkt.set_text(TCHAR_TO_ANSI(*chatMessage));
	
	// 송신
	auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(pkt);
	_ownerInstance->_netSession->Send(sendBuffer);
}

/*---------------------------------------------------------------------------
		UpdateInGame(In Tick Methods)
---------------------------------------------------------------------------*/
//void AMyPlayerController::UpdateWorld(float DeltaSeconds)
//{
//	// 인게임 액터들
//	TArray<AActor*> spawnedPlayerArray;
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMMOClientCharacter::StaticClass(), spawnedPlayerArray);
//	
//	// 하나씩 가져와서
//	for (auto& spawnedPlayer : spawnedPlayerArray) {
//		AMMOClientCharacter* playerCharacter = Cast<AMMOClientCharacter>(spawnedPlayer);
//
//		// 해당 액터가 데이터 상으로도 로그인 중인지
//		PlayerInfo* playerInfo = _ownerInstance->FindPlayer(playerCharacter->_playerIndex);
//		if (playerInfo && playerInfo->IsLogin()) {
//			// 이동
//			// 내 캐릭이면 패스, 남 캐릭이라면 이동
//			if (playerCharacter->_playerIndex == _ownerInstance->_myPlayerIndex)
//				continue;
//			
//			float speed = FVector::DotProduct(playerInfo->GetVelocity(), playerInfo->GetRotation().Vector());
//			//worldChars->AddMovementInput(info->GetVelocity());
//			
//			UCharacterMovementComponent* movementComponent = Cast<UCharacterMovementComponent>(playerCharacter->GetMovementComponent());
//			FVector Direction = (playerInfo->GetLocation() - playerCharacter->GetActorLocation()).GetSafeNormal();
//
//			playerCharacter->AddMovementInput(Direction, speed);
//			//movementComponent->MoveSmooth(Direction * speed, DeltaSeconds);
//
//			playerCharacter->SetActorRotation(playerInfo->GetRotation());
//			//FRotator rotation = FMath::RInterpTo(worldChars->GetActorRotation(), info->GetRotation(), DeltaSeconds, SmoothTargetViewRotationSpeed);
//			//SetControlRotation(rotation);
//		}
//	/*}
//
//	TArray<AActor*> spawnedMonsterArray;
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyMonster::StaticClass(), spawnedMonsterArray);
//
//	for (auto& spawnedMonster : spawnedMonsterArray) {
//		AMyMonster* monsterCharacter = Cast<AMyMonster>(spawnedMonster);
//		
//		MonsterInfo* monsterInfo = _ownerInstance->FindMonster(monsterCharacter->_monsterIndex);
//		
//		if (monsterInfo && monsterInfo->IsActor()) {
//			monsterCharacter->MoveToLocation(monsterInfo->GetLocation());
//		}
//	}*/
//}



/*-----------------------------------------------------------------------------------
	액션 인풋, 마우스 커서 등 기타 등등
-------------------------------------------------------------------------------------*/

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OpenInventory", IE_Pressed, this, &AMyPlayerController::OpenInventory);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AMyPlayerController::Interact);
	InputComponent->BindAction("MouseCursor", IE_Pressed, this, &AMyPlayerController::MouseCursorOnOff);
	InputComponent->BindAction("OpenQuest", IE_Pressed, this, &AMyPlayerController::OpenQuest);
}

void AMyPlayerController::Interact()
{
	if (IsValid(_ownerInstance->_nowInteractable))
		_ownerInstance->_nowInteractable->Interact();
}

void AMyPlayerController::MouseCursorOnOff()
{
	// if (bShowMouseCursor == true) ShowMouseCursor(false);
	// else ShowMouseCursor(true);
}

void AMyPlayerController::OpenInventory()
{
	UMyHUDWidget* ui = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);

	if (IsValid(ui->InventoryUI) && ui->InventoryUI->GetVisibility() == ESlateVisibility::Hidden) {
		ui->InventoryUI->SetVisibility(ESlateVisibility::Visible);
		UWidgetBlueprintLibrary::SetInputMode_UIOnly(this, ui->InventoryUI);
	}
	else {
		ui->InventoryUI->SetVisibility(ESlateVisibility::Hidden);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this);
	}
}

void AMyPlayerController::OpenQuest()
{
	// UI를 열때 입력 모드를 UI와 게임 둘 다 가능하게 했다.
	// 그렇게 하니 UI를 열고 UI에서 입력이 게임에서도 입력공유가 되어버림(Ex.UI 버튼 클릭했는데 캐릭터 공격 입력이 들어감)
	// 해당 현상을 막기 위해 입력을 UI로만 고정을 했다.
	// 좋은데 문제가 인풋 액션으로 끄고 싶은데, 게임모드(플레이어컨트롤러)의 인풋액션이 막혀서 UI를 닫을 수가 없음.

	// UI를 열면 UI만 입력 받게하고, UI에서 눌리는 키를 조회해서 원하는 키가 눌릴 시 게임모드(플컨)에서 닫는 함수를 호출

	UMyHUDWidget* ui = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);

	if (IsValid(ui->QuestUI) && ui->QuestUI->GetVisibility() == ESlateVisibility::Hidden) {
		ui->QuestUI->SetVisibility(ESlateVisibility::Visible);
		UWidgetBlueprintLibrary::SetInputMode_UIOnly(this, ui->QuestUI);
	}
	else {
		ui->QuestUI->SetVisibility(ESlateVisibility::Hidden);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this);
	}
}

