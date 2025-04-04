// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseManager.generated.h"

namespace firebase { class App; }

class UMyGameInstance;
UCLASS()
class MMOCLIENT_API UFirebaseManager : public UObject
{
	GENERATED_BODY()
public:
	bool Init(UMyGameInstance* instance);

	// ฟไรป
	void Register(FString email, FString password);
	void Login(FString email, FString password);
	void LoginToken();
	//void SendTokenToMyWeb(FGoogleLoginResponsePacket resPkt);
	//void Delete();

	
public:
	firebase::App* _app;

	FTimerHandle _timerHandle;
	int32 _handleCount;
	UMyGameInstance* _ownerInstance;
};