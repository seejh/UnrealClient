// Fill out your copyright notice in the Description page of Project Settings.


#include "WebManager.h"
#include"MyGameInstance.h"
#include<JsonObjectConverter.h>
#include"Data/GameStruct.h"
#include"FirebaseManager.h"

void FWebManager::Init(UMyGameInstance* instance, FString url)
{
	// 게임인스턴스 참조
	_ownerInstance = instance;

	// URL 설정
	_url = url;
}

FHttpRequestRef FWebManager::MakeRequest()
{
	auto request = FHttpModule::Get().CreateRequest();
	request->SetVerb("POST");
	request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	request->SetHeader("Content-Type", "application/json");

	return request;
}

/*-----------------------------------------------------------------------------
	요청
-----------------------------------------------------------------------------*/


void FWebManager::RequestPost(FLoginAccountReq reqModel, TFunction<void(FLoginAccountRes)> callback)
{
	// 리퀘스트 생성
	auto request = MakeRequest();
	request->SetURL(_url + FString("/api/account/login"));

	// 제이슨 오브젝트 직렬화, 리퀘스트에 등록
	TSharedRef<FJsonObject> jsonObject = FJsonObjectConverter::UStructToJsonObject(reqModel).ToSharedRef();
	FString jsonString = SerializeJsonObject(jsonObject);
	request->SetContentAsString(jsonString);

	// 콜백
	request->OnProcessRequestComplete().BindLambda([this, callback](FHttpRequestPtr req, FHttpResponsePtr res, bool success) {
		FLoginAccountRes response;
		
		if (success) {
			UE_LOG(LogTemp, Error, TEXT("Request Success = true"));

			TSharedPtr<FJsonObject> jsonObject = DeserializeJsonObject(res->GetContentAsString());
			if (jsonObject && FJsonObjectConverter::JsonObjectToUStruct(jsonObject.ToSharedRef(), &response)) {
				if (response.LoginOk) {
					callback(response);
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("JsonError"));
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Request Success = False. Not Callback"));
		}
		});

	// 요청
	request->ProcessRequest();
}

/*-----------------------------------------------------------------------------
	직렬화, 역직렬화 (InLine)
------------------------------------------------------------------------------*/
FString FWebManager::SerializeJsonObject(TSharedRef<FJsonObject>& jsonObject)
{
	FString jsonString;

	TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&jsonString);
	if (FJsonSerializer::Serialize(jsonObject, writer) == false)
		return FString("");

	return jsonString;
}

TSharedPtr<FJsonObject> FWebManager::DeserializeJsonObject(FString jsonString)
{
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(jsonString);
	if (FJsonSerializer::Deserialize(reader, jsonObject) == false) 
		return nullptr;

	return jsonObject;
}
