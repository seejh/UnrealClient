// Fill out your copyright notice in the Description page of Project Settings.


#include "FirebaseManager.h"
#include"MyGameInstance.h"

using std::string;
using namespace firebase;
using namespace firebase::auth;


bool UFirebaseManager::Init(UMyGameInstance* instance)
{
	// 앱세팅 - 웹API키, 구글앱ID, 프로젝트ID
	firebase::AppOptions option = firebase::AppOptions();
	option.set_api_key("AIzaSyAKKEbl8OEg3yzWXWOMswMojkoQ1bWJSEE");
	option.set_app_id("1:640344942311:android:da61a3787dc35095aae442");
	option.set_project_id("ueloginsystem-3d4ad");
	//option.set_database_url();
	//option.set_messaging_sender_id();
	//option.set_storage_bucket();

	// 
	_app = firebase::App::Create(option);
	if (_app == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Firebase App Init Failed"));
		return false;
	}

	_ownerInstance = instance;

	return true;
}

void UFirebaseManager::Register(FString email, FString password)
{
	//Auth::GetAuth(_app)->SignInWithProvider();


	//firebase::auth::Credential credential = firebase::auth::GoogleAuthProvider::GetCredential(,);
	//Future<AuthResult> result = Auth::GetAuth(_app)->SignInAndRetrieveDataWithCredential();

	// 요청
	/*Auth::GetAuth(_app)->CreateUserWithEmailAndPassword(TCHAR_TO_ANSI(*email), TCHAR_TO_ANSI(*password)).OnCompletion([](const Future<AuthResult> result) {
		if (result.error() == kAuthErrorNone) {
			User user = result.result()->user;
			user.GetToken(true).OnCompletion([](const Future<string> result) {
				FString token(ANSI_TO_TCHAR(result.result()->c_str()));
				UE_LOG(LogTemp, Error, TEXT("%s"), *token);
				});
		}
		});*/

}

void UFirebaseManager::Login(FString email, FString password)
{
	// 요청
	Credential emailCredential = EmailAuthProvider::GetCredential(TCHAR_TO_ANSI(*email), TCHAR_TO_ANSI(*password));
	Future<User> future = Auth::GetAuth(_app)->SignInWithCredential(emailCredential);
	
	// 응답 비동기
	future.OnCompletion([](const firebase::Future<firebase::auth::User> future) {
		UE_LOG(LogTemp, Error, TEXT("Get Login Response"));
		if (future.error() == firebase::auth::kAuthErrorNone) {
			UE_LOG(LogTemp, Error, TEXT("Login OK"));
			
			firebase::auth::User user = *future.result();
			user.GetToken(true).OnCompletion([](const Future<string> future) {
				// UE_LOG(LogTemp, Error, TEXT("GetToken OK : %s"), *ANSI_TO_TCHAR(future.result()->c_str()));
				
				});

		}

		});
	
}

void UFirebaseManager::LoginToken()
{
	auto credential = firebase::auth::GoogleAuthProvider().GetCredential("", "");
	
}

//void UFirebaseManager::SendTokenToMyWeb(FGoogleLoginResponsePacket resPkt)
//{
//	Credential credential = GoogleAuthProvider::GetCredential(TCHAR_TO_ANSI(*resPkt.IdToken), nullptr);
//	Auth::GetAuth(_app)->SignInAndRetrieveDataWithCredential(credential).OnCompletion([](const Future<AuthResult> result) {
//		if (result.error() == firebase::auth::kAuthErrorNone) {
//			UE_LOG(LogTemp, Error, TEXT(""))
//		}
//		})
//}

