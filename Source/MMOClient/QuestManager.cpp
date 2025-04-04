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

// ����Ʈ
void UQuest::Update(EQuestType& type, int32& objectiveId, int32& quantity, UMyGameInstance* instance)
{
	
}

/*--------------------------------------------------------------------------------
	����Ʈ �Ŵ���
---------------------------------------------------------------------------------*/
void UQuestManager::AddQuest(PROTOCOL::QuestInfo questInfo)
{
	// �� ó�� ����Ʈ�Ŵ������� �߰��ϴ� �Ͱ� �� �� ����ƮUI������ �߰��ϴ� �� ����
	
	// ����
	UQuest* quest = NewObject<UQuest>();
	quest->_questInfo.questDbId = questInfo.questdbid();
	quest->_questInfo.templateId = questInfo.templateid();
	quest->_questInfo.playerDbId = questInfo.playerdbid();
	quest->_questInfo.progress = questInfo.progress();
	quest->_questInfo.completed = questInfo.completed();

	// ����Ʈ �� ����
	auto questDataPtr = _ownerInstance->_dataManager->_questTable.Find(questInfo.templateid());
	if (questDataPtr == nullptr)
		return;
	quest->_questData = *(questDataPtr);

	// �߰�
	_quests.Add(quest->_questInfo.templateId, quest);

	// UI �߰� (ù���� ����, �ΰ������̳� �ٽ� �α����� ��)
	if (_ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_GAME ||
		_ownerInstance->_playerState == PROTOCOL::PlayerServerState::SERVER_STATE_LOBBY)
		Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->QuestUI->AddQuestUI(quest);
}

void UQuestManager::RemoveQuest(int32 questTemplateId)
{
	// UI ���� ����� ����Ʈ �Ŵ������� ���� (������ �α��� �������� ���� �ʿ� X)
	// ����Ʈ �Ŵ������� �ش� ����Ʈ ���� ��ȸ -> ����Ʈ���� UI ��ȸ �� ���� -> ����Ʈ�Ŵ������� ����Ʈ ���� ����

	auto QuestPtr = _quests.Find(questTemplateId);
	if (QuestPtr == nullptr)
		return;

	// UI ����
	Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->QuestUI->RemoveQuestUI(*QuestPtr);

	// ������ ����
	_quests.Remove(questTemplateId);
}

void UQuestManager::InteractUpdateReq(int objectId, int objectiveTemplateId)
{
	for (auto p : _quests) {
		// �Ϸ�� ����Ʈ �н�
		if (p.Value->_questInfo.completed)
			continue;

		// ��ȣ�ۿ����� �ƴϰų� ������Ƽ�갡 �ٸ��� �н�
		if (p.Value->_questData->type != EQuestType::QUEST_TYPE_INTERACT || p.Value->_questData->objectiveId != objectiveTemplateId
			|| p.Value->_questInfo.progress == p.Value->_questData->quantity)
			continue;

		// ��ü ������Ʈ
		p.Value->_questInfo.progress += 1;

		// ��Ŷ
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
	// �����Ϳ� UI �󿡼� ����Ʈ �����߿��� �Ϸ�� �̵�

	// ������
	auto questPtr = _quests.Find(questId);
	if (questPtr == nullptr)
		return;

	(*questPtr)->_questInfo.completed = true;

	// UI
	Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->QuestUI->CompleteQuestUI((*questPtr));
}

UQuest* UQuestManager::CanCompleteByNpc(int npcId)
{
	// �� npc�� ����Ʈ �����̸�, ���൵�� �� ä����, ����Ʈ �Ϸᰡ ���� ���� ����Ʈ�� ������
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

	// ������ ������Ʈ
	(*questPtr)->_questInfo.questDbId = questInfo.questdbid();
	(*questPtr)->_questInfo.templateId = questInfo.templateid();
	(*questPtr)->_questInfo.playerDbId = questInfo.playerdbid();
	(*questPtr)->_questInfo.progress = questInfo.progress();
	(*questPtr)->_questInfo.completed = questInfo.completed();

	// UI ������Ʈ
	Cast<UMyHUDWidget>(_ownerInstance->_uiManager->_mainUI)->QuestUI->UpdateUI((*questPtr));
}

void UQuestManager::Clear()
{
	// ����Ʈ ������ ����
	_quests.Empty();
}

bool UQuestManager::Contains(int questId)
{
	return _quests.Contains(questId);
}

