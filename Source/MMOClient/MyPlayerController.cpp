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
		// �����ν��Ͻ��� ���� ���� ����
		_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
		_ownerInstance->_playerController = this;

		// HUD UI ����
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->CreateMainUI(_ownerInstance->_uiManager->InGameUIClass));
		if (IsValid(hudUI)) {
			hudUI->Init(true);
			hudUI->AddToViewport();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::BeginPlay() Error - Invalid InGameUI"));
			return;
		}

		// �����Ǵ� �⺻ ĳ���� ã�Ƽ� ����
		myPlayer = Cast<AMMOClientCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (IsValid(myPlayer) == false) {
			UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::BeginPlay() Error - Invalid PlayerCharacter"));
			return;
		}
		myPlayer->Destroy();
	}
	else {
		// HUD UI ��⵿
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI))
			hudUI->Reset();
	}
	
	// ������Ʈ �Ŵ������� �� ĳ���� ����
	myPlayer = Cast<AMMOClientCharacter>(_ownerInstance->_objectsManager->Add(*(_ownerInstance->_myCharacterInfo)));
	myPlayer->_objectId = _ownerInstance->_myCharacterInfo->objectid();

	// �����ν��Ͻ��� �� ������ ���� ����
	_ownerInstance->_myCharacter = myPlayer;
	_ownerInstance->_myCharacterInfo = _ownerInstance->_objectsManager->GetObjectInfoById(myPlayer->_objectId);

	// ��Ʈ�ѷ� ����
	Possess(myPlayer);

	// �÷��̾� ���� ������Ʈ ���� (����Ʈ�Ŵ��������� ���⼭ ó��)
	_ownerInstance->_playerState = PROTOCOL::PlayerServerState::SERVER_STATE_GAME;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("PC::BeginPlay"));

	Init(true);
	//// Owner �����ν��Ͻ� ����
	//_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
	//_ownerInstance->_playerController = this;

	//// �÷��̾� ���� ������Ʈ(UMyGameInstance::Handle_EnterRoom���� �̸��� �̵�, ����Ʈ�Ŵ��� ������)
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
	//// �����Ǵ� �⺻ ĳ���� ã�Ƽ� ����
	//AMMOClientCharacter* myPlayer = Cast<AMMOClientCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	//if (IsValid(myPlayer) == false) {
	//	UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::BeginPlay() Error - Invalid PlayerCharacter"));
	//	return;
	//}
	//myPlayer->Destroy();

	//// ������Ʈ �Ŵ����� ���ؼ� ���� ����
	//myPlayer = Cast<AMMOClientCharacter>(_ownerInstance->_objectsManager->Add(*(_ownerInstance->_myCharacterInfo)));
	//myPlayer->_objectId = _ownerInstance->_myCharacterInfo->objectid();

	//// �����ν��Ͻ��� �� ������ ������ ����
	//_ownerInstance->_myCharacter = myPlayer;
	//_ownerInstance->_myCharacterInfo = _ownerInstance->_objectsManager->GetObjectInfoById(myPlayer->_objectId);

	//// ��Ʈ�ѷ� ����
	//Possess(myPlayer);

	
	// ƽ ����
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	
	// 0.2f �ֱ�� ������ �� ��ġ ������ ������ ����ȭ
	GetWorldTimerManager().SetTimer(_timerHandle_myPosSend, this, &AMyPlayerController::SendMyPos, 0.2f, true, 2.0f);
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// ���� ��Ŷ ó��
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

	// �̵�����ȭ 
	// �÷��̾�
	for (auto p : _ownerInstance->_objectsManager->_playerObjects) {
		// �� ĳ�� �н�
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

		// ���� ����
		if (spawnPkt.object(i).objectid() == _ownerInstance->_myCharacterInfo->objectid()) {
			UE_LOG(LogTemp, Error, TEXT("MyCharacter-%d Spawned"), spawnPkt.object(i).objectid());

			Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->UpdateHp();
			return;
		}

		// Ÿ�� �÷��̾�
		else if (spawnPkt.object(i).objecttype() == PROTOCOL::GameObjectType::PLAYER) {
			UE_LOG(LogTemp, Error, TEXT("Player-%d Spawned"), spawnPkt.object(i).objectid());
			
			actor = Cast<AMyCharacterBase>(_ownerInstance->_objectsManager->Add(spawnPkt.object(i)));
			actor->SpawnDefaultController(); // �ϴ� �÷��̾ ���� ��Ʈ�ѷ�
		}
		// ����
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

		// �� ��
		else {
			return;
		}

		// ���� �� �κ��� ��������ؼ� �ϴ� �ּ�ó��
		// ������ �ֱ��ϳ�
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
	// �ϴ� �� ĳ���� �̵��̶�� �н�
	if (info.objectid() == _ownerInstance->_myCharacterInfo->objectid())
		return;

	// Ÿ�� Ȯ��
	PROTOCOL::GameObjectType type = _ownerInstance->_objectsManager->GetTypeById(info.objectid());
	// �÷��̾�
	if (type == PROTOCOL::GameObjectType::PLAYER) {

		auto objectInfoPtr = _ownerInstance->_objectsManager->_objectInfos.Find(info.objectid());
		if (objectInfoPtr == nullptr)
			return;

		// ������Ʈ ���� ����
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
	// ����
	else if (type == PROTOCOL::GameObjectType::MONSTER) {
		auto actorPtr = _ownerInstance->_objectsManager->_objects.Find(info.objectid());
		auto objectInfoPtr = _ownerInstance->_objectsManager->_objectInfos.Find(info.objectid());
		if (objectInfoPtr == nullptr || actorPtr == nullptr)
			return;

		// ������Ʈ ���� ����
		objectInfoPtr->mutable_pos()->set_locationx(info.pos().locationx());
		objectInfoPtr->mutable_pos()->set_locationy(info.pos().locationy());

		// ������Ʈ ����
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
	// ������ ��ȸ
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
	// ������
	if (_ownerInstance->_myCharacterInfo->objectid() != skillPkt.attacker()) {
		auto attackerObjectPtr = _ownerInstance->_objectsManager->GetObjectById(skillPkt.attacker());
		if (attackerObjectPtr)
			attackerObjectPtr->OnAttack();
	}

	// ������
	for (int i = 0; i < skillPkt.victims_size(); i++) {
		auto victimObjectPtr = _ownerInstance->_objectsManager->GetObjectById(skillPkt.victims(i));
		if (victimObjectPtr) 
			victimObjectPtr->OnAttacked();
	}
}

void AMyPlayerController::Die(PROTOCOL::S_Die fromPkt)
{
	// �ش� ������Ʈ ��ȸ
	AMyCharacterBase* object = _ownerInstance->_objectsManager->GetObjectById(fromPkt.victim());
	if (object == nullptr)
		return;

	// ���� ó��
	object->OnDead();

	// �� ������Ʈ�� Ȥ�� �����
	if (object->_objectId == _ownerInstance->_myCharacterInfo->objectid()) {
		// �Է� ����
		_ownerInstance->_myCharacter->DisableInput(this);

		// ��Ȱ ����
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI)) {
			hudUI->RespawnUI->SetVisibility(ESlateVisibility::Visible);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, hudUI->RespawnUI);
		}

		// ���� ��ġ ����ȭ ����
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
		// ���� ������ ��ġ
		FVector loc = _ownerInstance->_myCharacter->GetActorLocation();
		FRotator rot = _ownerInstance->_myCharacter->GetActorRotation();
		FVector vel = _ownerInstance->_myCharacter->GetVelocity();

		// ������ ��ġ�� ���� ��ġ�� ����
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_locationx(loc.X);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_locationy(loc.Y);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_locationz(loc.Z);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_rotationpitch(rot.Pitch);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_rotationyaw(rot.Yaw);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_rotationroll(rot.Roll);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_velocityx(vel.X);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_velocityy(vel.Y);
		_ownerInstance->_myCharacterInfo->mutable_pos()->set_velocityz(vel.Z);

		// ��Ŷ ����
		PROTOCOL::C_Move toPkt;
		toPkt.mutable_object()->CopyFrom(*(_ownerInstance->_myCharacterInfo));

		auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(toPkt);
		_ownerInstance->_netSession->Send(sendBuffer);
	}
}

void AMyPlayerController::MyPlayerAttack(TArray<int32>& playerArray)
{
	// ��ų��Ŷ
	PROTOCOL::C_Skill toPkt;

	// ������, ��� ��ų, ������
	toPkt.set_attacker(_ownerInstance->_myCharacterInfo->objectid());
	toPkt.set_skillid(PROTOCOL::SkillType::SKILL_AUTO);
	for (int objectId : playerArray) 
		toPkt.add_victims(objectId);

	// ��Ŷ ����
	auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(toPkt);
	_ownerInstance->_netSession->Send(sendBuffer);
}


void AMyPlayerController::MyPlayerChat(FString& chatMessage)
{
	// ��Ŷ ����
	PROTOCOL::C_Chat pkt;
	pkt.set_text(TCHAR_TO_ANSI(*chatMessage));
	
	// �۽�
	auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(pkt);
	_ownerInstance->_netSession->Send(sendBuffer);
}

/*---------------------------------------------------------------------------
		UpdateInGame(In Tick Methods)
---------------------------------------------------------------------------*/
//void AMyPlayerController::UpdateWorld(float DeltaSeconds)
//{
//	// �ΰ��� ���͵�
//	TArray<AActor*> spawnedPlayerArray;
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMMOClientCharacter::StaticClass(), spawnedPlayerArray);
//	
//	// �ϳ��� �����ͼ�
//	for (auto& spawnedPlayer : spawnedPlayerArray) {
//		AMMOClientCharacter* playerCharacter = Cast<AMMOClientCharacter>(spawnedPlayer);
//
//		// �ش� ���Ͱ� ������ �����ε� �α��� ������
//		PlayerInfo* playerInfo = _ownerInstance->FindPlayer(playerCharacter->_playerIndex);
//		if (playerInfo && playerInfo->IsLogin()) {
//			// �̵�
//			// �� ĳ���̸� �н�, �� ĳ���̶�� �̵�
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
	�׼� ��ǲ, ���콺 Ŀ�� �� ��Ÿ ���
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
	// UI�� ���� �Է� ��带 UI�� ���� �� �� �����ϰ� �ߴ�.
	// �׷��� �ϴ� UI�� ���� UI���� �Է��� ���ӿ����� �Է°����� �Ǿ����(Ex.UI ��ư Ŭ���ߴµ� ĳ���� ���� �Է��� ��)
	// �ش� ������ ���� ���� �Է��� UI�θ� ������ �ߴ�.
	// ������ ������ ��ǲ �׼����� ���� ������, ���Ӹ��(�÷��̾���Ʈ�ѷ�)�� ��ǲ�׼��� ������ UI�� ���� ���� ����.

	// UI�� ���� UI�� �Է� �ް��ϰ�, UI���� ������ Ű�� ��ȸ�ؼ� ���ϴ� Ű�� ���� �� ���Ӹ��(����)���� �ݴ� �Լ��� ȣ��

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

