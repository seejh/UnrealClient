// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include"GameObject/MMOClientCharacter.h"
#include"MyGameInstance.h"
#include"ServerPacketHandler.h"
#include"NetSession.h"
#include"Buffer.h"
#include"ThreadHandler.h"
#include"GameObject/MyMonster.h"
#include"GameObject/ObjectManager.h"
#include"GameObject/MyCharacterBase.h"
#include"MyAIController.h"
#include"UI/InventoryWidget.h"
#include"UI/RespawnWidget.h"
#include"Data/DataManager.h"

#include<Kismet/GameplayStatics.h>
#include<Blueprint/WidgetBlueprintLibrary.h>
#include "GameFramework/Character.h"
/*---------------------------------------------------------------------------------------------
	MyPlayerController
---------------------------------------------------------------------------------------------*/
AMyPlayerController::AMyPlayerController()
{

	// BP - Player
	UObject* cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Blueprint'/Game/BP_Player.BP_Player'"));
	if (!IsValid(cls)) UE_LOG(LogTemp, Error, TEXT("BP_Player Load Failed"));
	UBlueprint* bp = Cast<UBlueprint>(cls);
	_bpPlayer = (UClass*)bp->GeneratedClass;

	// BP - Monster
	UObject* cls2 = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Blueprint'/Game/BP_Monster.BP_Monster'"));
	if (!IsValid(cls2)) UE_LOG(LogTemp, Error, TEXT("BP_Monster Load Failed"));
	UBlueprint* bp2 = Cast<UBlueprint>(cls2);
	_bpMonster = (UClass*)bp2->GeneratedClass;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> BlackCrunchAsset(TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/Skins/Tier_3/BlackSite/Meshes/Crunch_Black_Site"));
	if (BlackCrunchAsset.Succeeded())
		_blackCrunchMesh = BlackCrunchAsset.Object;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> CrunchAsset(TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/Meshes/Crunch"));
	if (CrunchAsset.Succeeded())
		_crunchMesh = CrunchAsset.Object;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("PC::BeginPlay"));

	// Owner 게임인스턴스 설정
	_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
	_ownerInstance->_gameController = this;

	// HUD
	if (IsValid(_ownerInstance->MyHUDWidgetClass)) {
		_ownerInstance->_hudWidget = Cast<UMyHUDWidget>(CreateWidget(GetWorld(), _ownerInstance->MyHUDWidgetClass));
		if (IsValid(_ownerInstance->_hudWidget)) {
			_ownerInstance->_hudWidget->AddToViewport();
			_ownerInstance->_hudWidget->Init();
		}
	}

	// 오브젝트 매니저 시동
	_objectManager = new FObjectManager();
	if (!_objectManager->Init(this))
		UE_LOG(LogTemp, Error, TEXT("PC::BeginPlay - ObjectManager Init Failed"));

	// 기본 생성되는 캐릭 삭제
	AMMOClientCharacter* myPlayer = Cast<AMMOClientCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	GetWorld()->DestroyActor(myPlayer);

	// 오브젝트 매니저를 통해 생성      , 액터 활성화, 컨트롤러 빙의
	myPlayer = Cast<AMMOClientCharacter>(_objectManager->Add(*(_ownerInstance->_myCharacterInfo)));
	_ownerInstance->_myCharacterInfo = myPlayer->info;
	myPlayer->SetActorHiddenInGame(false);
	Possess(myPlayer);
	
	// 인스턴스에서 이 캐릭에 대한 참조 설정
	_ownerInstance->_myCharacter = myPlayer;
	
	// 틱 가동
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	
	// 0.2f 주기로 서버로 내 위치 정보를 보내서 동기화
	// AMyPlayerController::UpdateMyPos
	GetWorldTimerManager().SetTimer(_timerHandle, this, &AMyPlayerController::SendMyPos, 0.2f, true, 2.0f);
	
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// 쌓인 패킷 처리
	if (!_ownerInstance->_packetHandler->_packetQueue.IsEmpty() &&
		_ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_GAME) {
		
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

	// 이동동기화 : 몬스터도 ai가 아니라 일반 컨트롤러로 처리하는 것이 옳아보임
	// 플레이어
	for (auto p : _objectManager->_players) {
		//UCharacterMovementComponent* movementComponent = Cast<UCharacterMovementComponent>(characterBase->GetMovementComponent());
		
		PROTOCOL::ObjectInfo* info = _objectManager->_objectInfos[p.Key];
		if (info == nullptr)
			continue;

		// 내 캐릭 패스
		if (info->objectid() == _ownerInstance->_myCharacterInfo->objectid())
			continue;

		AMMOClientCharacter* player = p.Value;

		FVector vel(info->pos().velocityx(), info->pos().velocityy(), info->pos().velocityz());
		FRotator rot(info->pos().rotationpitch(), info->pos().rotationyaw(), info->pos().rotationroll());
		float speed = FVector::DotProduct(vel, rot.Vector());

		FVector loc(info->pos().locationx(), info->pos().locationy(), info->pos().locationz());
		FVector direction = (loc - player->GetActorLocation()).GetSafeNormal();

		player->AddMovementInput(direction * speed);
	}
}

void AMyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UE_LOG(LogTemp, Error, TEXT("PC::EndPlay"));
}

void AMyPlayerController::HandleSpawn(PROTOCOL::S_Spawn& spawnPkt)
{
	for (int i = 0; i < spawnPkt.object_size(); i++) {
		AMyCharacterBase* actor;

		// 본인 스폰
		if (spawnPkt.object(i).objectid() == _ownerInstance->_myCharacterInfo->objectid()) {
			_ownerInstance->_hudWidget->UpdateHp();
			return;
		}
		// 타인 플레이어
		else if (spawnPkt.object(i).objecttype() == PROTOCOL::GameObjectType::PLAYER) {
			
			// 오브젝트, 오브젝트 인포 생성
			actor = _objectManager->Add(spawnPkt.object(i));
			actor->SetActorActive(true);
		}
		// 몬스터
		else if (spawnPkt.object(i).objecttype() == PROTOCOL::GameObjectType::MONSTER) {
			actor = _objectManager->Add(spawnPkt.object(i));
			actor->SetActorActive(true);
			AMyMonster* monster = Cast<AMyMonster>(actor);
			//UE_LOG(LogTemp, Error, TEXT("radius - %f"), monster->GetSimpleCollisionRadius());
			
			if (spawnPkt.object(i).name().compare("Crunch") == 0) {
				monster->GetMesh()->SetSkeletalMesh(_crunchMesh);
			}
			else {
				monster->GetMesh()->SetSkeletalMesh(_blackCrunchMesh);
			}
		}
		// 그 외
		else {
			return;
		}

		actor->UpdateHP();
	}
}

void AMyPlayerController::HandleDespawn(PROTOCOL::S_DeSpawn& despawnPkt)
{
	for (int objectId : despawnPkt.objectids()) {
		// 비활성화
		AMyCharacterBase* actor = _objectManager->GetObject(objectId);
		if (IsValid(actor) == false)
			return;

		//
		actor->SetActorActive(false);
		
		// 디스폰
		_objectManager->Remove(objectId);
	}
}

void AMyPlayerController::MoveUpdate(PROTOCOL::ObjectInfo info)
{
	// 일단 내 캐릭터 이동이라면 패스
	if (info.objectid() == _ownerInstance->_myCharacterInfo->objectid())
		return;

	// 타입 확인
	PROTOCOL::GameObjectType type = _objectManager->GetObjectTypeById(info.objectid());

	// 오브젝트
	// 오브젝트 인포
	AActor* object = _objectManager->GetObject(info.objectid());
	PROTOCOL::ObjectInfo* objectInfo = _objectManager->GetObjectInfo(info.objectid());
	
	// 둘 중 하나라도 없으면 
	if (object == nullptr && objectInfo == nullptr) {

		return;
	}

	// 서버에서 확정 받은 위치
	FVector loc(info.pos().locationx(), info.pos().locationy(), info.pos().locationz());
	FRotator rot(info.pos().rotationpitch(), info.pos().rotationyaw(), info.pos().rotationroll());
	FVector vel(info.pos().velocityx(), info.pos().velocityy(), info.pos().velocityz());

	// 플레이어
	if (type == PROTOCOL::GameObjectType::PLAYER) {

		// 오브젝트 인포 갱신
		objectInfo->mutable_pos()->set_locationx(loc.X);
		objectInfo->mutable_pos()->set_locationy(loc.Y);
		objectInfo->mutable_pos()->set_locationz(loc.Z);

		objectInfo->mutable_pos()->set_rotationpitch(info.pos().rotationpitch());
		objectInfo->mutable_pos()->set_rotationyaw(info.pos().rotationyaw());
		objectInfo->mutable_pos()->set_rotationroll(info.pos().rotationroll());

		objectInfo->mutable_pos()->set_velocityx(info.pos().velocityx());
		objectInfo->mutable_pos()->set_velocityy(info.pos().velocityy());
		objectInfo->mutable_pos()->set_velocityz(info.pos().velocityz());

		// 오브젝트 갱신
		AMMOClientCharacter* player = Cast<AMMOClientCharacter>(object);
	}

	// 몬스터
	else if (type == PROTOCOL::GameObjectType::MONSTER) {

		// 오브젝트 인포 갱신
		objectInfo->mutable_pos()->set_locationx(loc.X);
		objectInfo->mutable_pos()->set_locationy(loc.Y);

		// 오브젝트 갱신
		AMyMonster* monster = Cast<AMyMonster>(object);
		monster->MoveToLocation(loc);
	}
	
	/*
	UCharacterMovementComponent* movementComponent = Cast<UCharacterMovementComponent>(characterBase->GetMovementComponent());

	float speed = FVector::DotProduct(vel, rot.Vector());
	FVector direction = (loc - character->GetActorLocation()).GetSafeNormal();

	character->SetActorRotation(rot);
	character->AddMovementInput(direction, speed);
	*/
}

void AMyPlayerController::HandleChangeHp(PROTOCOL::S_ChangeHp fromPkt)
{
	// 
	PROTOCOL::ObjectInfo* info = _objectManager->GetObjectInfo(fromPkt.object());
	if (info == nullptr)
		return;

	// Data
	info->mutable_stat()->set_hp(fromPkt.hp());

	// UI
	// 내 캐릭
	if (info->objectid() == _ownerInstance->_myCharacterInfo->objectid()) {
		_ownerInstance->_hudWidget->UpdateHp();
	}

	// 다른 액터
	else {
		AMyCharacterBase* actor = _objectManager->GetObject(fromPkt.object());
		if (IsValid(actor) == false)
			return;

		actor->UpdateHP();
	}
}

void AMyPlayerController::HandleSkill(PROTOCOL::S_Skill& skillPkt)
{
	// 공격자
	AMyCharacterBase* attackerObject = Cast<AMyCharacterBase>(_objectManager->GetObject(skillPkt.attacker()));
	if (IsValid(attackerObject)) {
		attackerObject->OnAttack();
	}

	// 피해자
	for (int i = 0; i < skillPkt.victims_size(); i++) {
		AMyCharacterBase* victimObject = Cast<AMyCharacterBase>(_objectManager->GetObject(skillPkt.victims(i)));
		if (IsValid(victimObject)) {
			victimObject->OnAttacked();
		}
	}
}

void AMyPlayerController::HandleDie(PROTOCOL::S_Die fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Actor-%d Die"), fromPkt.victim());

	// 사망 처리 (TODO : 시체 처리 및 기타 등등)
	AMyCharacterBase* object = Cast<AMyCharacterBase>(_objectManager->GetObject(fromPkt.victim()));
	if (IsValid(object))
		object->OnDead();

	// 죽은 게 내 캐릭
	if (fromPkt.victim() == _ownerInstance->_myCharacterInfo->objectid()) {
		// 입력 해제
		_ownerInstance->_myCharacter->DisableInput(this);
		
		// 위치 동기화 정지
		_ownerInstance->_isMeAlive = false;

		// 부활 위젯
		SetShowMouseCursor(true);
		_ownerInstance->_hudWidget->RespawnUI->SetVisibility(ESlateVisibility::Visible);

		// 본인 제외 액터 제거 - 디스폰 리스트
		PROTOCOL::S_DeSpawn removeList;
		for (auto p : _objectManager->_objectInfos) {
			if (p.Key == _ownerInstance->_myCharacterInfo->objectid())
				continue;

			removeList.add_objectids(p.Key);
		}

		// 디스폰
		HandleDespawn(removeList);
	}
}

void AMyPlayerController::HandleChat(PROTOCOL::S_Chat fromPkt)
{
	// Chat Format
	FString chat("[");
	chat.Append(fromPkt.object().name().c_str());
	chat.Append("] : ");
	chat.Append(fromPkt.text().c_str());

	_ownerInstance->_hudWidget->AddChatMessage(chat);
}

void AMyPlayerController::SendMyPos()
{
	if (IsValid(_ownerInstance->_myCharacter) && _ownerInstance->_isMeAlive == true) {
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
	for (int objectId : playerArray) {
		toPkt.add_victims(objectId);
	}

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


void AMyPlayerController::OpenInventory()
{
	if (IsValid(_ownerInstance->_hudWidget) && IsValid(_ownerInstance->_hudWidget->InventoryUI)) {
		// 인벤이 닫힌 상태
		if (_ownerInstance->_hudWidget->InventoryUI->GetVisibility() == ESlateVisibility::Hidden) {
			// 열고, 마우스 커서, UI 컨트롤
			_ownerInstance->_hudWidget->InventoryUI->SetVisibility(ESlateVisibility::Visible);
			SetShowMouseCursor(true);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, _ownerInstance->_hudWidget->InventoryUI);
		}

		// 인벤이 열린 상태
		else {
			// 닫고, 마우스 숨기고, 게임만
			_ownerInstance->_hudWidget->InventoryUI->SetVisibility(ESlateVisibility::Hidden);
			SetShowMouseCursor(false);
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
		}
	}
}

void AMyPlayerController::MouseCursorOnOff()
{
	// 보임
	if (bShowMouseCursor == true) {

		SetShowMouseCursor(false);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	}

	// 안 보임
	else {
		SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this);
	}
	
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OpenInventory", IE_Pressed, this, &AMyPlayerController::OpenInventory);
	InputComponent->BindAction("MouseCursor", IE_Pressed, this, &AMyPlayerController::MouseCursorOnOff);
}


