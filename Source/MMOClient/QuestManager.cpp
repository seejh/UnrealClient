// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManager.h"
#include"MyGameInstance.h"
#include"DataManager.h"
#include"UIManager.h"
#include"UI/MyHUDWidget.h"
#include"UI/QuestLogWidget.h"
#include"NetSession.h"
#include"ServerPacketHandler.h"
#include"ObjectsManager.h"
#include"GameObject/MyNpc.h"

// 퀘스트
void UQuest::Update(EQuestType& type, int32& objectiveId, int32& quantity, UMyGameInstance* instance)
{
	
}

/*--------------------------------------------------------------------------------
	퀘스트 매니저
---------------------------------------------------------------------------------*/
void UQuestManager::AddQuest(PROTOCOL::QuestInfo questInfo)
{
	// 맨 처음 퀘스트매니저에만 추가하는 것과 그 후 퀘스트UI에까지 추가하는 것 구분
	
	// 생성
	UQuest* quest = NewObject<UQuest>();
	quest->_questInfo.questDbId = questInfo.questdbid();
	quest->_questInfo.templateId = questInfo.templateid();
	quest->_questInfo.playerDbId = questInfo.playerdbid();
	quest->_questInfo.progress = questInfo.progress();
	quest->_questInfo.completed = questInfo.completed();

	// 퀘스트 상세 스펙
	auto questDataPtr = _ownerInstance->_dataManager->_questTable.Find(questInfo.templateid());
	if (questDataPtr == nullptr)
		return;
	quest->_questData = *(questDataPtr);

	// 추가
	_quests.Add(quest->_questInfo.templateId, quest);

	// UI 추가 (첫접속 빼고, 인게임중이나 다시 로그인할 때)
	if (_ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_GAME ||
		_ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_LOBBY)
		Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->QuestUI->AddQuestUI(quest);
}

void UQuestManager::RemoveQuest(int32 questTemplateId)
{
	// UI 먼저 지우고 퀘스트 매니저에서 제거 (삭제는 로그인 직후인지 구분 필요 X)
	// 퀘스트 매니저에서 해당 퀘스트 정보 조회 -> 퀘스트에서 UI 조회 후 삭제 -> 퀘스트매니저에서 퀘스트 정보 삭제

	auto QuestPtr = _quests.Find(questTemplateId);
	if (QuestPtr == nullptr)
		return;

	// UI 삭제
	Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->QuestUI->RemoveQuestUI(*QuestPtr);

	// 데이터 삭제
	_quests.Remove(questTemplateId);
}

void UQuestManager::InteractUpdateReq(int objectId, int objectiveTemplateId)
{
	for (auto p : _quests) {
		// 완료된 퀘스트 패스
		if (p.Value->_questInfo.completed)
			continue;

		// 상호작용퀘가 아니거나 오브젝티브가 다르면 패스
		if (p.Value->_questData->type != EQuestType::QUEST_TYPE_INTERACT || p.Value->_questData->objectiveId != objectiveTemplateId
			|| p.Value->_questInfo.progress == p.Value->_questData->quantity)
			continue;

		// 자체 업데이트
		p.Value->_questInfo.progress += 1;

		// 패킷
		PROTOCOL::C_UPDATE_QUEST toPkt;
		toPkt.mutable_questinfo()->set_questdbid(p.Value->_questInfo.questDbId);
		toPkt.mutable_questinfo()->set_templateid(p.Value->_questInfo.templateId);
		toPkt.mutable_questinfo()->set_playerdbid(p.Value->_questInfo.playerDbId);
		toPkt.mutable_questinfo()->set_progress(p.Value->_questInfo.progress);
		toPkt.mutable_questinfo()->set_completed(p.Value->_questInfo.completed);
		toPkt.set_objectid(objectId);
		auto sendBuffer = _ownerInstance->_packetHandler->MakeSendBuffer(toPkt);
		_ownerInstance->_netSession->Send(sendBuffer);
	}
}

void UQuestManager::CompleteQuest(int questId)
{
	// 데이터와 UI 상에서 퀘스트 진행중에서 완료로 이동

	// 데이터
	auto questPtr = _quests.Find(questId);
	if (questPtr == nullptr)
		return;

	(*questPtr)->_questInfo.completed = true;

	// UI
	Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->QuestUI->CompleteQuestUI((*questPtr));
}

UQuest* UQuestManager::CanCompleteByNpc(int npcId)
{
	// 이 npc가 퀘스트 엔더이며, 진행도를 다 채웠고, 퀘스트 완료가 되지 않은 퀘스트가 있으면
	for (auto p : _quests) {
		if (p.Value->_questData->QuestEnder == npcId && p.Value->_questInfo.progress >= p.Value->_questData->quantity
			&& p.Value->_questInfo.completed == false) {

			return p.Value;
		}
	}

	return nullptr;
}

void UQuestManager::UpdateQuest(PROTOCOL::QuestInfo questInfo)
{
	auto questPtr = _quests.Find(questInfo.templateid());
	if (questPtr == nullptr)
		return;

	// 데이터 업데이트
	(*questPtr)->_questInfo.questDbId = questInfo.questdbid();
	(*questPtr)->_questInfo.templateId = questInfo.templateid();
	(*questPtr)->_questInfo.playerDbId = questInfo.playerdbid();
	(*questPtr)->_questInfo.progress = questInfo.progress();
	(*questPtr)->_questInfo.completed = questInfo.completed();

	// UI 업데이트
	Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->QuestUI->UpdateUI((*questPtr));
}

void UQuestManager::Clear()
{
	// 퀘스트 데이터 제거
	_quests.Empty();
}

bool UQuestManager::Contains(int questId)
{
	return _quests.Contains(questId);
}

