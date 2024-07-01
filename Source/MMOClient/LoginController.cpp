// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginController.h"

#include"MyGameInstance.h"
#include"ServerPacketHandler.h"

ALoginController::ALoginController()
{
}

void ALoginController::BeginPlay()
{
	Super::BeginPlay();

	_ownerInstance = Cast<UMyGameInstance>(GetGameInstance());
	_ownerInstance->_loginController = this;

	UE_LOG(LogTemp, Error, TEXT("LoginController::BeginPlay"));

	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

void ALoginController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (_ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_CONNECTED
		|| _ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_LOGIN) {

		if (!_ownerInstance->_packetHandler->_packetQueue.IsEmpty()) {
			TFunction<void()> func;
			_ownerInstance->_packetHandler->_packetQueue.Dequeue(func);
			
			func();
		}
	}
}

void ALoginController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogTemp, Error, TEXT("LoginController::EndPlay"));


}

