// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginController.h"

#include"MyGameInstance.h"
#include"ServerPacketHandler.h"
#include"UI/LoginWidget.h"
#include"UIManager.h"

ALoginController::ALoginController()
{
}

void ALoginController::BeginPlay()
{
	Super::BeginPlay();

	// GameInstance와 참조 연결
	_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
	_ownerInstance->_loginController = this;

	// 로그
	UE_LOG(LogTemp, Error, TEXT("LoginController::BeginPlay"));

	// 틱 시동 - 현재 비활성화
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

void ALoginController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 소켓 연결 상태, 로그인 상태여야 한다.
	if (_ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_CONNECTED
		|| _ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_LOGIN) {

		// 패킷 큐에서 패킷 확인하고 가져옴
		if (!_ownerInstance->_packetHandler->_packetQueue.IsEmpty()) {
			//
			TFunction<void()> func;
			_ownerInstance->_packetHandler->_packetQueue.Dequeue(func);
			
			// 패킷 실행
			func();
		}
	}
}

void ALoginController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogTemp, Error, TEXT("LoginController::EndPlay"));
}

