// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include"GameObject/MMOClientCharacter.h"
#include"MyGameInstance.h"
#include"ServerPacketHandler.h"
#include"NetSession.h"
#include"Buffer.h"
#include"ThreadHandler.h"
#include"GameObject/MyMonster.h"
#include"GameObject/MyCharacterBase.h"
#include"GameObject/MyNpc.h"
#include"MyAIController.h"
#include"UI/InventoryWidget.h"
#include"UI/RespawnWidget.h"
#include"DataManager.h"
#include"UIManager.h"
#include"ObjectsManager.h"
#include"UI/QuestLogWidget.h"
#include"GameObject/BotPlayer.h"

#include<Kismet/GameplayStatics.h>
#include<Blueprint/WidgetBlueprintLibrary.h>
#include<Blueprint/AIBlueprintHelperLibrary.h>
// #include "GameFramework/Character.h"
// #include<GameFramework/MovementComponent.h>
// #include<GameFramework/PawnMovementComponent.h>
// #include<GameFramework/CharacterMovementComponent.h>
/*---------------------------------------------------------------------------------------------
	MyPlayerController
---------------------------------------------------------------------------------------------*/
AMyPlayerController::AMyPlayerController()
{
}

void AMyPlayerController::Init(bool isFirst)
{
	AMMOClientCharacter* myPlayer = nullptr;
	UMyHUDWidget* hudUI = nullptr;

	// 접속
	if (isFirst == true) {
		// 게임 인스턴스, PC 서로 참조 추가
		_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
		_ownerInstance->_playerController = this;

		// 기본 스폰 캐릭터를 캐릭터로 설정
		myPlayer = Cast<AMMOClientCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (IsValid(myPlayer) == false) {
			UE_LOG(LogTemp, Error, TEXT("PC::Init(First) Error - Invalid PlayerCharacter"));
			return;
		}
		
		// 위치 설정
		myPlayer->SetActorLocation(FVector(_ownerInstance->_myCharacterInfo->pos().location().x(),
			_ownerInstance->_myCharacterInfo->pos().location().y(), _ownerInstance->_myCharacterInfo->pos().location().z()));

		// HudUI 시작
		hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->CreateMainUI(_ownerInstance->_uiManager->InGameUIClass));
		if (IsValid(hudUI)) {
			hudUI->Init(true);
			hudUI->AddToViewport();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("PC::Init() Error - Invalid HudUI"));
		}
	}
	// 리스폰
	else {
		// 기존 시체 제거
		_ownerInstance->_myCharacter->Destroy();

		// 캐릭터 스폰
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// 정보
		PROTOCOL::ObjectInfo* info = _ownerInstance->_myCharacterInfo;

		// 위치
		FVector loc(info->pos().location().x(), info->pos().location().y(), info->pos().location().z());
		FRotator rot(info->pos().rotation().y(), info->pos().rotation().z(), info->pos().rotation().x());
		
		// 스폰
		myPlayer = _ownerInstance->GetWorld()->SpawnActor<AMMOClientCharacter>(_ownerInstance->_objectsManager->_bpPlayer, loc, rot, param);
		if (IsValid(myPlayer) == false) {
			UE_LOG(LogTemp, Error, TEXT("PC::Init(ReSpawn) Error - Invalid PlayerCharacter"));
			return;
		}

		// 위치 설정
		myPlayer->SetActorLocation(FVector(_ownerInstance->_myCharacterInfo->pos().location().x(),
			_ownerInstance->_myCharacterInfo->pos().location().y(), _ownerInstance->_myCharacterInfo->pos().location().z()));

		// HudUI 재시작
		hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI)) {
			hudUI->Reset();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("PC::Init(ReSpawn) Error - Invalid HudUI"));
			return;
		}
	}

	bShowMouseCursor = true;

	// 내 캐릭터 오브젝트 정보 설정, 참조 설정
	myPlayer->_objectId = _ownerInstance->_myCharacterInfo->objectid();
	_ownerInstance->_myCharacter = myPlayer;

	// 내 캐릭터 PC 빙의
	Possess(myPlayer);

	// 게임스테이트 변경 ( -> 인게임)
	_ownerInstance->_playerState = PROTOCOL::PlayerServerState::SERVER_STATE_GAME;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("PC::BeginPlay"));

	// 
	Init(true);
	
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

	for (auto p : _ownerInstance->_objectsManager->_objects) {
		// 내 캐릭 패스
		
		PROTOCOL::ObjectInfo* info = _ownerInstance->_objectsManager->GetObjectInfoById(p.Key);
		if (info == nullptr)
			continue;

		FVector afterLoc(info->pos().location().x(), info->pos().location().y(), info->pos().location().z());

		// 방향
		FVector direction = (afterLoc - p.Value->GetActorLocation()).GetSafeNormal();
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(p.Value->GetController(), afterLoc);
	}
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UE_LOG(LogTemp, Error, TEXT("PC::EndPlay"));
}

void AMyPlayerController::SpawnObject(PROTOCOL::S_SPAWN& spawnPkt)
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
			//actor->SpawnDefaultController(); // 일단 플레이어만 스폰 컨트롤러
		}
		// 몬스터
		else if (spawnPkt.object(i).objecttype() == PROTOCOL::GameObjectType::MONSTER) {

			UE_LOG(LogTemp, Error, TEXT("Monster-%d Spawned"), spawnPkt.object(i).objectid());
			actor = Cast<AMyCharacterBase>(_ownerInstance->_objectsManager->Add(spawnPkt.object(i)));
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

		actor->UpdateHP();
	}
}

void AMyPlayerController::DeSpawnObject(PROTOCOL::S_DESPAWN& despawnPkt)
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

		// 오브젝트, 오브젝트인포 조회
		auto actorPtr = _ownerInstance->_objectsManager->_objects.Find(info.objectid());
		auto objectInfoPtr = _ownerInstance->_objectsManager->_objectInfos.Find(info.objectid());
		if (objectInfoPtr == nullptr)
			return;
		
		// 오브젝트 인포 갱신
		objectInfoPtr->mutable_pos()->mutable_location()->CopyFrom(info.pos().location());
	}
	// 몬스터
	else if (type == PROTOCOL::GameObjectType::MONSTER) {
		// 오브젝트, 오브젝트인포 조회
		auto actorPtr = _ownerInstance->_objectsManager->_objects.Find(info.objectid());
		auto objectInfoPtr = _ownerInstance->_objectsManager->_objectInfos.Find(info.objectid());
		if (actorPtr == nullptr || objectInfoPtr == nullptr)
			return;

		// 오브젝트 인포 갱신
		objectInfoPtr->mutable_pos()->mutable_location()->CopyFrom(info.pos().location());
		
		// 오브젝트 렌더 업데이트
		AMyMonster* monster = Cast<AMyMonster>((*actorPtr));
		if (IsValid(monster))
			monster->MoveToLocation(FVector(info.pos().location().x(), info.pos().location().y(), 0));
		else
			UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::MoveUpdate() Error - Invalid Monster"));
	}
}

void AMyPlayerController::ChangeHP(PROTOCOL::S_CHANGE_HP fromPkt)
{
	// 피해자가 내 캐릭
	if (fromPkt.object() == _ownerInstance->_myCharacterInfo->objectid()) {
		// C++ 데이터 업데이트
		_ownerInstance->_myCharacterInfo->mutable_stat()->set_hp(fromPkt.hp());

		// UI 업데이트
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI))
			hudUI->UpdateHp();
	}

	// 피해자가 내가 아님
	else {
		// 데이터 조회
		PROTOCOL::ObjectInfo* info = _ownerInstance->_objectsManager->GetObjectInfoById(fromPkt.object());
		if (info == nullptr)
			return;

		// C++ 데이터 업데이트
		info->mutable_stat()->set_hp(fromPkt.hp());

		// UI 업데이트
		AMyCharacterBase* object = _ownerInstance->_objectsManager->GetObjectById(fromPkt.object());
		if (object == nullptr)
			return;

		object->UpdateHP();
	}
}

void AMyPlayerController::Skill(PROTOCOL::S_SKILL& skillPkt)
{
	// 공격자
	if (_ownerInstance->_myCharacterInfo->objectid() != skillPkt.attacker()) {
		auto attackerObjectPtr = _ownerInstance->_objectsManager->GetObjectById(skillPkt.attacker());
		if (attackerObjectPtr) {
			attackerObjectPtr->OnAttack();
		}
	}

	// 피해자
	for (int i = 0; i < skillPkt.victims_size(); i++) {
		auto victimObjectPtr = _ownerInstance->_objectsManager->GetObjectById(skillPkt.victims(i));
		if (victimObjectPtr) 
			victimObjectPtr->OnAttacked();
	}
}

void AMyPlayerController::Die(PROTOCOL::S_DIE fromPkt)
{
	// 내 캐릭인가 (내 캐릭은 현재 오브젝트풀로 관리 안되기에 먼저 확인)
	if (fromPkt.victim() == _ownerInstance->_myCharacterInfo->objectid()) {
		// 입력 해제
		_ownerInstance->_myCharacter->DisableInput(this);

		// 죽음 애니메이션
		_ownerInstance->_myCharacter->OnDead();

		// 본인 위치 동기화 정지 - 이거는 체크
		_ownerInstance->_playerState = PROTOCOL::SERVER_STATE_LOBBY;

		// 리스폰 위젯
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI)) {
			hudUI->RespawnUI->SetVisibility(ESlateVisibility::Visible);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, hudUI->RespawnUI);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("PC::Die() Error - Invalid HudUI"));
		}
	}

	// 내 캐릭이 아닌 다른 캐릭
	else {
		// 오브젝트풀에서 해당 캐릭터 찾음
		AMyCharacterBase* object = _ownerInstance->_objectsManager->GetObjectById(fromPkt.victim());
		if (object == nullptr)
			return;

		// 죽음 애니메이션
		object->OnDead();
	}
}

void AMyPlayerController::PlayerChat(PROTOCOL::S_CHAT fromPkt)
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
		
		// 현재 렌더링 위치를 실제 위치로 지정
		_ownerInstance->_myCharacterInfo->mutable_pos()->mutable_location()->CopyFrom(_ownerInstance->FVectorToPFV(_ownerInstance->_myCharacter->GetActorLocation()));
		_ownerInstance->_myCharacterInfo->mutable_pos()->mutable_rotation()->CopyFrom(_ownerInstance->FRotatorToPFV(_ownerInstance->_myCharacter->GetActorRotation()));
		_ownerInstance->_myCharacterInfo->mutable_pos()->mutable_velocity()->CopyFrom(_ownerInstance->FVectorToPFV(_ownerInstance->_myCharacter->GetVelocity()));

		// 패킷 전송
		PROTOCOL::C_MOVE toPkt;
		toPkt.mutable_object()->CopyFrom(*(_ownerInstance->_myCharacterInfo));

		auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(toPkt);
		_ownerInstance->_netSession->Send(sendBuffer);
	}
}

void AMyPlayerController::MyPlayerAttack(TArray<int32>& playerArray)
{
	// 스킬패킷
	PROTOCOL::C_SKILL toPkt;

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
	PROTOCOL::C_CHAT pkt;
	pkt.set_text(TCHAR_TO_ANSI(*chatMessage));
	
	// 송신
	auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(pkt);
	_ownerInstance->_netSession->Send(sendBuffer);
}

/*-----------------------------------------------------------------------------------
	액션 인풋, 마우스 커서 등 기타 등등
-------------------------------------------------------------------------------------*/

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OpenInventory", IE_Pressed, this, &AMyPlayerController::OpenInventory);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AMyPlayerController::Interact);
	InputComponent->BindAction("MouseCursor", IE_Pressed, this, &AMyPlayerController::MouseCursor);
	InputComponent->BindAction("OpenQuest", IE_Pressed, this, &AMyPlayerController::OpenQuest);
}

void AMyPlayerController::Interact()
{
	if (IsValid(_ownerInstance->_nowInteractable))
		_ownerInstance->_nowInteractable->Interact();
}

void AMyPlayerController::MouseCursor()
{
	
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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


//void AMyPlayerController::Tick(float DeltaSeconds) {
	// 이동동기화 
	// 플레이어 - 플레이어를 봇으로 바꾸면서 이거 사용 정지
	//for (auto p : _ownerInstance->_objectsManager->_playerObjects) {
	//	// 내 캐릭 패스
	//	if (p.Key == _ownerInstance->_myCharacterInfo->objectid())
	//		continue;

	//	PROTOCOL::ObjectInfo* info = _ownerInstance->_objectsManager->GetObjectInfoById(p.Key);
	//	if (info == nullptr)
	//		continue;

	//	FVector vel(info->pos().velocityx(), info->pos().velocityy(), info->pos().velocityz());
	//	FRotator rot(info->pos().rotationpitch(), info->pos().rotationyaw(), info->pos().rotationroll());
	//	float speed = FVector::DotProduct(vel, rot.Vector());

	//	FVector loc(info->pos().locationx(), info->pos().locationy(), info->pos().locationz());
	//	FVector direction = (loc - p.Value->GetActorLocation()).GetSafeNormal();

	//	p.Value->AddMovementInput(direction * speed);
	//}

	// Cast<UCharacterMovementComponent>(p.Value->GetMovementComponent())->MoveSmooth(direction * 300, DeltaSeconds);

	// 속도
	//float speed = FVector::DotProduct(p.Value->GetVelocity(), p.Value->GetActorRotation().Vector());
	// p.Value->SetActorRotation(FRotator())


	// 이동
	// p.Value->AddMovementInput(direction, 1);
//}