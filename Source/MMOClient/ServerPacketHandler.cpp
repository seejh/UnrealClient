// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerPacketHandler.h"

#include"MyPlayerController.h"
#include"MyGameInstance.h"
#include"UIManager.h"

/*---------------------------------------------------------------------------------------------
	실제로 패킷을 처리하는 부분 (전역함수)
---------------------------------------------------------------------------------------------*/
bool Handle_INVALID(UMyGameInstance* instance, uint8* buffer, int len)
{
	UE_LOG(LogTemp, Error, TEXT("Packet Invalid"));
	return true;
}

bool Handle_S_LOGIN(UMyGameInstance* instance, PROTOCOL::S_Login fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_Login"));

	instance->Handle_Login(fromPkt);
	
	return true;
}

// 본인(로그인, 종료)
bool Handle_S_ENTER_ROOM(UMyGameInstance* instance, PROTOCOL::S_Enter_Room fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_EnterRoom"));

	if (fromPkt.success()) 
		instance->Handle_EnterRoom(fromPkt.object());

	return true;
}

bool Handle_S_LEAVE_ROOM(UMyGameInstance* instance, PROTOCOL::S_Leave_Room fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_LeaveRoom"));

	instance->Handle_LeaveRoom(fromPkt);
	
	return true;
}

bool Handle_S_SPAWN(UMyGameInstance* instance, PROTOCOL::S_Spawn fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_Spawn"));

	instance->Handle_Spawn(fromPkt);

	return true;
}

bool Handle_S_DESPAWN(UMyGameInstance* instance, PROTOCOL::S_DeSpawn fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_DeSpawn"));
	
	instance->Handle_DeSpawn(fromPkt);
	
	return true;
}

bool Handle_S_ITEMLIST(UMyGameInstance* instance, PROTOCOL::S_ItemList fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_ItemList - Count:%d"), fromPkt.items_size());

	instance->Handle_ItemList(fromPkt);

	return true;
}

bool Handle_S_MOVE(UMyGameInstance* instance, PROTOCOL::S_Move fromPkt)
{
	instance->_playerController->MoveUpdate(fromPkt.object());
	
	return true;
}

bool Handle_S_SKILL(UMyGameInstance* instance, PROTOCOL::S_Skill fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_Skill"));

	instance->Handle_Skill(fromPkt);

	return true;
}

bool Handle_S_CHAT(UMyGameInstance* instance, PROTOCOL::S_Chat fromPkt)
{
	instance->Handle_Chat(fromPkt);

	return true;
}

bool Handle_S_CHANGEHP(UMyGameInstance* instance, PROTOCOL::S_ChangeHp fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_ChangeHP"));

	instance->Handle_ChangeHp(fromPkt);

	return true;
}

bool Handle_S_DIE(UMyGameInstance* instance, PROTOCOL::S_Die fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_Die"));

	instance->Handle_Die(fromPkt);
	
	return true;
}

bool Handle_S_EQUIPITEM(UMyGameInstance* instance, PROTOCOL::S_EquipItem fromPkt)
{
	instance->Handle_EquipItem(fromPkt);

	return true;
}

bool Handle_S_USEITEM(UMyGameInstance* instance, PROTOCOL::S_UseItem fromPkt)
{
	instance->Handle_UseItem(fromPkt);

	return true;
}

bool Handle_S_ADDITEM(UMyGameInstance* instance, PROTOCOL::S_AddItem fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_AddItem"));

	instance->Handle_AddItem(fromPkt);

	return true;
}

bool Handle_S_UPDATEITEM(UMyGameInstance* instance, PROTOCOL::S_UpdateItem fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_UpdateItem"));

	instance->Handle_UpdateItem(fromPkt);

	return true;
}

bool Handle_S_REMOVEITEM(UMyGameInstance* instance, PROTOCOL::S_RemoveItem fromPkt)
{
	instance->Handle_RemoveItem(fromPkt);

	return true;
}

// 장비 변경, 레벨업, 
bool Handle_S_CHANGE_STAT(UMyGameInstance* instance, PROTOCOL::S_ChangeStat fromPkt)
{
	instance->Handle_ChangeStat(fromPkt.object());

	return true;
}

bool Handle_S_CREATE_PLAYER(UMyGameInstance* instance, PROTOCOL::S_CreatePlayer fromPkt)
{
	instance->Handle_CreatePlayer(fromPkt.object());
	
	return false;
}

bool Handle_S_ADD_EXP(UMyGameInstance* instance, PROTOCOL::S_AddExp fromPkt)
{
	instance->Handle_AddExp(fromPkt.exp());

	return true;
}

bool Handle_S_LEVEL_UP(UMyGameInstance* instance, PROTOCOL::S_LevelUp fromPkt)
{
	instance->Handle_ChangeStat(fromPkt.info());
	
	// TODO : 레벨업 이펙트
	Cast<UMyHUDWidget>(instance->_uiManager->_mainUI)->PlayLevelUpWidgetAnim();

	return true;
}

bool Handle_S_ADD_QUEST(UMyGameInstance* instance, PROTOCOL::S_AddQuest fromPkt)
{
	instance->Handle_AddQuest(fromPkt.quest());

	return true;
}

bool Handle_S_QUESTLIST(UMyGameInstance* instance, PROTOCOL::S_QuestList fromPkt)
{
	for (int i = 0; i < fromPkt.quests_size(); i++) 
		instance->Handle_AddQuest(fromPkt.quests(i));

	return true;
}

bool Handle_S_REMOVE_QUEST(UMyGameInstance* instance, PROTOCOL::S_RemoveQuest fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_RemoveQuest"));

	instance->Handle_RemoveQuest(fromPkt);

	return true;
}

bool Handle_S_COMPLETE_QUEST(UMyGameInstance* instance, PROTOCOL::S_CompleteQuest fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_CompleteQuest"));

	instance->Handle_CompleteQuest(fromPkt);

	return true;
}

bool Handle_S_UPDATE_QUEST(UMyGameInstance* instance, PROTOCOL::S_UpdateQuest fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_UpdateQuest"));

	instance->Handle_UpdateQuest(fromPkt);

	return true;
}



/*----------------------------------------------------
	ServerPacketHandler
-----------------------------------------------------*/
bool FServerPacketHandler::FlushPacket(TQueue<TFunction<void()>, EQueueMode::Spsc>& queue)
{
	if (_packetQueue.IsEmpty())
		return false;

	Swap(queue, _packetQueue);

	return true;
}

TFunction<void()> FServerPacketHandler::PopPacket()
{
	TFunction<void()> func = nullptr;

	_packetQueue.Dequeue(func);

	return func;
}

bool FServerPacketHandler::IsPacketQueueEmpty()
{
	return _packetQueue.IsEmpty();
}

void FServerPacketHandler::Init()
{
	for (int i = 0; i < PacketCount; i++) {
		_packetHandleFuncs[i] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
			return Handle_INVALID(instance, buffer, len);
		};
	}

	_packetHandleFuncs[PROTOCOL::MsgId::S_LOGIN] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Login>(Handle_S_LOGIN, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_ENTER_ROOM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Enter_Room>(Handle_S_ENTER_ROOM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_LEAVE_ROOM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Leave_Room>(Handle_S_LEAVE_ROOM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_SPAWN] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Spawn>(Handle_S_SPAWN, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_DESPAWN] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_DeSpawn>(Handle_S_DESPAWN, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_ITEM_LIST] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ItemList>(Handle_S_ITEMLIST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_MOVE] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Move>(Handle_S_MOVE, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_SKILL] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Skill>(Handle_S_SKILL, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_CHAT] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Chat>(Handle_S_CHAT, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_CHANGE_HP] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ChangeHp>(Handle_S_CHANGEHP, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_DIE] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_Die>(Handle_S_DIE, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_EQUIPITEM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_EquipItem>(Handle_S_EQUIPITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_USEITEM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_UseItem>(Handle_S_USEITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_ADD_ITEM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_AddItem>(Handle_S_ADDITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_UPDATE_ITEM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_UpdateItem>(Handle_S_UPDATEITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_REMOVE_ITEM] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_RemoveItem>(Handle_S_REMOVEITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_CHANGE_STAT] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ChangeStat>(Handle_S_CHANGE_STAT, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_CREATE_PLAYER] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_CreatePlayer>(Handle_S_CREATE_PLAYER, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_ADD_EXP] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_AddExp>(Handle_S_ADD_EXP, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_LEVEL_UP] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_LevelUp>(Handle_S_LEVEL_UP, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_ADD_QUEST] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_AddQuest>(Handle_S_ADD_QUEST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_QUEST_LIST] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_QuestList>(Handle_S_QUESTLIST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_REMOVE_QUEST] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_RemoveQuest>(Handle_S_REMOVE_QUEST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_COMPLETE_QUEST] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_CompleteQuest>(Handle_S_COMPLETE_QUEST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::S_UPDATE_QUEST] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_UpdateQuest>(Handle_S_UPDATE_QUEST, instance, buffer, len);
	};
}

/*---------------------------------------------------------------------------------------------
	HandlePacket - public
---------------------------------------------------------------------------------------------*/
bool FServerPacketHandler::HandlePacket(UMyGameInstance* instance, uint8* buffer, int len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	
	return _packetHandleFuncs[header->_id](instance, buffer, len);
}

/*---------------------------------------------------------------------------------------------
	 MakeSendBuffer - public
---------------------------------------------------------------------------------------------*/
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Login toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_LOGIN);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Enter_Room toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_ENTER_ROOM);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Leave_Room toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_LEAVE_ROOM);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Spawn toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_SPAWN);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Move toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_MOVE);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Skill toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_SKILL);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_Chat toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_CHAT);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_EquipItem toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_EQUIPITEM);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_UseItem toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_USEITEM);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_CreatePlayer toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_CREATE_PLAYER);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_AddQuest toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_ADD_QUEST);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_RemoveQuest toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_REMOVE_QUEST);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_CompleteQuest toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_COMPLETE_QUEST);
}

TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_UpdateQuest toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::C_UPDATE_QUEST);
}
