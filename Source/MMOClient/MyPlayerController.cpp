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

	// ����
	if (isFirst == true) {
		// ���� �ν��Ͻ�, PC ���� ���� �߰�
		_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
		_ownerInstance->_playerController = this;

		// �⺻ ���� ĳ���͸� ĳ���ͷ� ����
		myPlayer = Cast<AMMOClientCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (IsValid(myPlayer) == false) {
			UE_LOG(LogTemp, Error, TEXT("PC::Init(First) Error - Invalid PlayerCharacter"));
			return;
		}
		
		// ��ġ ����
		myPlayer->SetActorLocation(FVector(_ownerInstance->_myCharacterInfo->pos().location().x(),
			_ownerInstance->_myCharacterInfo->pos().location().y(), _ownerInstance->_myCharacterInfo->pos().location().z()));

		// HudUI ����
		hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->CreateMainUI(_ownerInstance->_uiManager->InGameUIClass));
		if (IsValid(hudUI)) {
			hudUI->Init(true);
			hudUI->AddToViewport();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("PC::Init() Error - Invalid HudUI"));
		}
	}
	// ������
	else {
		// ���� ��ü ����
		_ownerInstance->_myCharacter->Destroy();

		// ĳ���� ����
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// ����
		PROTOCOL::ObjectInfo* info = _ownerInstance->_myCharacterInfo;

		// ��ġ
		FVector loc(info->pos().location().x(), info->pos().location().y(), info->pos().location().z());
		FRotator rot(info->pos().rotation().y(), info->pos().rotation().z(), info->pos().rotation().x());
		
		// ����
		myPlayer = _ownerInstance->GetWorld()->SpawnActor<AMMOClientCharacter>(_ownerInstance->_objectsManager->_bpPlayer, loc, rot, param);
		if (IsValid(myPlayer) == false) {
			UE_LOG(LogTemp, Error, TEXT("PC::Init(ReSpawn) Error - Invalid PlayerCharacter"));
			return;
		}

		// ��ġ ����
		myPlayer->SetActorLocation(FVector(_ownerInstance->_myCharacterInfo->pos().location().x(),
			_ownerInstance->_myCharacterInfo->pos().location().y(), _ownerInstance->_myCharacterInfo->pos().location().z()));

		// HudUI �����
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

	// �� ĳ���� ������Ʈ ���� ����, ���� ����
	myPlayer->_objectId = _ownerInstance->_myCharacterInfo->objectid();
	_ownerInstance->_myCharacter = myPlayer;

	// �� ĳ���� PC ����
	Possess(myPlayer);

	// ���ӽ�����Ʈ ���� ( -> �ΰ���)
	_ownerInstance->_playerState = PROTOCOL::PlayerServerState::SERVER_STATE_GAME;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("PC::BeginPlay"));

	// 
	Init(true);
	
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

	for (auto p : _ownerInstance->_objectsManager->_objects) {
		// �� ĳ�� �н�
		
		PROTOCOL::ObjectInfo* info = _ownerInstance->_objectsManager->GetObjectInfoById(p.Key);
		if (info == nullptr)
			continue;

		FVector afterLoc(info->pos().location().x(), info->pos().location().y(), info->pos().location().z());

		// ����
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
			//actor->SpawnDefaultController(); // �ϴ� �÷��̾ ���� ��Ʈ�ѷ�
		}
		// ����
		else if (spawnPkt.object(i).objecttype() == PROTOCOL::GameObjectType::MONSTER) {

			UE_LOG(LogTemp, Error, TEXT("Monster-%d Spawned"), spawnPkt.object(i).objectid());
			actor = Cast<AMyCharacterBase>(_ownerInstance->_objectsManager->Add(spawnPkt.object(i)));
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
	// �ϴ� �� ĳ���� �̵��̶�� �н�
	if (info.objectid() == _ownerInstance->_myCharacterInfo->objectid())
		return;

	// Ÿ�� Ȯ��
	PROTOCOL::GameObjectType type = _ownerInstance->_objectsManager->GetTypeById(info.objectid());
	// �÷��̾�
	if (type == PROTOCOL::GameObjectType::PLAYER) {

		// ������Ʈ, ������Ʈ���� ��ȸ
		auto actorPtr = _ownerInstance->_objectsManager->_objects.Find(info.objectid());
		auto objectInfoPtr = _ownerInstance->_objectsManager->_objectInfos.Find(info.objectid());
		if (objectInfoPtr == nullptr)
			return;
		
		// ������Ʈ ���� ����
		objectInfoPtr->mutable_pos()->mutable_location()->CopyFrom(info.pos().location());
	}
	// ����
	else if (type == PROTOCOL::GameObjectType::MONSTER) {
		// ������Ʈ, ������Ʈ���� ��ȸ
		auto actorPtr = _ownerInstance->_objectsManager->_objects.Find(info.objectid());
		auto objectInfoPtr = _ownerInstance->_objectsManager->_objectInfos.Find(info.objectid());
		if (actorPtr == nullptr || objectInfoPtr == nullptr)
			return;

		// ������Ʈ ���� ����
		objectInfoPtr->mutable_pos()->mutable_location()->CopyFrom(info.pos().location());
		
		// ������Ʈ ���� ������Ʈ
		AMyMonster* monster = Cast<AMyMonster>((*actorPtr));
		if (IsValid(monster))
			monster->MoveToLocation(FVector(info.pos().location().x(), info.pos().location().y(), 0));
		else
			UE_LOG(LogTemp, Error, TEXT("AMyPlayerController::MoveUpdate() Error - Invalid Monster"));
	}
}

void AMyPlayerController::ChangeHP(PROTOCOL::S_CHANGE_HP fromPkt)
{
	// �����ڰ� �� ĳ��
	if (fromPkt.object() == _ownerInstance->_myCharacterInfo->objectid()) {
		// C++ ������ ������Ʈ
		_ownerInstance->_myCharacterInfo->mutable_stat()->set_hp(fromPkt.hp());

		// UI ������Ʈ
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI))
			hudUI->UpdateHp();
	}

	// �����ڰ� ���� �ƴ�
	else {
		// ������ ��ȸ
		PROTOCOL::ObjectInfo* info = _ownerInstance->_objectsManager->GetObjectInfoById(fromPkt.object());
		if (info == nullptr)
			return;

		// C++ ������ ������Ʈ
		info->mutable_stat()->set_hp(fromPkt.hp());

		// UI ������Ʈ
		AMyCharacterBase* object = _ownerInstance->_objectsManager->GetObjectById(fromPkt.object());
		if (object == nullptr)
			return;

		object->UpdateHP();
	}
}

void AMyPlayerController::Skill(PROTOCOL::S_SKILL& skillPkt)
{
	// ������
	if (_ownerInstance->_myCharacterInfo->objectid() != skillPkt.attacker()) {
		auto attackerObjectPtr = _ownerInstance->_objectsManager->GetObjectById(skillPkt.attacker());
		if (attackerObjectPtr) {
			attackerObjectPtr->OnAttack();
		}
	}

	// ������
	for (int i = 0; i < skillPkt.victims_size(); i++) {
		auto victimObjectPtr = _ownerInstance->_objectsManager->GetObjectById(skillPkt.victims(i));
		if (victimObjectPtr) 
			victimObjectPtr->OnAttacked();
	}
}

void AMyPlayerController::Die(PROTOCOL::S_DIE fromPkt)
{
	// �� ĳ���ΰ� (�� ĳ���� ���� ������ƮǮ�� ���� �ȵǱ⿡ ���� Ȯ��)
	if (fromPkt.victim() == _ownerInstance->_myCharacterInfo->objectid()) {
		// �Է� ����
		_ownerInstance->_myCharacter->DisableInput(this);

		// ���� �ִϸ��̼�
		_ownerInstance->_myCharacter->OnDead();

		// ���� ��ġ ����ȭ ���� - �̰Ŵ� üũ
		_ownerInstance->_playerState = PROTOCOL::SERVER_STATE_LOBBY;

		// ������ ����
		UMyHUDWidget* hudUI = Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI);
		if (IsValid(hudUI)) {
			hudUI->RespawnUI->SetVisibility(ESlateVisibility::Visible);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, hudUI->RespawnUI);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("PC::Die() Error - Invalid HudUI"));
		}
	}

	// �� ĳ���� �ƴ� �ٸ� ĳ��
	else {
		// ������ƮǮ���� �ش� ĳ���� ã��
		AMyCharacterBase* object = _ownerInstance->_objectsManager->GetObjectById(fromPkt.victim());
		if (object == nullptr)
			return;

		// ���� �ִϸ��̼�
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
		
		// ���� ������ ��ġ�� ���� ��ġ�� ����
		_ownerInstance->_myCharacterInfo->mutable_pos()->mutable_location()->CopyFrom(_ownerInstance->FVectorToPFV(_ownerInstance->_myCharacter->GetActorLocation()));
		_ownerInstance->_myCharacterInfo->mutable_pos()->mutable_rotation()->CopyFrom(_ownerInstance->FRotatorToPFV(_ownerInstance->_myCharacter->GetActorRotation()));
		_ownerInstance->_myCharacterInfo->mutable_pos()->mutable_velocity()->CopyFrom(_ownerInstance->FVectorToPFV(_ownerInstance->_myCharacter->GetVelocity()));

		// ��Ŷ ����
		PROTOCOL::C_MOVE toPkt;
		toPkt.mutable_object()->CopyFrom(*(_ownerInstance->_myCharacterInfo));

		auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(toPkt);
		_ownerInstance->_netSession->Send(sendBuffer);
	}
}

void AMyPlayerController::MyPlayerAttack(TArray<int32>& playerArray)
{
	// ��ų��Ŷ
	PROTOCOL::C_SKILL toPkt;

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
	PROTOCOL::C_CHAT pkt;
	pkt.set_text(TCHAR_TO_ANSI(*chatMessage));
	
	// �۽�
	auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(pkt);
	_ownerInstance->_netSession->Send(sendBuffer);
}

/*-----------------------------------------------------------------------------------
	�׼� ��ǲ, ���콺 Ŀ�� �� ��Ÿ ���
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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


//void AMyPlayerController::Tick(float DeltaSeconds) {
	// �̵�����ȭ 
	// �÷��̾� - �÷��̾ ������ �ٲٸ鼭 �̰� ��� ����
	//for (auto p : _ownerInstance->_objectsManager->_playerObjects) {
	//	// �� ĳ�� �н�
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

	// �ӵ�
	//float speed = FVector::DotProduct(p.Value->GetVelocity(), p.Value->GetActorRotation().Vector());
	// p.Value->SetActorRotation(FRotator())


	// �̵�
	// p.Value->AddMovementInput(direction, 1);
//}