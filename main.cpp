#include "F4SE/PluginAPI.h"
#include "F4SE/GameMenus.h"
#include "f4se/GameReferences.h"

#include "F4SE_common/F4SE_version.h"
#include "F4SE_common/Relocation.h"
#include "F4SE_common/SafeWrite.h"
#include "F4SE_common/BranchTrampoline.h"

#include "Scaleform.h"
#include "HookUtil.h"
#include "Console.h"
#include "Settings.h"

#include <shlobj.h>

#include "xbyak/xbyak.h"

#define PLUGIN_VERSION	MAKE_EXE_VERSION(1, 3, 0)

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

#ifdef DEBUG

#include "F4SE/GameInput.h"
#include "f4se/GameEvents.h"

///V1.9.4
class IdleInputEvent
{

};

class PipboyValueChangedEvent
{

};
//260
class PipboyMenu : public GameMenuBase,							// 000
	public BSTEventSink<MenuOpenCloseEvent>,		// 0E0
	public BSTEventSink<IdleInputEvent>			// 0E8

{
public:
	class PipboySubMenu : BSTEventSink<PipboyValueChangedEvent>
	{
	public:
		~PipboySubMenu();
		void			* unk08; // = address of (PipboyMenu + 0xF0)
		GFxValue		* stage;
	};

	class PipboyInventoryMenu : public PipboySubMenu
	{
	public:
		~PipboyInventoryMenu();
	};

	class PipboyValue
	{
	public:
		union Data
		{
			UInt32			u32;
			SInt32			s32;
			bool			boolean;
			const char		* string;
			PipboyValue		** values;
			double			number;
		};

		struct Node
		{
			UInt64			mask;
			PipboyValue		* value;
			Node			* next;
		};

		virtual ~PipboyValue();
		//void				** vtbl;
		UInt32				refCount;					// 08 ++dword_59F5290
		UInt16				unk0C;						// 0C ini'd to 0x101
		UInt64				unk10;						// 10
		Data				data;						// 18
		UInt32				unk20;						// 20
		UInt32				hashCode;					// 24	count?
		UInt32				count;						// 28
	};

	class PipboyObject : public PipboyValue
	{
	public:
		virtual ~PipboyObject();
		Node				* end;						// 30
		UInt64				unk38;						// 38
		Node				* nodes;					// 40
		UInt64				unk30[(0x98 - 0x48) >> 3];	// 48

		MEMBER_FN_PREFIX(PipboyObject);
		DEFINE_MEMBER_FN(GetMember, PipboyValue *, 0xB6BD60, UInt64& key);
		DEFINE_MEMBER_FN(DeleteMember, void, 0x0C179F0);
	};

	class PipboyArray : public PipboyValue
	{
	public:
		virtual ~PipboyArray();
		UInt64				unk30;						// 30
		UInt64				unk38;						// 38
		void				* unk40;					// 40 36623E4
		UInt64				unk48[(0x80 - 0x48) >> 3];	// 48

		MEMBER_FN_PREFIX(PipboyArray);
		DEFINE_MEMBER_FN(GetSize, UInt32, 0xBEA860);
		DEFINE_MEMBER_FN(ctor, PipboyArray*, 0xBEA760);

		inline PipboyValue * GetNthItem(UInt32 index)
		{
			return (index < count) ? data.values[index] : nullptr;
		}
	};

	//void								** vtbl;		// 02CCCA48
	UInt64								unkF0;			// seems to be an array.
	UInt32								unkF8;
	UInt64								unk100;
	UInt64								unk108;
	UInt64								unk110[(0x160 - 0x110) >> 3];
	PipboyInventoryMenu					inventory;      // 160
	UInt64								unk178[(0x258 - 0x178) >> 3];
	UInt32								unk258;
	bool								unk25C;
private:
	MEMBER_FN_PREFIX(PipboyMenu);
	DEFINE_MEMBER_FN(ctor, PipboyMenu*, 0x0B79740);
};

static_assert(sizeof(PipboyMenu) == 0x260, "sizeof(PipboyMenu) != 0x260");
static_assert(sizeof(PipboyMenu::PipboyObject) == 0x98, "sizeof(PipboyMenu::PipboyObject) == 0x98");
#endif

#ifdef DEBUG
#include "f4se/GameData.h"
#include "f4se/GameReferences.h"
struct ViewCasterUpdateEvent
{
	UInt32			crosshairHandler;	// 00	RefHandler
	UInt32			unk04;				// 04
	UInt32			unk08;				// 08
	UInt32			unk0C;				// 0C
	UInt64			unk10;				// 10
	UInt64			unk18;				// 18
	UInt64			unk20;				// 20
	bool			unk28;				// 28
	UInt32			unk2C;				// 2C
	bool			unk30;				// 30
	UInt64			isChanged;			// 38
};
///V1.10.20
class CrosshairChangeHandler : public BSTEventSink<ViewCasterUpdateEvent>
{
	//sub_9E1BE0
	//80 7F 40 00 0F 84 8A 00 00 00 80 7F 38 00 8B 47 08
public:
	virtual ~CrosshairChangeHandler() { };
	virtual	EventResult	ReceiveEvent(ViewCasterUpdateEvent * evn, void * dispatcher) override
	{
		if (evn->isChanged)
		{
			TESObjectREFR* pRef = nullptr;
			LookupREFRByHandle(&evn->crosshairHandler, &pRef);
			if (pRef != nullptr)
			{
				_MESSAGE("name: %s unk30: %d", CALL_MEMBER_FN(pRef, GetReferenceName)(), evn->unk30);
				pRef->handleRefObject.DecRefHandle();
			}
		}
		return kEvent_Continue;
	}
	static void Register()
	{
		RelocPtr<BSTEventDispatcher<ViewCasterUpdateEvent> *> pDispatcher = 0x5A7DAE8;//V1.10
		if (*pDispatcher != nullptr)
		{
			reinterpret_cast<BSTEventDispatcher<ViewCasterUpdateEvent> *>((uintptr_t)(*pDispatcher) + 0x10)->AddEventSink(new CrosshairChangeHandler());
		}
		else
		{
			_MESSAGE("Can't register crosshair update handler...");
		}
	}
};

class TESLoadGameHandler : public BSTEventSink<TESLoadGameEvent>
{
public:
	virtual ~TESLoadGameHandler() { };
	virtual	EventResult	ReceiveEvent(TESLoadGameEvent * evn, void * dispatcher) override
	{
		CrosshairChangeHandler::Register();
		return kEvent_Continue;
	}
};
#endif

bool ScaleformCallback(GFxMovieView * view, GFxValue * value)
{
	GFxMovieRoot * movieRoot = view->movieRoot;
	if (movieRoot)
	{
		GFxValue loaderInfo;
		if (movieRoot->GetVariable(&loaderInfo, "root.loaderInfo.url"))
		{
			std::string sResult = loaderInfo.GetString();
			if (sResult.find("Console.swf") != std::string::npos)
			{
				RegisterFunction<BetterConsole_GetBaseData>(value, view->movieRoot, "GetBaseData");
				RegisterFunction<BetterConsole_GetExtraData>(value, view->movieRoot, "GetExtraData");
				RegisterFunction<BetterConsole_WriteLog>(value, view->movieRoot, "WriteLog");
			}
		}
	}
	return true;
}

//0xF0
class Console : public GameMenuBase
{
public:
	using	FnPickNext = bool(__thiscall Console::*)(void* pData);
	static	FnPickNext	 fnPickNext;

	bool	PickNext_Hook(void * pData)
	{
		GFxValue result(true);
		if (stage.IsDisplayObject())
		{
			stage.Invoke("canScrollMouse", &result, nullptr, 0);
		}
		if (!result.GetBool())
		{
			return false;
		}
		return (this->*fnPickNext)(pData);
	}

	static const char * GetReferenceName(TESObjectREFR * pRef)
	{
		return pRef->GetReferenceName();
	}

	static void InitHook()
	{
		fnPickNext = HookUtil::SafeWrite64(RelocAddr<uintptr_t>(0x02DAE938).GetUIntPtr() + 8 * 0xF, &PickNext_Hook); //V1.10.26 //48 8D 05 ? ? ? ? BD ? ? ? ? F3 0F 11 44 24 ? //02DAE938
		g_branchTrampoline.Write6Call(RelocAddr<uintptr_t>(0x125CF0F).GetUIntPtr(), (uintptr_t)GetReferenceName); //FF 90 ? ? ? ? 48 89 74 24 ? 4C 89 7C 24 ?
	}
};
Console::FnPickNext			Console::fnPickNext = nullptr;



class MenuOpenCloseHandler : public BSTEventSink<MenuOpenCloseEvent>
{
public:
	virtual ~MenuOpenCloseHandler() { };
	virtual	EventResult	ReceiveEvent(MenuOpenCloseEvent * evn, void * dispatcher) override
	{ 
		static BSFixedString console("Console");
		if (evn->menuName == console && evn->isOpen)
		{
			GFxValue dispatchEvent;
			GFxValue eventArgs[3];
			IMenu * pConsole = (*g_ui)->GetMenu(&console);
			auto * movieRoot = pConsole->movie->movieRoot;
			movieRoot->CreateString(&eventArgs[0], "OnConsoleOpen");
			eventArgs[1].SetBool(true);
			eventArgs[2].SetBool(false);
			movieRoot->CreateObject(&dispatchEvent, "flash.events.Event", eventArgs, 3);
			movieRoot->Invoke("root.dispatchEvent", nullptr, &dispatchEvent, 1);
		}
		return kEvent_Continue; 
	};

	static void Register()
	{
		static auto * pHandler = new MenuOpenCloseHandler();
		(*g_ui)->menuOpenCloseEventSource.AddEventSink(pHandler); //V1.10.26
	}
};

void F4SEMessageHandler(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == F4SEMessagingInterface::kMessage_GameLoaded)
	{
		MenuOpenCloseHandler::Register();
	}
}



extern "C"
{
	bool F4SEPlugin_Query(const F4SEInterface * f4se, PluginInfo * info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\BetterConsole.log");
		_MESSAGE("BetterConsole: %08X", PLUGIN_VERSION);

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "BetterConsole";
		info->version = PLUGIN_VERSION;

		g_pluginHandle = f4se->GetPluginHandle();

		if (f4se->runtimeVersion != RUNTIME_VERSION_1_10_50)
		{
			MessageBox(nullptr, "UNSUPPORTED GAME VERSION.THE REQUIRED VERSION IS: V1.10.40", "BetterConsole", MB_OK);
			return false;
		}

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

		if (g_messaging != nullptr)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", F4SEMessageHandler);

		if (g_scaleform)
			g_scaleform->Register("BetterConsole", ScaleformCallback);

		Settings::LoadSettings();

		Console::InitHook();

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


