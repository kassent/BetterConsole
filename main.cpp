#include "F4SE/PluginAPI.h"
#include "F4SE/GameMenus.h"
#include "f4se/GameReferences.h"

#include "F4SE_common/F4SE_version.h"
#include "F4SE_common/Relocation.h"
#include "F4SE_common/SafeWrite.h"
#include "F4SE_common/BranchTrampoline.h"

#include "Relocation/RVA.h"

#include "Scaleform.h"
#include "HookUtil.h"
#include "Console.h"
#include "Settings.h"

#include <shlobj.h>

#include "xbyak/xbyak.h"

#define PLUGIN_VERSION	MAKE_EXE_VERSION(1, 5, 1)
#define PLUGIN_NAME		"BetterConsole"

IDebugLog						gLog;
PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
F4SEScaleformInterface			* g_scaleform = nullptr;
F4SEMessagingInterface			* g_messaging = nullptr;

//#pragma warning (push)
//#pragma warning (disable : 4200)
//struct TypeHierarchy
//{
//	struct Node
//	{
//		UInt32	    type;  //32bit RTTIType*
//		UInt32		depth;
//		UInt32		offset;
//	};
//
//	UInt32		memPtr;
//	UInt32		unk04;
//	UInt32		length;
//	UInt32		nodes;     //32bit Node**
//};
//
//
//struct RTTIType
//{
//	void	 ** vtbl;
//	UInt64	data;
//	char	name[0];
//};
//
//struct RTTILocator
//{
//	UInt32		sig, offset, cdOffset;
//	UInt32		typeDesc;
//	UInt32		herarchyDesc;
//	UInt32		thisLocator;
//};
//#pragma warning (pop)
//
//
//const RelocAddr<uintptr_t>	type_info_vtbl = 0x30CDC08;
//
//const RelocAddr<uintptr_t>  rdata_begin = 0x2C40000;
//const RelocAddr<uintptr_t>  rdata_end = 0x3702000;
//
//const RelocAddr<uintptr_t>  data_begin = 0x3702000;
//const RelocAddr<uintptr_t>  data_end = 0x676D000;
//
//const RelocAddr<uintptr_t>  text_begin = 0x1000;
//const RelocAddr<uintptr_t>  text_end = 0x2C35000;
//
//
//
//void SearchVTable(uintptr_t typeinfo_addr)
//{
//	for (uintptr_t i = rdata_begin.GetUIntPtr(); i < rdata_end.GetUIntPtr(); i += sizeof(int))
//	{
//		static uintptr_t iOffset = (uintptr_t)GetModuleHandle(nullptr);
//		if ((*reinterpret_cast<UInt32*>(i) + iOffset) == typeinfo_addr)
//		{
//			const RTTILocator * rtti = reinterpret_cast<RTTILocator*>(i - 0x0C);
//
//			if (rtti->sig != 1 || rtti->cdOffset != 0) continue;
//
//			uintptr_t location = reinterpret_cast<uintptr_t>(rtti);
//			for (uintptr_t i = rdata_begin.GetUIntPtr(); i < rdata_end.GetUIntPtr(); i += sizeof(uintptr_t))
//			{
//				uintptr_t * p = reinterpret_cast<uintptr_t*>(i);
//				if (*p == location)
//				{
//					uintptr_t * vtbl = reinterpret_cast<uintptr_t*>(p + 1);
//					if (text_begin.GetUIntPtr() <= vtbl[0] && vtbl[0] < text_end.GetUIntPtr())
//					{
//						_MESSAGE("VTBL: %08X	OFFSET: %04X	CLASS: %s", (uintptr_t)vtbl - iOffset, rtti->offset, (char*)(typeinfo_addr + 0x10));
//						TypeHierarchy* pHierarchy = reinterpret_cast<TypeHierarchy *>(rtti->herarchyDesc + iOffset);
//						UInt32 * pInt = reinterpret_cast<UInt32*>(pHierarchy->nodes + iOffset);
//
//						for (size_t i = 0; i < pHierarchy->length; ++i)
//						{
//							auto pNode = reinterpret_cast<TypeHierarchy::Node*>(pInt[i] + iOffset);
//							RTTIType* pRTTI = reinterpret_cast<RTTIType*>(pNode->type + iOffset);
//							_MESSAGE("		>> DEPTH: %d	OFFSET: %04X	CLASS: %s", pNode->depth, pNode->offset, pRTTI->name);
//						}
//					}
//				}
//			}
//			_MESSAGE(" ");
//		}
//	}
//};

#ifdef DEBUG


#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
class VirtualMachineEx
{
public:
	using FnRegisterFunction = void(__thiscall VirtualMachineEx::*)(IFunction * fn);
	static FnRegisterFunction	fnRegisterFunction;

	void RegisterFunction_Hook(IFunction * fn)
	{
		uintptr_t callback = *(uintptr_t*)((uintptr_t)fn + 0x50);
		_MESSAGE("<%s> %s (%016I64X) callback=%08X", fn->GetClassName()->c_str(), fn->GetName()->c_str(), (uintptr_t)fn, callback - (uintptr_t)GetModuleHandle(NULL));
		_MESSAGE("");
		(this->*fnRegisterFunction)(fn);
	}

	static void InitHook()
	{
		//Dump all papyrus functions.
		fnRegisterFunction = HookUtil::SafeWrite64(RelocAddr<uintptr_t>(0x030A8608).GetUIntPtr() + 8 * 0x1B, &RegisterFunction_Hook);//V1.10 .?AVVirtualMachine@Internal@BSScript@@
	}
};


VirtualMachineEx::FnRegisterFunction VirtualMachineEx::fnRegisterFunction = nullptr;

#endif


//// 18 
//class PipboyValue
//{
//public:
//
//	virtual ~PipboyValue();
//	virtual    void Unk01();    // Sets unk0C to 0
//	virtual void Unk02();    // pure
//	virtual void Unk03(void *arg1);
//	virtual void Unk04();    // pure
//
//	UInt32    unk08;    // 08 - init'd to incremental variable  hash code
//	UInt8    unk0C;    // 0C - init'd to 1
//	UInt8    unk0D;    // 0D - init'd to 1
//	UInt16    pad0E;    // 0E
//	PipboyValue    *unk10;    // 10
//};
//
//template <class T>
//class PipboyPrimitiveValue : public PipboyValue
//{
//public:
//
//	T    value;    // 18    
//};
//STATIC_ASSERT(offsetof(PipboyPrimitiveValue<bool>, value) == 0x18);
//
//class PipboyObject : public PipboyValue
//{
//public:
//
//	struct PipboyTableItem
//	{
//		BSFixedString    key;
//		PipboyValue        *value;
//
//		bool operator==(const BSFixedString & a_name) const { return key == a_name; }
//		operator BSFixedString() const { return key; }
//
//		static inline UInt32 GetHash(BSFixedString * key)
//		{
//			UInt32 hash;
//			CalculateCRC32_64(&hash, (UInt64)key->data, 0);
//			return hash;
//		}
//	};
//
//
//	virtual ~PipboyObject();
//
//	tHashSet<PipboyTableItem, BSFixedString>    table;    // 18
//														  //...
//};
//
//class PipboyArray : public PipboyValue
//{
//public:
//	virtual ~PipboyArray();
//	struct PipboyTableItem
//	{
//		BSFixedString    key;
//		PipboyValue        *value;
//
//		bool operator==(const BSFixedString & a_name) const { return key == a_name; }
//		operator BSFixedString() const { return key; }
//
//		static inline UInt32 GetHash(BSFixedString * key)
//		{
//			UInt32 hash;
//			CalculateCRC32_64(&hash, (UInt64)key->data, 0);
//			return hash;
//		}
//	};
//	tArray<PipboyValue*>	  unk18;
//	tHashSet<PipboyTableItem, BSFixedString>    table;    // 30
//	tArray<void*>			  unk60;
//};
//
//STATIC_ASSERT(offsetof(PipboyObject, table) == 0x18);




extern "C"
{

	F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,

		PLUGIN_VERSION,
		PLUGIN_NAME,
		"kassent",

		F4SEPluginVersionData::kAddressIndependence_Signatures,	// not version independent
		F4SEPluginVersionData::kStructureIndependence_NoStructs,	// not version independent (extended field)
		{ 0 },	// compatible with 1.10.980
		0,	// works with any version of the script extender. you probably do not need to put anything here
	};

	bool F4SEPlugin_Preload(const F4SEInterface* f4se)
	{
		std::unique_ptr<char[]> sPath(new char[MAX_PATH]);
		sprintf_s(sPath.get(), MAX_PATH, "%s%s.log", "\\My Games\\Fallout4\\F4SE\\", PLUGIN_NAME);
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, sPath.get());

		_MESSAGE("%s: %08X", PLUGIN_NAME, PLUGIN_VERSION);

		g_pluginHandle = f4se->GetPluginHandle();
		g_runtimeVersion = f4se->runtimeVersion;

		plugin_info.plugin_name = PLUGIN_NAME;
		plugin_info.runtime_version = f4se->runtimeVersion;

		if (f4se->isEditor)
		{
			_FATALERROR("loaded in editor, marking as incompatible");
			return false;
		}

		g_scaleform = (F4SEScaleformInterface*)f4se->QueryInterface(kInterface_Scaleform);
		if (!g_scaleform)
		{
			_FATALERROR("couldn't get scaleform interface");
			return false;
		}

		g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging)
		{
			_FATALERROR("couldn't get messaging interface");
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info)
	{
		std::unique_ptr<char[]> sPath(new char[MAX_PATH]);
		sprintf_s(sPath.get(), MAX_PATH, "%s%s.log", "\\My Games\\Fallout4\\F4SE\\", PLUGIN_NAME);
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, sPath.get());

		_MESSAGE("%s: %08X", PLUGIN_NAME, PLUGIN_VERSION);

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PLUGIN_NAME;
		info->version = PLUGIN_VERSION;

		g_pluginHandle = f4se->GetPluginHandle();
		g_runtimeVersion = f4se->runtimeVersion;

		plugin_info.plugin_name = "BetterConsole";
		plugin_info.runtime_version = f4se->runtimeVersion;

		if (f4se->isEditor)
		{
			_FATALERROR("loaded in editor, marking as incompatible");
			return false;
		}

		g_scaleform = (F4SEScaleformInterface *)f4se->QueryInterface(kInterface_Scaleform);
		if (!g_scaleform)
		{
			_FATALERROR("couldn't get scaleform interface");
			return false;
		}

		g_messaging = (F4SEMessagingInterface *)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging)
		{
			_FATALERROR("couldn't get messaging interface");
			return false;
		}

		return true;
	}


	bool F4SEPlugin_Load(const F4SEInterface * f4se)
	{

		if (!g_branchTrampoline.Create(1024 * 64))
		{
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}
		try
		{
			sig_scan_timer timer;

			if (g_runtimeVersion >= RUNTIME_VERSION_1_10_980) {
				InitRuntimeData_NG();
			}
			else {
				InitRuntimeData();
			}
		}
		catch (const no_result_exception & exception)
		{
			_MESSAGE(exception.what());
			MessageBoxA(nullptr, "Signature scan failed, please deactive BetterConsole and download newer version from Nexus.", "BetterConsole", MB_OK);
			return false;
		}

		Settings::LoadSettings();

		if (g_messaging != nullptr)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", F4SEMessageHandler);

		if (g_scaleform)
			g_scaleform->Register("BetterConsole", ScaleformCallback);
#ifdef DEBUG

		for (uintptr_t i = data_begin.GetUIntPtr(); i < data_end.GetUIntPtr(); i += sizeof(uintptr_t))
		{
			uintptr_t * pointer = reinterpret_cast<uintptr_t*>(i);
			if (*pointer == type_info_vtbl.GetUIntPtr())
			{
				SearchVTable(i);
			}
		}
		VirtualMachineEx::InitHook();
		RegisterCommands();
#endif
		return true;
	}
};


