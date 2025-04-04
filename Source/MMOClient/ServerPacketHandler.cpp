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

bool Handle_S_LOGIN(UMyGameInstance* instance, PROTOCOL::S_LOGIN fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_Login"));

	instance->Handle_Login(fromPkt);
	
	return true;
}

// 본인(로그인, 종료)
bool Handle_S_ENTER_ROOM(UMyGameInstance* instance, PROTOCOL::S_ENTER_ROOM fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_EnterRoom"));

	if (fromPkt.success()) 
		instance->Handle_EnterRoom(fromPkt.object());

	return true;
}

bool Handle_S_LEAVE_ROOM(UMyGameInstance* instance, PROTOCOL::S_LEAVE_ROOM fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_LeaveRoom"));

	instance->Handle_LeaveRoom(fromPkt);
	
	return true;
}

bool Handle_S_SPAWN(UMyGameInstance* instance, PROTOCOL::S_SPAWN fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_Spawn"));

	instance->Handle_Spawn(fromPkt);

	return true;
}

bool Handle_S_DESPAWN(UMyGameInstance* instance, PROTOCOL::S_DESPAWN fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_DeSpawn"));
	
	instance->Handle_DeSpawn(fromPkt);
	
	return true;
}

bool Handle_S_ITEMLIST(UMyGameInstance* instance, PROTOCOL::S_ITEMLIST fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_ItemList - Count:%d"), fromPkt.items_size());

	instance->Handle_ItemList(fromPkt);

	return true;
}

bool Handle_S_MOVE(UMyGameInstance* instance, PROTOCOL::S_MOVE fromPkt)
{
	instance->_playerController->MoveUpdate(fromPkt.object());
	
	return true;
}

bool Handle_S_SKILL(UMyGameInstance* instance, PROTOCOL::S_SKILL fromPkt)
{
	instance->Handle_Skill(fromPkt);

	return true;
}

bool Handle_S_CHAT(UMyGameInstance* instance, PROTOCOL::S_CHAT fromPkt)
{
	instance->Handle_Chat(fromPkt);

	return true;
}

bool Handle_S_CHANGEHP(UMyGameInstance* instance, PROTOCOL::S_CHANGE_HP fromPkt)
{
	instance->Handle_ChangeHp(fromPkt);

	return true;
}

bool Handle_S_DIE(UMyGameInstance* instance, PROTOCOL::S_DIE fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_Die"));

	instance->Handle_Die(fromPkt);
	
	return true;
}

bool Handle_S_EQUIPITEM(UMyGameInstance* instance, PROTOCOL::S_EQUIP_ITEM fromPkt)
{
	instance->Handle_EquipItem(fromPkt);

	return true;
}

bool Handle_S_USEITEM(UMyGameInstance* instance, PROTOCOL::S_USE_ITEM fromPkt)
{
	instance->Handle_UseItem(fromPkt);

	return true;
}

bool Handle_S_ADDITEM(UMyGameInstance* instance, PROTOCOL::S_ADD_ITEM fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_AddItem"));

	instance->Handle_AddItem(fromPkt);

	return true;
}

bool Handle_S_UPDATEITEM(UMyGameInstance* instance, PROTOCOL::S_UPDATE_ITEM fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_UpdateItem"));

	instance->Handle_UpdateItem(fromPkt);

	return true;
}

bool Handle_S_REMOVEITEM(UMyGameInstance* instance, PROTOCOL::S_REMOVE_ITEM fromPkt)
{
	instance->Handle_RemoveItem(fromPkt);

	return true;
}

// 장비 변경, 레벨업, 
bool Handle_S_CHANGE_STAT(UMyGameInstance* instance, PROTOCOL::S_CHANGE_STAT fromPkt)
{
	instance->Handle_ChangeStat(fromPkt.object());

	return true;
}

bool Handle_S_CREATE_PLAYER(UMyGameInstance* instance, PROTOCOL::S_CREATE_PLAYER fromPkt)
{
	instance->Handle_CreatePlayer(fromPkt.object());
	
	return false;
}

bool Handle_S_ADD_EXP(UMyGameInstance* instance, PROTOCOL::S_ADD_EXP fromPkt)
{
	instance->Handle_AddExp(fromPkt.exp());

	return true;
}

bool Handle_S_LEVEL_UP(UMyGameInstance* instance, PROTOCOL::S_LEVEL_UP fromPkt)
{
	instance->Handle_ChangeStat(fromPkt.info());
	
	// TODO : 레벨업 이펙트
	Cast<UMyHUDWidget>(instance->_uiManager->_mainUI)->PlayLevelUpWidgetAnim();

	return true;
}

bool Handle_S_ADD_QUEST(UMyGameInstance* instance, PROTOCOL::S_ADD_QUEST fromPkt)
{
	instance->Handle_AddQuest(fromPkt.quest());

	return true;
}

bool Handle_S_QUESTLIST(UMyGameInstance* instance, PROTOCOL::S_QUESTLIST fromPkt)
{
	for (int i = 0; i < fromPkt.quests_size(); i++) 
		instance->Handle_AddQuest(fromPkt.quests(i));

	return true;
}

bool Handle_S_REMOVE_QUEST(UMyGameInstance* instance, PROTOCOL::S_REMOVE_QUEST fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_RemoveQuest"));

	instance->Handle_RemoveQuest(fromPkt);

	return true;
}

bool Handle_S_COMPLETE_QUEST(UMyGameInstance* instance, PROTOCOL::S_COMPLETE_QUEST fromPkt)
{
	UE_LOG(LogTemp, Error, TEXT("Packet S_CompleteQuest"));

	instance->Handle_CompleteQuest(fromPkt);

	return true;
}

bool Handle_S_UPDATE_QUEST(UMyGameInstance* instance, PROTOCOL::S_UPDATE_QUEST fromPkt)
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

	_packetHandleFuncs[PROTOCOL::MsgId::s_login] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_LOGIN>(Handle_S_LOGIN, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_enter_room] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ENTER_ROOM>(Handle_S_ENTER_ROOM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_leave_room] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_LEAVE_ROOM>(Handle_S_LEAVE_ROOM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_spawn] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_SPAWN>(Handle_S_SPAWN, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_despawn] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_DESPAWN>(Handle_S_DESPAWN, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_itemlist] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ITEMLIST>(Handle_S_ITEMLIST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_move] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_MOVE>(Handle_S_MOVE, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_skill] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_SKILL>(Handle_S_SKILL, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_chat] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_CHAT>(Handle_S_CHAT, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_change_hp] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_CHANGE_HP>(Handle_S_CHANGEHP, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_die] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_DIE>(Handle_S_DIE, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_equip_item] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_EQUIP_ITEM>(Handle_S_EQUIPITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_use_item] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_USE_ITEM>(Handle_S_USEITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_add_item] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ADD_ITEM>(Handle_S_ADDITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_update_item] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_UPDATE_ITEM>(Handle_S_UPDATEITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_remove_item] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_REMOVE_ITEM>(Handle_S_REMOVEITEM, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_change_stat] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_CHANGE_STAT>(Handle_S_CHANGE_STAT, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_create_player] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_CREATE_PLAYER>(Handle_S_CREATE_PLAYER, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_add_exp] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ADD_EXP>(Handle_S_ADD_EXP, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_level_up] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_LEVEL_UP>(Handle_S_LEVEL_UP, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_add_quest] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_ADD_QUEST>(Handle_S_ADD_QUEST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_questlist] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_QUESTLIST>(Handle_S_QUESTLIST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_remove_quest] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_REMOVE_QUEST>(Handle_S_REMOVE_QUEST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_complete_quest] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_COMPLETE_QUEST>(Handle_S_COMPLETE_QUEST, instance, buffer, len);
	};
	_packetHandleFuncs[PROTOCOL::MsgId::s_update_quest] = [this](UMyGameInstance* instance, uint8* buffer, int len) {
		return HandlePacket<PROTOCOL::S_UPDATE_QUEST>(Handle_S_UPDATE_QUEST, instance, buffer, len);
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
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_LOGIN toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_login);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_ENTER_ROOM toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_enter_room);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_LEAVE_ROOM toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_leave_room);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_SPAWN toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_spawn);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_MOVE toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_move);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_SKILL toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_skill);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_CHAT toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_chat);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_EQUIP_ITEM toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_equip_item);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_USE_ITEM toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_use_item);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_CREATE_PLAYER toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_create_player);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_ADD_QUEST toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_add_quest);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_REMOVE_QUEST toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_remove_quest);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_COMPLETE_QUEST toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_complete_quest);
}
TSharedPtr<FSendBuffer> FServerPacketHandler::MakeSendBuffer(PROTOCOL::C_UPDATE_QUEST toPkt)
{
	return MakeSendBuffer(toPkt, PROTOCOL::MsgId::c_update_quest);
}
