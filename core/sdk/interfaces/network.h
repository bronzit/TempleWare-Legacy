#pragma once
#include "..\..\utils\utils.h"
class CNetworkStringTableContainer;
class ClassName
{
public:
	int64_t size; //0x0000
	class ClassData* class_data; //0x0008
}; //Size: 0x0010


class CNetworkGameClient
{
public:
	enum vtables
	{
		global_vars_ = 4,
		client_tick = 5,
		server_tick = 6,
		net_channel = 41,
		unk = 68,
		set_signon_state = 116,
		copy_new_entity = 123
	};
	CGlobalVarsBase* get_global_vars()
	{
		//xref: Curtime( %f )\nRealtime( %f )\n module: engine2
		return utils::CallVFunc<CGlobalVarsBase*, vtables::global_vars_>(this);
	}

	INetChannelInfo* get_net_channel()
	{
		return utils::CallVFunc<INetChannelInfo*, vtables::net_channel>(this, 0);
	}

	int get_client_tick()
	{
		return utils::CallVFunc<int, vtables::client_tick>(this);
	}

	int get_server_tick()
	{
		return utils::CallVFunc<int, vtables::server_tick>(this);
	}

	void get_prediction_tick(int* prediction_tick)
	{
		int unk{};
		return utils::CallVFunc<void, vtables::unk>(this, prediction_tick, &unk);
	}

	void m_SetInPrediction(bool value)
	{
		*(bool*)(std::uintptr_t(this) + 0xD4) = value;
	}

	bool m_InPrediction()
	{
		return *(bool*)(std::uintptr_t(this) + 0xD4);
	}

	int m_LastCommandCreated()
	{
		return *(int*)(std::uintptr_t(this) + 0xD8);
	}

	INetChannelInfo* get_net_channel2()
	{
		return *(INetChannelInfo**)(std::uintptr_t(this) + 0xE8);
	}

	float lerp()
	{
		return utils::CallVFunc<float, 62U>(this);
	}

	void set_lerp(float lerp)
	{
		*(float*)(std::uintptr_t(this) + 0x285F08) = lerp;
	}

	int m_server_count()
	{
		return *(int*)(std::uintptr_t(this) + 0x258);
	}

	int& m_delta_tick()
	{
		return *(int*)(std::uintptr_t(this) + 0x27C);
	}

	int m_command()
	{
		return *(int*)(std::uintptr_t(this) + 0x285E78);
	}

	void SetNetMsg(void* message_handle, void* message_data, int unk)
	{
		// xref: SetSignonState
		utils::CallVFunc<void, 68>(this, message_handle, message_data, unk);
	}

	bool IsConnected() {
		return utils::CallVFunc<bool, 13U>(this);
	}

	float GetClientInterpAmount() {
		return utils::CallVFunc<float, 62U>(this);
	}

	void FullUpdate() {
		utils::CallVFunc<void, 29U>(this, xorstr_("initial update"));
	}

	void Update() {
		int* deltaTickPtr = reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0x25C);
		*deltaTickPtr = -1;
	}

	int GetDeltaTick() {
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0x27C);
	}

	int m_delta() {
		return *(int*)(std::uintptr_t(this) + 0x278);
	}

public:
	char pad_0000[104]; //0x0000
	std::uintptr_t* N0000005C; //0x0068
	char pad_0070[48]; //0x0070
	CGlobalVarsBase global_vars; //0x00A0
	char pad_00F0[176]; //0x00F0
	ClassName class_names; //0x01A0
	char pad_01B0[40]; //0x01B0
	CNetworkStringTableContainer* CNetworkStringTableContainer; //0x01D8
	char pad_01E0[64]; //0x01E0
	char* map_vpk; //0x0220
	char* map_name; //0x0228
	char pad_0230[16]; //0x0230
	int64_t signon_state; //0x0240
	char pad_0248[16]; //0x0248
	int32_t server_count; //0x0258
	int32_t delta_tick; //0x025C
};

class INetworkClientService
{
public:
	enum msg : int
	{
		unknown = 23,
		voice_channel = 48
	};

	CNetworkGameClient* GetNetworkClient() {
		return utils::CallVFunc<CNetworkGameClient*, 23>(this);
	}
};
