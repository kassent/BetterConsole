#include "F4SE_common/BranchTrampoline.h"
#include "F4SE/ScaleformMovie.h"
#include "F4SE/ScaleformValue.h"
#include "F4SE/ScaleformState.h"
#include "F4SE/ScaleformLoader.h"
#include "F4SE/ScaleformTranslator.h"
#include "F4SE/ScaleformCallbacks.h"
#include "F4SE/GameCamera.h"
#include "F4SE/GameMenus.h"
#include "f4se/GameHandle.h"
#include "HookUtil.h"

#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <string_view>
#include <format>
#include <cmath>
#include <numbers>

#include "Scaleform.h"
#include "GameData.h"
#include "Settings.h"

std::vector<std::string> FORM_TYPES =
{
	"NONE",
	"TES4",
	"GRUP",
	"GMST",
	"KYWD",
	"LCRT",
	"AACT",
	"TRNS",
	"CMPO",
	"TXST",
	"MICN",
	"GLOB",
	"DMGT",
	"CLAS",
	"FACT",
	"HDPT",
	"EYES",
	"RACE",
	"SOUN",
	"ASPC",
	"SKIL",
	"MGEF",
	"SCPT",
	"LTEX",
	"ENCH",
	"SPEL",
	"SCRL",
	"ACTI",
	"TACT",
	"ARMO",
	"BOOK",
	"CONT",
	"DOOR",
	"INGR",
	"LIGH",
	"MISC",
	"STAT",
	"SCOL",
	"MSTT",
	"GRAS",
	"TREE",
	"FLOR",
	"FURN",
	"WEAP",
	"AMMO",
	"NPC_",
	"LVLN",
	"KEYM",
	"ALCH",
	"IDLM",
	"NOTE",
	"PROJ",
	"HAZD",
	"BNDS",
	"SLGM",
	"TERM",
	"LVLI",
	"WTHR",
	"CLMT",
	"SPGD",
	"RFCT",
	"REGN",
	"NAVI",
	"CELL",
	"REFR",
	"ACHR",
	"PMIS",
	"PARW",
	"PGRE",
	"PBEA",
	"PFLA",
	"PCON",
	"PBAR",
	"PHZD",
	"WRLD",
	"LAND",
	"NAVM",
	"TLOD",
	"DIAL",
	"INFO",
	"QUST",
	"IDLE",
	"PACK",
	"CSTY",
	"LSCR",
	"LVSP",
	"ANIO",
	"WATR",
	"EFSH",
	"TOFT",
	"EXPL",
	"DEBR",
	"IMGS",
	"IMAD",
	"FLST",
	"PERK",
	"BPTD",
	"ADDN",
	"AVIF",
	"CAMS",
	"CPTH",
	"VTYP",
	"MATT",
	"IPCT",
	"IPDS",
	"ARMA",
	"ECZN",
	"LCTN",
	"MESG",
	"RGDL",
	"DOBJ",
	"DFOB",
	"LGTM",
	"MUSC",
	"FSTP",
	"FSTS",
	"SMBN",
	"SMQN",
	"SMEN",
	"DLBR",
	"MUST",
	"DLVW",
	"WOOP",
	"SHOU",
	"EQUP",
	"RELA",
	"SCEN",
	"ASTP",
	"OTFT",
	"ARTO",
	"MATO",
	"MOVT",
	"SNDR",
	"DUAL",
	"SNCT",
	"SOPM",
	"COLL",
	"CLFM",
	"REVB",
	"PKIN",
	"RFGP",
	"AMDL",
	"LAYR",
	"COBJ",
	"OMOD",
	"MSWP",
	"ZOOM",
	"INNR",
	"KSSM",
	"AECH",
	"SCCO",
	"AORU",
	"SCSN",
	"STAG",
	"NOCM",
	"LENS",
	"LSPR",
	"GDRY",
	"OVIS"
};

std::vector<std::string> ACTOR_VALUE_TYPES = 
{
	"DERIVED ATTRIBUTE",
	"SPECIAL",
	"SKILL",
	"AI ATTRIBUTE",
	"RESISTANCE",
	"CONDITION",
	"UNKNOWN",
	"INT VALUE",
	"VARIABLE",
	"RESOURCE",
	"UNKNOWN"
};

std::vector<std::string> WEAPON_TYPES =
{
	"UNARMED",
	"ONE-HANDED SWORD",
	"ONE-HANDED DAGGER",
	"ONE-HANDED AXE",
	"ONE-HANDED MACE",
	"TWO-HANDED SWORD",
	"TWO-HANDED AXE",
	"BOW",
	"STAFF",
	"GUN",
	"GRENADE",
	"MINE",
	"UNDEFINED"
};


template<typename T>
inline void Register(GFxValue * dst, const char * name, T value)
{

}

template<>
inline void Register(GFxValue * dst, const char * name, SInt32 value)
{
	GFxValue	fxValue;
	fxValue.SetInt(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue * dst, const char * name, UInt32 value)
{
	GFxValue	fxValue;
	fxValue.SetUInt(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue * dst, const char * name, double value)
{
	GFxValue	fxValue;
	fxValue.SetNumber(value);
	dst->SetMember(name, &fxValue);
}

template<>
inline void Register(GFxValue * dst, const char * name, bool value)
{
	GFxValue	fxValue;
	fxValue.SetBool(value);
	dst->SetMember(name, &fxValue);
}

inline void RegisterUnmanagedString(GFxValue * dst, std::string_view name, std::string_view str)
{
	GFxValue	fxValue;
	fxValue.SetString(str.data());
	dst->SetMember(name.data(), &fxValue);
}

inline void RegisterString(GFxValue * dst, GFxMovieRoot * view, std::string_view name, std::string_view str)
{
	GFxValue	fxValue;
	view->CreateString(&fxValue, str.data());
	dst->SetMember(name.data(), &fxValue);
}

#pragma warning (push)
#pragma warning (disable : 4200)
struct Runtime_RTTIType
{
	void	* typeInfo;
	UInt64	data;
	char	name[0];
};

struct Runtime_RTTILocator
{
	UInt32		sig, offset, cdOffset;
	UInt32		typeDesc;
	UInt32		classDesc;
};
#pragma warning (pop)


#define DEFINE_CLASS_VTABLE(className, offset)		\
	RelocAddr<uintptr_t> className##_vtbl(offset);

#define RELOC_CLASS_VTABLE(className, ...)			\
	className##_vtbl = sig_scan_processor("VTABLE_"#className, __VA_ARGS__).GetOffset(); 

#define GET_CLASS_VTABLE(className)			\
	className##_vtbl.GetUIntPtr()
	
RelocPtr<Runtime_RTTIType>			Runtime_RTTI_TESFullName(0x02D29EB0); // 02D29EB0
RelocPtr<Runtime_RTTIType>			Runtime_RTTI_TESForm(0x02D27678);

using _VisitPerks = void(*)(Actor*, HasPerkVisitor&);
RelocAddr<_VisitPerks> VisitPerks(0x0C0BC30); // sub_140C0BC30

using _CalcMagicEffect = void(*)(MagicItem*, MagicItem::EffectItem*, float& magnitude, float& duration);
RelocAddr<_CalcMagicEffect> CalcMagicEffect(0x0A96ED0);///V1.10 CalaPerkEffect in this funtion.BC2D70  //48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 55 56

using _GetRestoredHPPercent = float(*)(MagicItem*, MagicItem::EffectItem*);
RelocAddr<_GetRestoredHPPercent> GetRestoredHPPercent(0x1000DA0); ///V1.10.26 //48 8B C4 53 48 83 EC 40 0F 29 70 E8

using _GetModifiedValueName = void(*)(MagicItem::EffectItem*, MagicItem::EffectItem::Description&);
RelocAddr<_GetModifiedValueName> GetModifiedValueName(0x0ACF980); ///V1.10.26 C0AE60 //48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B F2 48 8B E9 48 8D 15 ? ? ? ?

using _GetActorValueHolder = ActorValueInfo**(*)();
RelocAddr<_GetActorValueHolder> GetActorValueCollector(0x020E9F0); //E8 ? ? ? ? 4C 8B 46 58 48 8D 4E 58 48 8B 90 ? ? ? ? 41 FF 50 08 0F 57 C9

using _GetChargeKW = BGSKeyword*(*)();
RelocAddr<_GetChargeKW> GetChargeKW(0x03200B0); /// sub_1403200B0

using _GeHealthtKW = BGSKeyword*(*)();
RelocAddr<_GeHealthtKW> GeHealthtKW(0x0320110); /// sub_140320110

//using _ReleaseSimpleColllector = void(*)(BGSInventoryItem::SimpleDataCollector&);
//RelocAddr<_ReleaseSimpleColllector>	ReleaseSimpleColllector(0x153F730); /// sub_14153F730

using _GetWeaponRange = float(*)(ObjectInstanceData&);
RelocAddr<_GetWeaponRange> GetWeaponRange(0x0679C30);/// sub_140679C30

using _GetWeaponAccuracy = float(*)(ObjectInstanceData&, Actor*);
RelocAddr<_GetWeaponAccuracy> GetWeaponAccuracy(0x0679D10); /// sub_140679D10

using _GetWeaponSpeedDesc = const char * (*)(UInt32);
RelocAddr<_GetWeaponSpeedDesc>	GetWeaponSpeedDesc(0x0425810); /// sub_140425810

using _GetStringFromHash = const char *(*)(UInt64 &);
RelocAddr<_GetStringFromHash> GetStringFromHash(0x02FCD00); /// sub_1402FCD00

using _CalcInstanceData = ObjectInstanceData *(*)(ObjectInstanceData & out, TESForm *, TBO_InstanceData *);
RelocAddr<_CalcInstanceData>	CalcInstanceData(0x03E29D0); /// sub_1403E29D0

using _RegisterMenuOpenCloseEvent = void(*)(BSTEventDispatcher<MenuOpenCloseEvent> & dispatcher, BSTEventSink<MenuOpenCloseEvent> * pHandler);
RelocAddr <_RegisterMenuOpenCloseEvent>	RegisterMenuOpenCloseEvent(0x0612530);// sub_140612530

//RelocAddr <float *>    fNewBatteryCapacity(0x3761848);
//
//RelocPtr <BGSKeyword *>	pSyringerAmmoKW(0x591B350);///V1.10.26 ObjectTypeSyringerAmmo [KYWD:0018506F] 48 8B 15 ? ? ? ? 49 8D 8D ? ? ? ? 4C 89 6C 24 ?


namespace {

	enum FilterFlag {
		FILTER_FLAG_INACTIVE = 1,
		FILTER_FLAG_ACTIVE = 2,
	};

	void AppendInfoToArray(GFxValue& array, GFxMovieRoot* root, std::string_view key, std::string_view value, UInt32 filterFlag = FILTER_FLAG_ACTIVE)
	{
		GFxValue object;
		root->CreateObject(&object);
		RegisterString(&object, root, "key", key);
		RegisterString(&object, root, "value", value);
		Register<UInt32>(&object, "filterFlag", filterFlag);
		array.PushBack(&object);
	}

	void AppendInfoToArray(GFxValue& array, GFxMovieRoot* root, std::string_view text, UInt32 filterFlag = FILTER_FLAG_ACTIVE, std::function<void(GFxValue&)> callback = {})
	{
		GFxValue object;
		root->CreateObject(&object);
		RegisterString(&object, root, "text", text);
		Register<UInt32>(&object, "filterFlag", filterFlag);
		if (callback)
		{
			callback(object);
		}
		array.PushBack(&object);
	}

	[[nodiscard]] inline std::string_view GetModName(TESForm* form) {
		if (!form) return std::string_view(Settings::EXTRAINFO_NULL);
		auto* pModInfo = (*g_dataHandler)->GetLocatedModInfo(form);
		return (pModInfo != nullptr && pModInfo->name) ? std::string_view(pModInfo->name) : std::string_view(Settings::EXTRAINFO_NULL);
	}

	[[nodiscard]] inline std::string_view GetStringOrDefault(const char* str, std::string_view defaultValue = std::string_view(Settings::EXTRAINFO_NULL)) {
		return (str != nullptr && str[0] != '\0') ? std::string_view(str) : defaultValue;
	}


	[[nodiscard]] std::string GetFormDisplayName(TESForm* form)
	{
		TESFullName* pFullName = (TESFullName*)Runtime_DynamicCast_Internal(form, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
		return (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL.c_str();
	}

}


class BetterConsole_GetBaseData : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args)
	{
		// TESObjectREFR * pRef = nullptr;
		NiPointer<TESObjectREFR> pRef;
		LookupREFRByHandle(*g_consoleHandle, pRef);
		if (pRef != nullptr)
		{
			args->movie->movieRoot->CreateObject(args->result);
			TESForm * pBaseForm = pRef->baseForm;
			BSExtraData * pExtraData = pRef->extraDataList->GetExtraData(kExtraData_LeveledCreature);
			if (pExtraData != nullptr)
			{
				auto * leveledBaseForm = *reinterpret_cast<TESForm**>((uintptr_t)pExtraData + 0x18);
				pBaseForm = (leveledBaseForm != nullptr) ? leveledBaseForm : pRef->baseForm;
			}

			RegisterString(args->result, args->movie->movieRoot, "refFormID", std::format("{:08X}", pRef->formID));
			RegisterString(args->result, args->movie->movieRoot, "refFormType", FORM_TYPES[pRef->formType]);
			RegisterString(args->result, args->movie->movieRoot, "baseFormID", std::format("{:08X}", pBaseForm->formID));
			RegisterString(args->result, args->movie->movieRoot, "baseFormType", FORM_TYPES[pBaseForm->formType]);

			const char * pRefName = pRef->GetReferenceName();
			auto pExtraTextData = static_cast<ExtraTextDisplayData*>(pRef->extraDataList->GetExtraData(kExtraData_TextDisplayData));
			if (pExtraTextData != nullptr)
			{
				pRefName = pExtraTextData->GetReferenceName(pBaseForm)->c_str();
			}
			if (pRefName && pRefName[0])
			{
				RegisterString(args->result, args->movie->movieRoot, "refName", pRefName);
			}
			TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pBaseForm, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
			if (pFullName && strlen(pFullName->name.c_str()))
			{
				RegisterString(args->result, args->movie->movieRoot, "baseName", pFullName->GetFullName());
			}
			auto * pModInfo = (*g_dataHandler)->GetLocatedModInfo(pBaseForm);
			const char * baseDefineModName = (pModInfo != nullptr) ? pModInfo->name : "undefined";
			RegisterString(args->result, args->movie->movieRoot, "baseDefineModName", baseDefineModName);

			pModInfo = (*g_dataHandler)->GetLocatedModInfo(pRef);
			const char * refDefineModName = (pModInfo != nullptr) ? pModInfo->name : "undefined";
			RegisterString(args->result, args->movie->movieRoot, "refDefineModName", refDefineModName);

			RegisterString(args->result, args->movie->movieRoot, "refMemory", std::format("{:016X}", (uintptr_t)pRef.get()));

			pModInfo = (*g_dataHandler)->GetLastChangeModInfo(pBaseForm);

			RegisterString(args->result, args->movie->movieRoot, "baseLastChangeModName", (pModInfo != nullptr) ? pModInfo->name : "undefined");

			// pRef->handleRefObject.DecRefHandle();
		}
	}
};


class BetterConsole_GetExtraData : public GFxFunctionHandler
{
public:

	static void	CollectPositionData(GFxValue& object, GFxMovieRoot * root, TESObjectREFR *  pRef)
	{
		if (object.GetType() == GFxValue::kType_Object && root != nullptr)
		{
			GFxValue posArray;
			root->CreateArray(&posArray); //extraData

			// posX
			AppendInfoToArray(posArray, root, std::format("{}: ", Settings::POSITIONINFO_POSX), std::format("{:.2f}", pRef->pos.x));

			// posY
			AppendInfoToArray(posArray, root, std::format("{}: ", Settings::POSITIONINFO_POSY), std::format("{:.2f}", pRef->pos.y));

			// posZ
			AppendInfoToArray(posArray, root, std::format("{}: ", Settings::POSITIONINFO_POSZ), std::format("{:.2f}", pRef->pos.z));

			auto RadToDeg = [](float rad) -> float {
				return rad * 180.0f / std::numbers::pi;
			};

			// rotX
			AppendInfoToArray(posArray, root, std::format("{}: ", Settings::POSITIONINFO_ROTX), std::format("{:.2f}/{:.2f}", pRef->rot.x, RadToDeg(pRef->rot.x)));

			// rotY
			AppendInfoToArray(posArray, root, std::format("{}: ", Settings::POSITIONINFO_ROTY), std::format("{:.2f}/{:.2f}", pRef->rot.y, RadToDeg(pRef->rot.y)));

			// rotZ
			AppendInfoToArray(posArray, root, std::format("{}: ", Settings::POSITIONINFO_ROTZ), std::format("{:.2f}/{:.2f}", pRef->rot.z, RadToDeg(pRef->rot.z)));

			object.SetMember("extraInfo", &posArray);
		}
	}

	static void	ResolvePerkData(GFxValue& parent, GFxMovieRoot * root, Actor *  actor, UInt32&	dataListSize)
	{
		if (parent.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
		{
			if (actor->middleProcess != nullptr)
			{
				HasPerkVisitor visitor;
				VisitPerks(actor, visitor);
				dataListSize = visitor.result.size();
				if (!dataListSize)	return;
				GFxValue perkArray;
				root->CreateArray(&perkArray); //extraData
				for (auto& pEntry : visitor.result)
				{
					auto displayName = GetFormDisplayName(pEntry.perk);


					GFxValue extraInfo;
					root->CreateArray(&extraInfo);

					// perk name
					AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, displayName));

					// perk ID
					AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pEntry.perk->formID));

					// perk level
					AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::PERKINFO_LEVEL, pEntry.perk->perkLevel));

					// perk rank
					AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::PERKINFO_RANK, pEntry.rank));

					// perk mod
					auto* pModInfo = (*g_dataHandler)->GetLocatedModInfo(pEntry.perk);
					AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));

					AppendInfoToArray(perkArray, root, displayName, FILTER_FLAG_ACTIVE, [&extraInfo](GFxValue& object) {
						object.SetMember("extraInfo", &extraInfo);
					});
				}
				parent.SetMember("extraInfo", &perkArray);
			}
		}
	}

	static void	ResolveFactionData(GFxValue& parent, GFxMovieRoot * root, Actor *  actor, UInt32& dataListSize)
	{
		if (parent.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
		{
			std::map<TESFaction*, SInt8>	collector;
			BSExtraData * pExtraData = actor->extraDataList->GetExtraData(kExtraData_FactionChanges);
			if (pExtraData != nullptr)
			{
				auto * pFactionChanges = static_cast<ExtraFactionChanges*>(pExtraData);
				for (uintptr_t i = 0; i < pFactionChanges->factions.count; ++i)
				{
					const auto factionInfo = pFactionChanges->factions[i];
					collector.insert({ factionInfo.faction, factionInfo.rank });
				}
			}
			TESNPC * npc = static_cast<TESNPC*>(actor->baseForm);
			for (uintptr_t i = 0; i < npc->actorData.factions.count; ++i)
			{
				const auto factionInfo = npc->actorData.factions[i];
				collector.insert({ factionInfo.faction, factionInfo.rank });
			}

			dataListSize = collector.size();
			if (!dataListSize)	return;
			GFxValue factionArray;
			root->CreateArray(&factionArray); //extraData

			for (const auto& info : collector)
			{
				auto displayName = GetFormDisplayName(info.first);


				GFxValue extraInfo;
				root->CreateArray(&extraInfo); //extraData

				// faction ID
				AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, info.first->formID));

				// faction name
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, displayName));

				// faction rank
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::FACTIONINFO_RANK, info.second));

				if (info.first->vendorValues.merchantContainer)
				{
					// faction vendor
					AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::FACTIONINFO_MERCHANTFACTION, (info.first->factionFlags & TESFaction::kFlag_Vender) ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

					// faction vendor container ID
					AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::FACTIONINFO_MERCHANTCONTAINERID, info.first->vendorValues.merchantContainer->formID));
				}

				// faction mod
				auto* pModInfo = (*g_dataHandler)->GetLocatedModInfo(info.first);
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));


				AppendInfoToArray(factionArray, root, displayName, FILTER_FLAG_ACTIVE, [&extraInfo](GFxValue& object) {
					object.SetMember("extraInfo", &extraInfo);
				});
			}

			parent.SetMember("extraInfo", &factionArray);
		}
	}

	static void	ResolveEffectData(GFxValue& parent, GFxMovieRoot * root, Actor *  actor, UInt32& dataListSize)
	{
		if (parent.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
		{
			auto * pMaigicTarget = &actor->magicTarget;
			BSSimpleList<ActiveEffect*>	* pEffectList = pMaigicTarget->GetEffectList();

			if (pEffectList != nullptr && pEffectList->count)
			{
				++pEffectList->refCount;
				GFxValue effectArray;
				root->CreateArray(&effectArray); //extraData
				for (size_t i = 0; i < pEffectList->count; ++i)
				{
					auto pEffect = pEffectList->items[i];
					if (pEffect != nullptr)
					{
						auto * pEffectSetting = pEffect->effect->mgef;
						if (pEffectSetting != nullptr)
						{
							++dataListSize;
							auto displayName = GetFormDisplayName(pEffectSetting);

							GFxValue extraInfo;
							root->CreateArray(&extraInfo); //extraData

							// effect name
							AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, displayName));

							// effect ID
							AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pEffectSetting->formID));

							// effect mod
							auto* pModInfo = (*g_dataHandler)->GetLocatedModInfo(pEffectSetting);
							AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));

							// elapsed
							AppendInfoToArray(extraInfo, root, std::format("{}: {:.2f}", Settings::EFFECTINFO_ELAPSED, pEffect->elapsed));

							// duration
							AppendInfoToArray(extraInfo, root, std::format("{}: {:.2f}", Settings::EFFECTINFO_DURATION, pEffect->duration));

							// magnitude
							AppendInfoToArray(extraInfo, root, std::format("{}: {:.2f}", Settings::EFFECTINFO_MAGNITUDE, pEffect->magnitude));

							// inactive
							AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EFFECTINFO_INACTIVE, (pEffect->flags & ActiveEffect::kFlag_Inactive) ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

							// detrimental
							AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EFFECTINFO_DETRIMENTAL, (pEffectSetting->effectType & EffectSetting::kEffectType_Detrimental) ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

							if (pEffectSetting->actorValInfoD8)
							{
								const char * pName = pEffectSetting->actorValInfoD8->GetDisplayName();
								if (!pName || !pName[0])
								{
									pName = pEffectSetting->actorValInfoD8->avName;
								}
								if (pName && pName[0])
								{
									// modified actor value
									AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EFFECTINFO_MODIFIEDAV, pName));
								}
							}

							if (pEffect->item)
							{
								auto spellName = GetFormDisplayName(pEffect->item);
								// spell name
								AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EFFECTINFO_SPELLNAME, spellName));

								// spell ID
								AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EFFECTINFO_SPELLID, pEffect->item->formID));

								// spell type
								AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EFFECTINFO_SPELLTYPE, FORM_TYPES[pEffect->item->formType]));

								// spell mod
								auto* pModInfo = (*g_dataHandler)->GetLocatedModInfo(pEffect->item);
								AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EFFECTINFO_SPELLMOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));
							}


							AppendInfoToArray(effectArray, root, displayName, 
								(!(pEffect->flags & ActiveEffect::kFlag_Inactive)) ? FILTER_FLAG_ACTIVE : FILTER_FLAG_INACTIVE, 
								[&extraInfo](GFxValue& object) {
								object.SetMember("extraInfo", &extraInfo);
							});
						}
					}
				}
				parent.SetMember("extraInfo", &effectArray);
				--pEffectList->refCount;
			}
		}
	}

	static void	ResolveActorValueData(GFxValue& parent, GFxMovieRoot * root, Actor *  actor, UInt32&	dataListSize)
	{
		if (parent.GetType() == GFxValue::kType_Object && actor->formType == kFormType_ACHR && root != nullptr)
		{

			struct AVInfo
			{
				ActorValueInfo		* pAVI;
				float				base;
				float				current;
			};
			std::map<std::string, AVInfo> actorValueDict;

			for (uint32_t i = 0; i < actor->actorValueData.count; ++i)
			{
				auto data = actor->actorValueData[i];
				TESForm * pForm = LookupFormByID(data.avFormID);
				if (pForm != nullptr && pForm->formType == kFormType_AVIF)
				{
					auto * pAVI = static_cast<ActorValueInfo*>(pForm);
					float currentValue = actor->actorValueOwner.GetValue(pAVI);
					if (data.value != 0.0f || currentValue != 0.0f)
					{
						const char * actorValueName = pAVI->GetDisplayName();
						if (!actorValueName || !actorValueName[0]) {
							actorValueName = pAVI->avName;
						}
						if (!actorValueName || !actorValueName[0]) {
							actorValueName = pAVI->GetFullName();
						}
						if (!actorValueName || !actorValueName[0]) {
							continue;
						}
						if (nullptr != actorValueName) {
							actorValueDict.insert({ actorValueName,{ pAVI, data.value, currentValue } });
						}
					}
				}
			}
			if (!actorValueDict.size())
				return;

			GFxValue actorValueArray;
			root->CreateArray(&actorValueArray); //extraData

									   //sort by string.A-Z
			for (const auto& actorValuePair : actorValueDict)
			{
				++dataListSize;
				auto pAVI = actorValuePair.second.pAVI;

				GFxValue extraInfo;
				root->CreateArray(&extraInfo); //extraData

				// actor value name
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, actorValuePair.first));

				// actor value type
				auto isValidIndex = pAVI->type >= 0 && pAVI->type < ACTOR_VALUE_TYPES.size();
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::AVINFO_TYPE, isValidIndex ? ACTOR_VALUE_TYPES[pAVI->type] : Settings::EXTRAINFO_NULL));

				// actor value ID
				AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pAVI->formID));

				// actor value base value
				AppendInfoToArray(extraInfo, root, std::format("{}: {:.2f}", Settings::AVINFO_BASEVALUE, actorValuePair.second.base));

				// actor value current value
				AppendInfoToArray(extraInfo, root, std::format("{}: {:.2f}", Settings::AVINFO_CURRENTVALUE, actorValuePair.second.current));

				// actor value mod
				auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pAVI);
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));

				// actor value data
				AppendInfoToArray(actorValueArray, root, actorValuePair.first, FILTER_FLAG_ACTIVE, [&extraInfo](GFxValue& object) {
					object.SetMember("extraInfo", &extraInfo);
				});
			}

			parent.SetMember("extraInfo", &actorValueArray);
		}
	}

	static void	ResolveInventoryData(GFxValue& parentArray, GFxMovieRoot * root, Actor * actor, TESForm * pBaseForm)
	{
		auto pInvList = actor->inventoryList;
		bool result = false;
		if (pInvList != nullptr)
		{
			BSReadLocker inventoryLocker(&pInvList->inventoryLock);

			for (size_t i = 0; i < pInvList->items.count; ++i)
			{
				BGSInventoryItem & item = pInvList->items[i];
				if (item.form != pBaseForm)	continue;
				result = true;
				if (item.stack != nullptr)
				{
					item.stack->Visit([&](BGSInventoryItem::Stack * stack)->bool
					{
						if (!(stack->flags & BGSInventoryItem::Stack::kFlagEquipped))
							return true;

						ExtraDataList * pExtraDataList = stack->extraData;
						TBO_InstanceData * pExtraInstanceData = nullptr;
						const char * pName = nullptr;
						if (pExtraDataList != nullptr)
						{
							pExtraInstanceData = pExtraDataList->GetExtraInstanceData();
							auto * pExtraName = static_cast<ExtraTextDisplayData*>(pExtraDataList->GetExtraData(kExtraData_TextDisplayData));
							pName = (pExtraName != nullptr) ? pExtraName->GetReferenceName(item.form)->c_str() : nullptr;
						}
						if (!pName)
						{
							TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pBaseForm, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);
							pName = (pFullName != nullptr) ? pFullName->GetFullName() : nullptr;
						}

						// item name
						AppendInfoToArray(parentArray, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, pName));

						// item ID
						AppendInfoToArray(parentArray, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pBaseForm->formID));

						// item type
						AppendInfoToArray(parentArray, root, std::format("{}: {}", Settings::EXTRAINFO_TYPE, FORM_TYPES[pBaseForm->formType]));

						ObjectInstanceData	objInst{ nullptr, nullptr };
						CalcInstanceData(objInst, pBaseForm, pExtraInstanceData);

						switch (pBaseForm->formType)
						{
						case FormType::kFormType_ARMO:
						{
							TESObjectARMO * pArmor = static_cast<TESObjectARMO*>(pBaseForm);
							auto * pArmorInst = (TESObjectARMO::InstanceData*)((objInst.data != nullptr) ? objInst.data : pArmor->GetInstanceData());///V1.10
							BGSInventoryItem::SimpleDataCollector resistInfo;
							item.CollectResistData(stack, resistInfo, 1.0f);
							for (size_t i = 0; i < resistInfo.count; ++i)
							{
								if (resistInfo.data[i].value != 0.0f)
								{
									// item resist
									AppendInfoToArray(parentArray, root, std::format("{}: {:.2f}", Settings::RESISTTYPES[resistInfo.data[i].type], resistInfo.data[i].value));
								}
							}
							BGSKeyword * pKW = GeHealthtKW();
							if (pArmor->keywordForm.keywordBase.HasKeyword(pKW))
							{
								UInt32 fullHealth = pArmor->GetFullHealth(pArmorInst);///V1.10.26
								UInt32 currentHealth = (pExtraDataList != nullptr) ? static_cast<UInt32>(pExtraDataList->GetCurrentHealth(pArmor)) : fullHealth;	 ///V1.10.26		
								// item health
								AppendInfoToArray(parentArray, root, std::format("{}: {}/{}", Settings::EQUIPMENTINFO_HEALTH, currentHealth, fullHealth));
							}
							break;
						}
						case FormType::kFormType_AMMO:
						{
							auto pAmmo = static_cast<TESAmmo*>(pBaseForm);
							BGSKeyword * pKW = GetChargeKW(); //Charge keyword.
							if (pAmmo->keywordForm.keywordBase.HasKeyword(pKW))
							{
								UInt32 currentCharge = pExtraDataList->GetCharge();
								// item charge
								AppendInfoToArray(parentArray, root, std::format("{}: {}/{}", Settings::EQUIPMENTINFO_CHARGE, currentCharge, 100));
							}
							break;
						}
						case FormType::kFormType_WEAP:
						{
							TESObjectWEAP * pWeapon = static_cast<TESObjectWEAP*>(pBaseForm);
							auto * pWeaponInst = (TESObjectWEAP::InstanceData*)((objInst.data != nullptr) ? objInst.data : pWeapon->GetInstanceData()); //V1.10.26

							if (pWeaponInst != nullptr) {
								// weapon type
								AppendInfoToArray(parentArray, root, std::format("{}: {}", Settings::EQUIPMENTINFO_CLASS, (pWeaponInst->weaponType > TESObjectWEAP::InstanceData::kWeaponType_Mine) ? WEAPON_TYPES[WEAPON_TYPES.size() - 1] : WEAPON_TYPES[pWeaponInst->weaponType]));

								TESAmmo * pAmmo = (pWeaponInst != nullptr) ? pWeaponInst->ammo : pWeapon->weapData.ammo;
								if (pAmmo != nullptr)
								{
									bool isJunk = pAmmo->IsJunk();
									// item ammo
									AppendInfoToArray(parentArray, root, std::format("{}: {}", Settings::EQUIPMENTINFO_AMMO, GetStringFromHash(pAmmo->shortDesc)));
								}
								///V1.10
								if (pWeapon->weapData.weaponType > TESObjectWEAP::InstanceData::kWeaponType_TwoHandedAxe)
								{
									float shotSpeed = pWeapon->GetShootSpeed(pWeaponInst);///V1.10.26

									// item speed
									AppendInfoToArray(parentArray, root, std::format("{}: {:.2f}", Settings::EQUIPMENTINFO_SPEED, shotSpeed));

									float range = GetWeaponRange(objInst);

									// item range
									AppendInfoToArray(parentArray, root, std::format("{}: {:.2f}", Settings::EQUIPMENTINFO_RANGE, range));

									float accuracy = GetWeaponAccuracy(objInst, actor);

									// item accuracy
									AppendInfoToArray(parentArray, root, std::format("{}: {:.2f}", Settings::EQUIPMENTINFO_ACCURACY, accuracy));
								}
								else
								{
									UInt32 attackSpeed = pWeapon->GetAttackSpeed();
									// item speed
									AppendInfoToArray(parentArray, root, std::format("{}: {}", Settings::EQUIPMENTINFO_SPEED, attackSpeed));
								}
								BGSInventoryItem::SimpleDataCollector damageInfo;
								item.CollectDamageData(stack, damageInfo);
								for (size_t i = 0; i < damageInfo.count; ++i)
								{
									if (damageInfo.data[i].value != 0.0f)
									{
										// item damage
										AppendInfoToArray(parentArray, root, std::format("{}: {:.2f}", Settings::DAMAGETYPES[damageInfo.data[i].type], damageInfo.data[i].value));
									}
								}
							}
							break;
						}
						}

						if (objInst.data != nullptr)
						{
							objInst.data->DecRefHandle();
							objInst.data = nullptr;
						}

						UInt32 value = pBaseForm->GetValue(pExtraDataList);

						// item value
						AppendInfoToArray(parentArray, root, std::format("{}: {}.00", Settings::EQUIPMENTINFO_VALUE, value));

						float weight = pBaseForm->GetWeight(objInst.data);
						weight = actor->CalcFormWeight(pBaseForm, stack, weight, nullptr);

						// item weight
						AppendInfoToArray(parentArray, root, std::format("{}: {:.2f}", Settings::EQUIPMENTINFO_WEIGHT, weight));

						return false;
					});
				}
			}
		}
		if (!result)
		{
			TESFullName * pFullName = (TESFullName *)Runtime_DynamicCast_Internal(pBaseForm, 0, Runtime_RTTI_TESForm.GetPtr(), Runtime_RTTI_TESFullName.GetPtr(), 0);

			// item name
			AppendInfoToArray(parentArray, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, (pFullName != nullptr && strlen(pFullName->name.c_str())) ? pFullName->GetFullName() : Settings::EXTRAINFO_NULL));

			// item ID
			AppendInfoToArray(parentArray, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pBaseForm->formID));

			// item type
			AppendInfoToArray(parentArray, root, std::format("{}: {}", Settings::EXTRAINFO_TYPE, FORM_TYPES[pBaseForm->formType]));

			float weight = pBaseForm->GetWeight(nullptr);
			// item weight
			AppendInfoToArray(parentArray, root, std::format("{}: {:.2f}", Settings::EQUIPMENTINFO_WEIGHT, weight));
		}
	}

	static void	ResolveEquipmentData(GFxValue& parent, GFxMovieRoot * root, Actor * actor, UInt32& dataListSize)
	{
		if (parent.GetType() != GFxValue::kType_Object || actor->formType != kFormType_ACHR || root == nullptr) {
			return;
		}
		BipedAnim *bipedAnim = actor->biped.get();
		bool isPCFirstPerson = actor->formID == 0x14 && *g_playerCamera && (*g_playerCamera)->cameraState == (*g_playerCamera)->cameraStates[PlayerCamera::kCameraState_FirstPerson];
		if (isPCFirstPerson) {
			bipedAnim = static_cast<PlayerCharacter*>(actor)->playerEquipData.get();
		}
		if (bipedAnim == nullptr) {
			return;
		}
		auto *equipData = reinterpret_cast<ActorEquipData*>(bipedAnim);
		GFxValue equipmentArray;
		root->CreateArray(&equipmentArray);
		for (size_t slotIndex = 0; slotIndex < ActorEquipData::kMaxSlots; ++slotIndex)
		{
			auto * pForm = equipData->slots[slotIndex].item;
			if (pForm == nullptr) {
				continue;
			}
			++dataListSize;
			auto displayName = GetFormDisplayName(pForm);
			

			GFxValue extraInfo;
			root->CreateArray(&extraInfo); //extraData

			if (pForm->formType == FormType::kFormType_ARMA)
			{
				// equipment name
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, displayName));

				// equipment ID
				AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pForm->formID));

				// equipment type
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_TYPE, FORM_TYPES[pForm->formType]));
			}
			else
			{
				ResolveInventoryData(extraInfo, root, actor, pForm);
			}
			// equipment slot index
			AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EQUIPMENTINFO_SLOTINDEX, static_cast<int>(slotIndex)));

			// equipment mod
			auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pForm);
			AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));


			AppendInfoToArray(equipmentArray, root, std::format("{}({})", displayName, FORM_TYPES[pForm->formType]), FILTER_FLAG_ACTIVE, [&extraInfo](GFxValue& object) {
				object.SetMember("extraInfo", &extraInfo);
			});

		}
		if (equipmentArray.GetArraySize())
		{
			parent.SetMember("extraInfo", &equipmentArray);
		}
	}

	virtual void Invoke(Args * args)
	{
		NiPointer<TESObjectREFR> pRef;
		LookupREFRByHandle(*g_consoleHandle, pRef);
		if (pRef != nullptr)
		{
			
			auto *root = args->movie->movieRoot;
			root->CreateArray(args->result);

			auto &parent = *args->result;

			// reference name
			const char * pRefName = pRef->GetReferenceName();
			AppendInfoToArray(parent, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, (pRefName != nullptr && strlen(pRefName)) ? pRefName : Settings::EXTRAINFO_NULL));

			// reference ID
			AppendInfoToArray(parent, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pRef->formID));

			TESForm* pBaseForm = pRef->baseForm;
			BSExtraData* pExtraData = pRef->extraDataList->GetExtraData(kExtraData_LeveledCreature);
			if (pExtraData != nullptr)
			{
				auto * leveledBaseForm = *reinterpret_cast<TESForm**>((uintptr_t)pExtraData + 0x18);
				pBaseForm = (leveledBaseForm != nullptr) ? leveledBaseForm : pRef->baseForm;
			}

			// base form ID
			AppendInfoToArray(parent, root, std::format("{}: {:08X}", Settings::EXTRAINFO_BASEID, pBaseForm->formID));

			auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pBaseForm);
			if (pModInfo != nullptr)
			{
				// base form mod
				AppendInfoToArray(parent, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));
			}

			if (pRef->formType == kFormType_ACHR)
			{
				Actor * actor = static_cast<Actor*>(pRef.get());
				TESNPC * npc = static_cast<TESNPC*>(pBaseForm);
				auto raceName = GetFormDisplayName(actor->race);
				// race name
				AppendInfoToArray(parent, root, std::format("{}: {}", Settings::EXTRAINFO_RACE, raceName));

				// race ID
				AppendInfoToArray(parent, root, std::format("{}: {:08X}", Settings::EXTRAINFO_RACEID, actor->race->formID));

				// sex
				AppendInfoToArray(parent, root, std::format("{}: {}", Settings::EXTRAINFO_SEX, npc->actorData.flags.female ? Settings::EXTRAINFO_FEMALE : Settings::EXTRAINFO_MALE));

				// level
				AppendInfoToArray(parent, root, std::format("{}: {}", Settings::EXTRAINFO_LEVEL, actor->GetLevel()));///V1.10


				if (npc->actorData.flags.pcLevelMult)
				{
					// PC level mult
					AppendInfoToArray(parent, root, std::format("{}: {:.2f}", Settings::EXTRAINFO_PCLVMULT, static_cast<float>(npc->actorData.level) / 1000));

					// min level
					AppendInfoToArray(parent, root, std::format("{}: {}", Settings::EXTRAINFO_MINLEVEL, npc->actorData.minLevel));

					// max level
					AppendInfoToArray(parent, root, std::format("{}: {}", Settings::EXTRAINFO_MAXLEVEL, npc->actorData.maxLevel));
				}

				auto * pAVs = GetActorValueCollector();
				ActorValueInfo*	pHealth = *reinterpret_cast<ActorValueInfo**>(pAVs + 0x1B);
				AppendInfoToArray(parent, root, std::format("HP: {}", static_cast<SInt32>(actor->actorValueOwner.GetValue(pHealth))));

				// action points
				if (TESForm *pForm = LookupFormByID(0x2D5); pForm != nullptr && pForm->formType == kFormType_AVIF)
				{
					auto * pAV = static_cast<ActorValueInfo*>(pForm);
					// action points
					AppendInfoToArray(parent, root, std::format("AP: {}", static_cast<UInt32>(actor->actorValueOwner.GetValue(pAV))));
				}

				// carry weight
				AppendInfoToArray(parent, root, std::format("{}: {:.2f}", Settings::EXTRAINFO_CARRYWEIGHT, pRef->GetCarryWeight()));
			}

			if (pRef->formType == kFormType_ACHR)
			{
				TESNPC * npc = static_cast<TESNPC*>(pBaseForm);

				GFxValue extraInfo;
				root->CreateArray(&extraInfo);

				// is unique
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::FLAGINFO_UNIQUE, npc->actorData.flags.unique ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

				// is essential
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::FLAGINFO_ESSENTIAL, npc->actorData.flags.essential ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

				// is protected
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::FLAGINFO_PROTECTED, npc->actorData.flags.protect ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

				// is invulnerable
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::FLAGINFO_INVULNERABLE, npc->actorData.flags.invulnerable ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

				// flags
				AppendInfoToArray(parent, root, std::format("{} >", Settings::FLAGINFO), FILTER_FLAG_ACTIVE, [&extraInfo](GFxValue& object) {
					object.SetMember("extraInfo", &extraInfo);
				});
			}

			// position
			AppendInfoToArray(parent, root, std::format("{} >", Settings::POSITIONINFO), FILTER_FLAG_ACTIVE, [root, pRef](GFxValue& object) {
				CollectPositionData(object, root, pRef);
			});

			TESObjectCELL * pCell = pRef->parentCell;  //extra
			if (pCell != nullptr)
			{
				GFxValue extraInfo;
				root->CreateArray(&extraInfo);

				// cell name
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, strlen(pCell->fullName.name.c_str()) ? pCell->GetFullName() : Settings::EXTRAINFO_NULL));

				// cell ID
				AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pCell->formID));

				// cell is interior
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::CELLINFO_ISINTERIOR, (pCell->flags_2 & TESObjectCELL::kFlag_IsInterior) ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

				// cell mod
				auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pCell);
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));

				// cell
				AppendInfoToArray(parent, root, std::format("{} >", Settings::CELLINFO), FILTER_FLAG_ACTIVE, [&extraInfo](GFxValue& object) {
					object.SetMember("extraInfo", &extraInfo);
				});
			}

			auto * pLocation = pRef->GetCurrentLocation(); //extra V1.10.20
			if (pLocation != nullptr)
			{
				GFxValue extraInfo;
				root->CreateArray(&extraInfo);

				// location name
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, strlen(pLocation->fullName.name.c_str()) ? pLocation->GetFullName() : Settings::EXTRAINFO_NULL));

				// location ID
				AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pLocation->formID));

				// location mod
				auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pLocation);
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));

				// location
				AppendInfoToArray(parent, root, std::format("{} >", Settings::LOCATIONINFO), FILTER_FLAG_ACTIVE, [&extraInfo](GFxValue& object) {
					object.SetMember("extraInfo", &extraInfo);
				});
			}

			auto * pWorld = pRef->GetWorldspace();  //extra
			if (pWorld != nullptr)
			{
				auto worldName = GetFormDisplayName(pWorld);

				GFxValue extraInfo;
				root->CreateArray(&extraInfo);

				// world name
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_NAME, worldName));

				// world ID
				AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pWorld->formID));

				// world mod
				auto pModInfo = (*g_dataHandler)->GetLocatedModInfo(pWorld);
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));

				// world
				AppendInfoToArray(parent, root, std::format("{} >", Settings::WORLDSPACEINFO), FILTER_FLAG_ACTIVE, [&extraInfo](GFxValue& object) {
					object.SetMember("extraInfo", &extraInfo);
				});
			}

			auto * pZone = pRef->GetEncounterZone();
			if (pZone != nullptr)
			{
				GFxValue extraInfo;
				args->movie->movieRoot->CreateArray(&extraInfo);

				// zone ID
				AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::EXTRAINFO_ID, pZone->formID));

				// zone min level
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::ENCOUNTERZONEINFO_MINLEVEL, pZone->minLevel));

				// zone max level
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::ENCOUNTERZONEINFO_MAXLEVEL, pZone->maxLevel));

				// zone current level
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::ENCOUNTERZONEINFO_CURRENTLEVEL, pZone->curLevel));

				// zone never reset
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::ENCOUNTERZONEINFO_NEVERRESET, (pZone->flags_2 & BGSEncounterZone::kFlag_NeverResets) ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

				// zone disable boundary
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::ENCOUNTERZONEINFO_DISABLEBOUNDARY, (pZone->flags_2 & BGSEncounterZone::kFlag_DisableCombatBoundary) ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

				// zone match PC level
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::ENCOUNTERZONEINFO_MATCHPCLEVEL, (pZone->flags_2 & BGSEncounterZone::kFlag_MatchPCBelowMinimumLevel) ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

				// zone workshop
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::ENCOUNTERZONEINFO_ISWORKSHOP, (pZone->flags_2 & BGSEncounterZone::kFlag_Workshop) ? Settings::EXTRAINFO_TRUE : Settings::EXTRAINFO_FALSE));

				if (pZone->location != nullptr)
				{
					// zone location ID
					AppendInfoToArray(extraInfo, root, std::format("{}: {:08X}", Settings::ENCOUNTERZONEINFO_LOCATIONID, pZone->location->formID));
				}

				// zone mod
				auto* pModInfo = (*g_dataHandler)->GetLocatedModInfo(pZone);
				AppendInfoToArray(extraInfo, root, std::format("{}: {}", Settings::EXTRAINFO_MOD, (pModInfo != nullptr) ? pModInfo->name : Settings::EXTRAINFO_NULL));

				// zone
				AppendInfoToArray(parent, root, std::format("{} >", Settings::ENCOUNTERZONEINFO), FILTER_FLAG_ACTIVE, [&extraInfo](GFxValue& object) {
					object.SetMember("extraInfo", &extraInfo);
				});
			}

			if (pRef->formType == kFormType_ACHR)
			{
				Actor * actor = static_cast<Actor*>(pRef.get());
				GFxValue perks;
				root->CreateObject(&perks);
				UInt32 perkCount = 0;
				ResolvePerkData(perks, root, actor, perkCount);
				RegisterString(&perks, root, "text", std::format("{}({}) >", Settings::PERKINFO, perkCount));
				Register<UInt32>(&perks, "filterFlag", 2);
				args->result->PushBack(&perks);

				GFxValue factions;
				root->CreateObject(&factions);
				UInt32 factionCount = 0;
				ResolveFactionData(factions, root, actor, factionCount);
				RegisterString(&factions, root, "text", std::format("{}({}) >", Settings::FACTIONINFO, factionCount));
				Register<UInt32>(&factions, "filterFlag", 2);
				args->result->PushBack(&factions);

				GFxValue effects;
				root->CreateObject(&effects);
				UInt32 effectCount = 0;
				ResolveEffectData(effects, root, actor, effectCount);
				RegisterString(&effects, root, "text", std::format("{}({}) >", Settings::EFFECTINFO, effectCount));
				Register<UInt32>(&effects, "filterFlag", 2);
				args->result->PushBack(&effects);

				GFxValue actorValues;
				UInt32 valueCount = 0;
				root->CreateObject(&actorValues);
				ResolveActorValueData(actorValues, root, actor, valueCount);
				RegisterString(&actorValues, root, "text", std::format("{}({}) >", Settings::AVINFO, valueCount));
				Register<UInt32>(&actorValues, "filterFlag", 2);
				args->result->PushBack(&actorValues);

				GFxValue equipments;
				UInt32 equipmentCount = 0;
				root->CreateObject(&equipments);
				ResolveEquipmentData(equipments, root, actor, equipmentCount);
				RegisterString(&equipments, root, "text", std::format("{}({}) >", Settings::EQUIPMENTINFO, equipmentCount));
				Register<UInt32>(&equipments, "filterFlag", 2);
				args->result->PushBack(&equipments);
			}

			// memory
			AppendInfoToArray(parent, root, std::format("{}: 0x{:X}", Settings::EXTRAINFO_MEMORY, (uintptr_t)pRef.get()), FILTER_FLAG_ACTIVE);
		}
	}
};
												   //0xF0
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
			IMenu * pConsole = (*g_ui)->GetMenu(console);
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
		if ((*g_ui) != nullptr)
		{
			static auto * pHandler = new MenuOpenCloseHandler();
			RegisterMenuOpenCloseEvent((*g_ui)->menuOpenCloseEventSource, pHandler);
		}
	}
};

class Console : public GameMenuBase
{
public:
	using	FnPickNext = bool(__thiscall Console::*)(void* pData);
	static	FnPickNext	 PickNext_Original;

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
		return (this->*PickNext_Original)(pData);
	}

	static const char * GetReferenceName(TESObjectREFR * pRef)
	{
		return pRef->GetReferenceName();
	}

	static void InitHook()
	{
		PickNext_Original = HookUtil::SafeWrite64(RELOC_RUNTIME_ADDR("48 8D 05 ? ? ? ? BD ? ? ? ? F3 0F 11 44 24 ?", 0, 3, 7) + 8 * 0xF, &PickNext_Hook); //V1.10.26 //48 8D 05 ? ? ? ? BD ? ? ? ? F3 0F 11 44 24 ? //02DAE938
		g_branchTrampoline.Write6Call(RELOC_RUNTIME_ADDR("FF 90 ? ? ? ? 48 89 74 24 ? 4C 89 7C 24 ?"), (uintptr_t)GetReferenceName); //FF 90 ? ? ? ? 48 89 74 24 ? 4C 89 7C 24 ?
	}

	static void InitHook_NG()
	{
		PickNext_Original = HookUtil::SafeWrite64(RELOC_RUNTIME_ADDR("48 8D 05 ? ? ? ? 48 89 06 4C 8D 46 ? 33 C9", 0, 3, 7) + 8 * 0xF, &PickNext_Hook); //V1.10.26 //48 8D 05 ? ? ? ? BD ? ? ? ? F3 0F 11 44 24 ? //02DAE938
		g_branchTrampoline.Write6Call(RELOC_RUNTIME_ADDR("FF 90 ? ? ? ? 48 89 74 24 ? 4C 8D 05 ? ? ? ?"), (uintptr_t)GetReferenceName); //FF 90 ? ? ? ? 48 89 74 24 ? 4C 89 7C 24 ?
	}

	static void InitHook_AE()
	{
		PickNext_Original = HookUtil::SafeWrite64(RELOC_RUNTIME_ADDR("48 8D 05 ? ? ? ? 48 89 06 4C 8D 46 ? 33 C9", 0, 3, 7) + 8 * 0xF, &PickNext_Hook); //V1.10.26 //48 8D 05 ? ? ? ? BD ? ? ? ? F3 0F 11 44 24 ? //02DAE938
		g_branchTrampoline.Write6Call(RELOC_RUNTIME_ADDR("FF 90 ? ? ? ? 48 89 74 24 ? 4C 8D 05 ? ? ? ?"), (uintptr_t)GetReferenceName); //FF 90 ? ? ? ? 48 89 74 24 ? 4C 89 7C 24 ?
	}
};
Console::FnPickNext			Console::PickNext_Original = nullptr;


void F4SEMessageHandler(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == F4SEMessagingInterface::kMessage_GameLoaded)
	{
		MenuOpenCloseHandler::Register();
	}
}

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
			}
		}
	}
	return true;
}


void InitRuntimeData()
{
	RELOC_GLOBAL_VAL(Runtime_RTTI_TESFullName, "4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? 33 FF 33 D2 48 8B D9 89 7C 24 20", 0, 3, 7);
	RELOC_GLOBAL_VAL(Runtime_RTTI_TESForm, "4C 8D 05 ? ? ? ? 33 D2 48 8B C8 89 6C 24 20 E8 ? ? ? ? 48 85 C0 74 11", 0, 3, 7);
	RELOC_GLOBAL_VAL(Runtime_DynamicCast_Internal, "E8 ? ? ? ? 48 8D 54 24 ? 48 8B CF 48 89 43 18", 0, 1, 5);

	RELOC_MEMBER_FN(BSFixedString, ctor, "E8 ? ? ? ? 49 8B CF 48 8B D0 E8 ? ? ? ? 48 8D 8D ? ? ? ? 8B D8", 0, 1, 5);
	RELOC_MEMBER_FN(BSFixedString, Set, "E8 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 83 FB 02", 0, 1, 5);
	RELOC_MEMBER_FN(BSFixedString, Release, "E8 ? ? ? ? 48 8B 0E E8 ? ? ? ? 48 C7 06 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN(BSFixedString, ctor_w, "E8 ? ? ? ? 48 8B 7D F0 48 8D 15 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN(BSFixedString, Set_w, "44 88 44 24 ? 48 89 4C 24 ? 53 55 56");

	RELOC_MEMBER_FN(Heap, Allocate, "E8 ? ? ? ? 48 85 C0 74 08 48 8B C8 E8 ? ? ? ? 49 8B 0E", 0, 1, 5);
	RELOC_MEMBER_FN(Heap, Free, "E8 ? ? ? ? EB 1E 41 8D 46 FF 48 8B D3 83 E0 01", 0, 1, 5);

	RELOC_MEMBER_FN(GFxValue::ObjectInterface, SetMember, "25 ? ? ? ? 4C 8D 4D F0 4C 8D 05 ? ? ? ? 3C 0A 0F 94 C0", 0x19, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, HasMember, "E8 ? ? ? ? 84 C0 74 0D 41 8B 47 08", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, GetMember, "E8 ? ? ? ? 8B 4D 90 81 E1 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, Invoke, "E8 ? ? ? ? 49 8B CC E8 ? ? ? ? 49 8B 54 24 ?", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, PushBack, "40 53 48 83 EC 40 48 8B 41 08 4D 8B C8");
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, GetArraySize, "E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? 48 8B 4B 40", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, AddManaged_Internal, "8B 42 08 25 ? ? ? ? 83 F8 06 74 2E");
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, ReleaseManaged_Internal, "E8 ? ? ? ? 48 8D 15 ? ? ? ? 45 33 C0 49 8B CE 41 C6 86 ? ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, SetMember, "25 ? ? ? ? 4C 8D 4D F0 4C 8D 05 ? ? ? ? 3C 0A 0F 94 C0", 0x19, 1, 5);
	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, AddManaged_Internal, "8B 42 08 25 ? ? ? ? 83 F8 06 74 2E");
	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, ReleaseManaged_Internal, "E8 ? ? ? ? 48 8D 15 ? ? ? ? 45 33 C0 49 8B CE 41 C6 86 ? ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(UI, IsMenuOpen, "E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 48 8B 3D ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraData, "E8 ? ? ? ? 48 8B D8 48 85 C0 75 42 8D 48 28", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraInstanceData, "E8 ? ? ? ? 48 8B D8 48 89 44 24 ? 48 85 C0 75 3D", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetCurrentHealth, "48 89 5C 24 ? 57 48 83 EC 20 48 8B DA 48 8B F9 E8 ? ? ? ? 48 8B D0 48 8B CB");
	RELOC_MEMBER_FN_EX(ExtraDataList, GetCharge, "48 83 EC 28 B2 25 E8 ? ? ? ? 48 85 C0");
	RELOC_MEMBER_FN_EX(ExtraDataList, GetEffectiveMod, "48 83 EC 48 B2 35");
	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraEnchantmentItem, "48 83 EC 28 B2 9B E8 ? ? ? ? 48 85 C0 74 09 48 8B 40 20");

	RELOC_MEMBER_FN_EX(ExtraTextDisplayData, GetReferenceName, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8D 79 18 48 8B D9");

	RELOC_MEMBER_FN(TESObjectREFR, GetReferenceName, "40 53 55 57 41 55 48 81 EC ? ? ? ?");
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetReferenceName, "40 53 55 57 41 55 48 81 EC ? ? ? ?");
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetCurrentLocation, "40 53 48 83 EC 20 83 79 14 14");
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetEncounterZone, "E8 ? ? ? ? 48 85 C0 75 56 49 8B 86 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetWorldspace, "48 83 EC 28 48 8B 81 ? ? ? ? 48 85 C0 75 0E");

	RELOC_MEMBER_FN_EX(TESObjectREFR, GetCarryWeight, "E8 ? ? ? ? 48 8B 45 7F F3 0F 11 05 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetInventoryWeight, "E8 ? ? ? ? F3 0F 58 F0 40 84 F6", 0, 1, 5);

	RELOC_MEMBER_FN_EX(SimpleLock, LockThis, "E8 ? ? ? ? 41 8B 16 48 8D 4C 24 ? 45 33 C0 33 F6", 0, 1, 5);
	RELOC_MEMBER_FN_EX(SimpleLock, ReleaseThis, "E8 ? ? ? ? 8B 45 70 48 8D 8D ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(BSReadWriteLock, LockForRead, "E8 ? ? ? ? 41 8B 16 48 8D 4C 24 ? 45 33 C0 33 F6", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, UnlockRead, "E8 ? ? ? ? 8B 45 70 48 8D 8D ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ActorValueInfo, GetDisplayName, "E8 ? ? ? ? 4C 8B F0 48 85 C0 0F 84 ? ? ? ? 66 39 7E 18", 0, 1, 5);

	RELOC_MEMBER_FN_EX(BGSInventoryItem, CollectDamageData, "48 8B C4 48 89 50 10 55 56 41 54");
	RELOC_MEMBER_FN_EX(BGSInventoryItem, CollectResistData, "48 8B C4 F3 0F 11 58 ? 48 89 48 08 55 41 54");
	RELOC_MEMBER_FN_EX(BGSInventoryItem::SimpleDataCollector, Release, "40 53 48 83 EC 20 48 8B 51 08 48 8B D9 48 85 D2 74 1A");

	RELOC_MEMBER_FN_EX(TESDescription, Get, "E8 ? ? ? ? 48 8B 93 ? ? ? ? C6 44 24 ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESObjectWEAP, GetShootSpeed, "E8 ? ? ? ? 0F 28 F8 48 85 FF 74 19", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectWEAP, GetAttackSpeed, "E8 ? ? ? ? 8B D8 48 85 FF 74 15", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESObjectARMO, GetFullHealth, "48 89 5C 24 ? 57 48 83 EC 30 48 8B F9 48 85 D2 74 1A");

	RELOC_MEMBER_FN_EX(TESAmmo, IsJunk, "48 85 C9 74 15 48 3B 0D ? ? ? ?");

	RELOC_MEMBER_FN_EX(Actor, GetItemCount, "40 53 48 83 EC 20 48 8B DA 48 85 D2 74 44");
	RELOC_MEMBER_FN_EX(Actor, HasPerk, "48 83 EC 28 48 8B 81 ? ? ? ? 48 85 C0 74 16 4C 8B C2");
	RELOC_MEMBER_FN_EX(Actor, GetLevel, "E8 ? ? ? ? F3 0F 2C 15 ? ? ? ? 0F B7 C0", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESForm, GetWeight, "40 53 48 83 EC 30 0F 29 74 24 ? 48 8B D9 48 85 D2 74 1B");
	RELOC_MEMBER_FN_EX(TESForm, GetValue, "E8 ? ? ? ? 40 84 ED 74 04", 0, 1, 5);

	RELOC_GLOBAL_VAL(CalculateCRC32_64, "48 89 5C 24 ? 48 89 7C 24 ? 41 8B C0");
	RELOC_GLOBAL_VAL(CalculateCRC32_32, "48 89 5C 24 ? 44 8B CA 41 8B C0");
	RELOC_GLOBAL_VAL(LookupREFRByHandle, "E8 ? ? ? ? 48 8B 5D 67 48 85 DB 0F 84 ? ? ? ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(LookupFormByID, "E8 ? ? ? ? 48 8B E8 48 85 C0 0F 84 ? ? ? ? 80 78 1A 90", 0, 1, 5);
	RELOC_GLOBAL_VAL(EvaluationConditions, "4C 8B DC 48 81 EC ? ? ? ? 49 89 53 88");

	RELOC_GLOBAL_VAL(CalcMagicEffect, "48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 55 56");
	RELOC_GLOBAL_VAL(GetRestoredHPPercent, "48 8B C4 53 48 83 EC 40 0F 29 70 E8");
	RELOC_GLOBAL_VAL(GetModifiedValueName, "48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B F2 48 8B E9 48 8D 15 ? ? ? ?");
	RELOC_GLOBAL_VAL(GetActorValueCollector, "E8 ? ? ? ? 4C 8B 46 58 48 8D 4E 58 48 8B 90 ? ? ? ? 41 FF 50 08 0F 57 C9", 0, 1, 5);
	RELOC_GLOBAL_VAL(GetChargeKW, "E8 ? ? ? ? 33 FF 45 33 C9 45 33 C0", 0, 1, 5);
	RELOC_GLOBAL_VAL(GeHealthtKW, "E8 ? ? ? ? 4C 8B C3 48 8B D0 48 8B CF FF 56 08 48 8B 74 24 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(GetWeaponRange, "40 53 48 83 EC 20 48 83 39 00 0F 57 C9");
	RELOC_GLOBAL_VAL(GetWeaponAccuracy, "40 53 48 83 EC 30 0F 29 74 24 ? 48 8B D9 E8 ? ? ? ? 48 8B 4B 08");
	RELOC_GLOBAL_VAL(GetWeaponSpeedDesc, "E8 ? ? ? ? 48 8D 8D ? ? ? ? 48 8B D0 E8 ? ? ? ? 48 8D 15 ? ? ? ? 4D 8B CC", 0, 1, 5);
	RELOC_GLOBAL_VAL(GetStringFromHash, "E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B FB", 0, 1, 5);
	RELOC_GLOBAL_VAL(VisitPerks, "E8 ? ? ? ? 48 8B 44 24 ? 48 85 C0 74 09 0F B6 40 08", 0, 1, 5);
	RELOC_GLOBAL_VAL(CalcInstanceData, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 89 11 49 8B F8");
	RELOC_GLOBAL_VAL(RegisterMenuOpenCloseEvent, "E8 ? ? ? ? E9 ? ? ? ? 4D 85 E4 0F 84 ? ? ? ? 4C 8D 0D ? ? ? ?", 0, 1, 5);

	RELOC_GLOBAL_VAL(g_consoleHandle, "8B 05 ? ? ? ? 4D 8D 4A 04", 0, 2, 6);
	RELOC_GLOBAL_VAL(g_dataHandler, "48 8B 05 ? ? ? ? 8B 13", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_scaleformHeap, "48 8B 0D ? ? ? ? 48 8B 01 FF 50 60 48 83 EF 28 48 FF CE 75 BA 48 8B 55 58", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_scaleformManager, "48 8B 0D ? ? ? ? 48 8B 49 18 E8 ? ? ? ? 48 8B 44 24 ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_mainHeap, "48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 55 AF", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_player, "48 8B 05 ? ? ? ? 48 85 C0 74 0C F0 FF 40 28", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_playerCamera, "48 8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 84 C0 75 05 48 83 C4 28", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_ui, "48 8B 3D ? ? ? ? E8 ? ? ? ? 48 8B CF 48 8B D0 E8 ? ? ? ? 84 C0 0F 85 ? ? ? ?", 0, 3, 7);

	RELOC_GLOBAL_VAL(g_menuStackLock, "48 8D 0D ? ? ? ? 4C 89 74 24 ? 41 8B FC", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_menuTableLock, "48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 96 ? ? ? ? 4C 8B 7C 24 ?", 0, 3, 7);

	Console::InitHook();
}

void InitRuntimeData_NG()
{
	RELOC_GLOBAL_VAL(Runtime_RTTI_TESFullName, "4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? 89 7C 24 ? 33 D2 48 8B D9", 0, 3, 7); // 1.10.980
	RELOC_GLOBAL_VAL(Runtime_RTTI_TESForm, "4C 8D 05 ? ? ? ? 89 7C 24 ? 33 D2 48 8B D9", 0, 3, 7); // 1.10.980
	RELOC_GLOBAL_VAL(Runtime_DynamicCast_Internal, "E8 ? ? ? ? 48 8B F8 48 85 C0 0F 84 ? ? ? ? 48 8B 80 ? ? ? ?", 0, 1, 5);// 1.10.980

	RELOC_MEMBER_FN(BSFixedString, ctor, "E8 ? ? ? ? 48 8B 55 ? 48 8D 4D ? 49 8B 5D ? 45 33 C0 49 8B FE", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, Set, "E8 ? ? ? ? E9 ? ? ? ? 33 DB 48 8D 54 24 ? 45 8B C1", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, Release, "E8 ? ? ? ? 48 8B 1E 8B 43 ? C1 E8 ? A8 ? 75 ?", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, ctor_w, "E8 ? ? ? ? 48 8B 7D ? 48 8D 15 ? ? ? ?", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, Set_w, "48 81 EC ? ? ? ? 41 0F B6 F0 4C 8B EA E8 ? ? ? ? 41 0F B7 4D ?", -0x1B); // 1.10.980

	RELOC_MEMBER_FN(Heap, Allocate, "E8 ? ? ? ? 4C 8B D0 48 89 05 ? ? ? ? 48 F7 D8 41 8B C5", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(Heap, Free, "E8 ? ? ? ? 45 89 7D ? 41 8B EF", 0, 1, 5); // 1.10.980

	RELOC_MEMBER_FN(GFxValue::ObjectInterface, SetMember, "4C 8D 4D ? 3C ? 4C 8D 05 ? ? ? ?", 0x14, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, HasMember, "E8 ? ? ? ? 84 C0 74 ? 49 8B CF E8 ? ? ? ? 8B 4B ?", 0, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, GetMember, "E8 ? ? ? ? 84 C0 74 ? 48 8B 55 ? 44 8B CB", 0, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, Invoke, "E8 ? ? ? ? EB ? 8B 07 39 05 ? ? ? ?", 0, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, PushBack, "40 53 48 83 EC ? 48 8B 41 ? 4D 8B C8 48 8B DA 4C 8D 44 24 ? 49 8B D1"); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, GetArraySize, "E8 ? ? ? ? 3B F8 0F 82 ? ? ? ? 4C 8D 9C 24 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, AddManaged_Internal, "E8 ? ? ? ? 48 8D 44 24 ? EB ?", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, ReleaseManaged_Internal, "E8 ? ? ? ? 48 C7 03 ? ? ? ? 8B 46 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, SetMember, "4C 8D 4D ? 3C ? 4C 8D 05 ? ? ? ?", 0x14, 1, 5); // new
	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, AddManaged_Internal, "E8 ? ? ? ? 48 8D 44 24 ? EB ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, ReleaseManaged_Internal, "E8 ? ? ? ? 48 C7 03 ? ? ? ? 8B 46 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(UI, IsMenuOpen, "E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 48 8B 3D ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraData, "E8 ? ? ? ? 48 85 C0 74 ? 4C 8B 70 ? EB ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraInstanceData, "E8 ? ? ? ? 48 89 44 24 ? 48 8B D8 48 85 C0 75 ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetCurrentHealth, "48 8B DA 48 8B F9 E8 ? ? ? ? 48 8B D0 48 8B CB", -0xA);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetCharge, "E8 ? ? ? ? 0F 28 F0 48 8B CD F3 0F 5F 35 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetEffectiveMod, "48 83 EC ? B2 ? 48 C7 44 24 ? ? ? ? ?"); //  sub_1409ED630 item card logic
	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraEnchantmentItem, "E8 ? ? ? ? 48 8B D0 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8B 4D ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ExtraTextDisplayData, GetReferenceName, "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC ? 48 8D 79 ?");

	RELOC_MEMBER_FN(TESObjectREFR, GetReferenceName, "48 8B E9 0F 29 74 24 ? 48 8B 89 ? ? ? ?", -0x12);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetReferenceName, "48 8B E9 0F 29 74 24 ? 48 8B 89 ? ? ? ?", -0x12);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetCurrentLocation, "83 79 ? ? 48 8B D9 75 ? 48 8B 05 ? ? ? ?", -0x6);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetEncounterZone, "E8 ? ? ? ? 48 85 C0 75 ? 49 8B 86 ? ? ? ? 48 85 C0", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetWorldspace, "48 83 EC ? 48 8B 81 ? ? ? ? 48 85 C0 75 ? 48 8B 01");

	RELOC_MEMBER_FN_EX(TESObjectREFR, GetInventoryWeight, "E8 ? ? ? ? 0F 2F C6 41 0F 43 DC", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetCarryWeight, "E8 ? ? ? ? F3 0F 5D C6 48 8B 74 24 ?", 0, 1, 5);

	//DEFINE_MEMBER_FN(GetInventoryWeight, float, 0x004B3B20);
	//DEFINE_MEMBER_FN(GetCarryWeight, float, 0x00BEACF0);

	RELOC_MEMBER_FN_EX(SimpleLock, LockThis, "E8 ? ? ? ? 41 8B 16 48 8D 4C 24 ? 45 33 C0 33 F6", 0, 1, 5);
	RELOC_MEMBER_FN_EX(SimpleLock, ReleaseThis, "E8 ? ? ? ? 41 0F B6 C6 48 83 C4 ? 41 5E 5F C3", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, LockForRead, "E8 ? ? ? ? 41 8B 16 48 8D 4C 24 ? 45 33 C0 33 F6", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, UnlockRead, "E8 ? ? ? ? 41 0F B6 C6 48 83 C4 ? 41 5E 5F C3", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, LockForWrite, "E8 ? ? ? ? 48 8B 85 ? ? ? ? 48 8D 4D ? 48 83 C0 ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, UnlockWrite, "E8 ? ? ? ? 48 8B 44 24 ? 48 8B 7C 24 ? 48 8B 5C 24 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ActorValueInfo, GetDisplayName, "E8 ? ? ? ? 48 8B F0 48 85 C0 0F 84 ? ? ? ? 66 41 39 7E ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(BGSInventoryItem, CollectDamageData, "E8 ? ? ? ? 44 8B 85 ? ? ? ? 41 0F 28 F2", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BGSInventoryItem, CollectResistData, "48 8B C4 F3 0F 11 58 ? 56");
	RELOC_MEMBER_FN_EX(BGSInventoryItem::SimpleDataCollector, Release, "40 53 48 83 EC ? 48 8B 51 ? 48 8B D9 48 85 D2 74 ? 48 8B 09 E8 ? ? ? ?");

	RELOC_MEMBER_FN_EX(TESDescription, Get, "E8 ? ? ? ? 48 8B 93 ? ? ? ? 48 8D 45 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESObjectWEAP, GetShootSpeed, "E8 ? ? ? ? 0F 28 F8 48 85 FF 74 ? 48 85 DB", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectWEAP, GetAttackSpeed, "E8 ? ? ? ? 8B D8 48 85 FF 74 ? 48 8B CF", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESObjectARMO, GetFullHealth, "E8 ? ? ? ? 85 C0 74 ? F3 0F 10 4F ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESAmmo, IsJunk, "E8 ? ? ? ? 84 C0 74 ? 41 8B F6", 0, 1, 5);

	RELOC_MEMBER_FN_EX(Actor, GetItemCount, "40 53 48 83 EC ? 48 8B DA 48 85 D2 74 ? 48 8B 81 ? ? ? ?");
	RELOC_MEMBER_FN_EX(Actor, HasPerk, "E8 ? ? ? ? 48 8B 0D ? ? ? ? 45 33 C0 48 8B D5", 0, 1, 5);
	RELOC_MEMBER_FN_EX(Actor, GetLevel, "E8 ? ? ? ? 0F B7 D0 F3 0F 2C 05 ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESForm, GetWeight, "40 53 48 83 EC ? 0F 29 74 24 ? 48 8B D9 48 85 D2 74 ? 48 8B 02");
	RELOC_MEMBER_FN_EX(TESForm, GetValue, "E8 ? ? ? ? 40 84 ED 74 ? 0F AF 43 ?", 0, 1, 5);

	RELOC_GLOBAL_VAL(CalculateCRC32_64, "48 89 5C 24 ? 48 89 7C 24 ? 41 8B C0 48 8D 3D ? ? ? ?"); // new
	RELOC_GLOBAL_VAL(CalculateCRC32_32, "41 8B C0 44 8B CA 4C 33 C8");
	RELOC_GLOBAL_VAL(LookupREFRByHandle, "E8 ? ? ? ? 48 8B BD ? ? ? ? 48 8B 0B", 0, 1, 5);
	RELOC_GLOBAL_VAL(LookupFormByID, "E8 ? ? ? ? 48 8B F8 48 85 C0 74 ? 80 78 ? ? 75 ? 48 8D 48 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(EvaluationConditions, "48 8B C4 53 57 48 81 EC ? ? ? ? 48 8B 19");

	RELOC_GLOBAL_VAL(CalcMagicEffect, "48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 55 56"); // new
	RELOC_GLOBAL_VAL(GetRestoredHPPercent, "48 8B C4 53 48 83 EC ? 0F 29 70 ? 48 8B DA"); 
	RELOC_GLOBAL_VAL(GetModifiedValueName, "57 41 56 41 57 48 83 EC ? 0F B7 42 ?", -0xF);
	RELOC_GLOBAL_VAL(GetActorValueCollector, "E8 ? ? ? ? 48 8B 5C 24 ? 48 8B 88 ? ? ? ? 48 89 0D ? ? ? ?", 0, 1, 5); // new detail: sub_14020E8F0
	RELOC_GLOBAL_VAL(GetChargeKW, "E8 ? ? ? ? 48 8B D0 48 C7 44 24 ? ? ? ? ? 45 33 C9 45 33 C0", 0, 1, 5);
	RELOC_GLOBAL_VAL(GeHealthtKW, "E8 ? ? ? ? 48 8B D0 49 8D 8C 24 ? ? ? ? 45 33 C0", 0, 1, 5);
	// RELOC_GLOBAL_VAL(ReleaseSimpleColllector, "40 53 48 83 EC ? 48 8B 51 ? 48 8B D9 48 85 D2 74 ? 48 8B 09 E8 ? ? ? ?");

	RELOC_GLOBAL_VAL(GetWeaponRange, "40 53 48 83 EC ? 48 83 39 ? 48 8B D9 0F 57 C9"); 
	RELOC_GLOBAL_VAL(GetWeaponAccuracy, "40 53 48 83 EC ? 0F 29 74 24 ? 48 8B D9 E8 ? ? ? ? 48 8B 4B ?");
	RELOC_GLOBAL_VAL(GetWeaponSpeedDesc, "E8 ? ? ? ? 48 8B D0 48 8D 8D ? ? ? ? E8 ? ? ? ? 4C 8B C0", 0, 1, 5); // this function in main logic.
	RELOC_GLOBAL_VAL(GetStringFromHash, "E8 ? ? ? ? 4C 8B F8 48 85 C0 74 ? 80 38 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(VisitPerks, "E8 ? ? ? ? 48 8B 44 24 ? 48 85 C0 74 ? 0F B6 40 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(CalcInstanceData, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 89 11 49 8B F8");
	RELOC_GLOBAL_VAL(RegisterMenuOpenCloseEvent, "E8 ? ? ? ? B0 ? E9 ? ? ? ? 4D 85 FF", 0, 1, 5);

	RELOC_GLOBAL_VAL(g_consoleHandle, "8B 1D ? ? ? ? 45 8B C7", 0, 2, 6);
	RELOC_GLOBAL_VAL(g_dataHandler, "48 8B 05 ? ? ? ? 8B 13", 0, 3, 7); // new 
	RELOC_GLOBAL_VAL(g_scaleformHeap, "48 8B 0D ? ? ? ? 48 8B 01 FF 50 ? 48 83 EF ? 48 83 EE ? 75 ? 48 8B 55 ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_scaleformManager, "48 8B 0D ? ? ? ? 48 8B 49 ? E8 ? ? ? ? 48 8B 45 ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_mainHeap, "48 8D 0D ? ? ? ? 45 33 C0 49 8B D6 E8 ? ? ? ? 4C 8B D0", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_player, "48 3B 1D ? ? ? ? 75 ? E8 ? ? ? ? 84 C0", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_playerCamera, "48 8B 0D ? ? ? ? 88 05 ? ? ? ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_ui, "48 8B 0D ? ? ? ? 89 1C 2E", 0, 3, 7);

	RELOC_GLOBAL_VAL(g_menuStackLock, "48 8D 0D ? ? ? ? 48 89 45 ? 44 8B 38", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_menuTableLock, "48 8D 0D ? ? ? ? E8 ? ? ? ? 4D 8B 85 ? ? ? ?", 0, 3, 7);

	Console::InitHook_NG();
}



void InitRuntimeData_AE()
{
	RELOC_GLOBAL_VAL(Runtime_RTTI_TESFullName, "4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? 89 7C 24 ? 33 D2 48 8B D9", 0, 3, 7); // 1.10.980
	RELOC_GLOBAL_VAL(Runtime_RTTI_TESForm, "4C 8D 05 ? ? ? ? 89 7C 24 ? 33 D2 48 8B D9", 0, 3, 7); // 1.10.980
	RELOC_GLOBAL_VAL(Runtime_DynamicCast_Internal, "E8 ? ? ? ? 48 8B F8 48 85 C0 0F 84 ? ? ? ? 48 8B 80 ? ? ? ?", 0, 1, 5);// 1.10.980

	RELOC_MEMBER_FN(BSFixedString, ctor, "E8 ? ? ? ? 48 8B 55 ? 48 8D 4D ? 49 8B 5D ? 45 33 C0 49 8B FE", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, Set, "E8 ? ? ? ? E9 ? ? ? ? 33 DB 48 8D 54 24 ? 45 8B C1", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, Release, "E8 ? ? ? ? 48 8B 1E 8B 43 ? C1 E8 ? A8 ? 75 ?", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, ctor_w, "E8 ? ? ? ? 48 8B 7D ? 48 8D 15 ? ? ? ?", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(BSFixedString, Set_w, "48 81 EC ? ? ? ? 41 0F B6 F0 4C 8B EA E8 ? ? ? ? 41 0F B7 4D ?", -0x1B); // 1.10.980

	RELOC_MEMBER_FN(Heap, Allocate, "E8 ? ? ? ? 33 FF 48 8B F0 48 85 C0 74 55", 0, 1, 5); // 1.10.980
	RELOC_MEMBER_FN(Heap, Free, "E8 ? ? ? ? 45 89 7D ? 41 8B EF", 0, 1, 5); // 1.10.980

	RELOC_MEMBER_FN(GFxValue::ObjectInterface, SetMember, "4C 8D 4D ? 3C ? 4C 8D 05 ? ? ? ?", 0x14, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, HasMember, "E8 ? ? ? ? 84 C0 74 ? 49 8B CF E8 ? ? ? ? 8B 4B ?", 0, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, GetMember, "E8 ? ? ? ? 84 C0 74 ? 48 8B 55 ? 44 8B CB", 0, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, Invoke, "E8 ? ? ? ? EB ? 8B 07 39 05 ? ? ? ?", 0, 1, 5); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, PushBack, "40 53 48 83 EC ? 48 8B 41 ? 4D 8B C8 48 8B DA 4C 8D 44 24 ? 49 8B D1"); // new
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, GetArraySize, "E8 ? ? ? ? 3B F8 0F 82 ? ? ? ? 4C 8D 9C 24 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, AddManaged_Internal, "E8 ? ? ? ? 48 8D 44 24 ? EB ?", 0, 1, 5);
	RELOC_MEMBER_FN(GFxValue::ObjectInterface, ReleaseManaged_Internal, "E8 ? ? ? ? 48 C7 03 ? ? ? ? 8B 46 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, SetMember, "4C 8D 4D ? 3C ? 4C 8D 05 ? ? ? ?", 0x14, 1, 5); // new
	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, AddManaged_Internal, "E8 ? ? ? ? 48 8D 44 24 ? EB ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(GFxValue::ObjectInterface, ReleaseManaged_Internal, "E8 ? ? ? ? 48 C7 03 ? ? ? ? 8B 46 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(UI, IsMenuOpen, "E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 48 8B 3D ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraData, "E8 ? ? ? ? 48 85 C0 74 ? 4C 8B 70 ? EB ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraInstanceData, "E8 ? ? ? ? 48 89 44 24 ? 48 8B D8 48 85 C0 75 ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetCurrentHealth, "48 8B DA 48 8B F9 E8 ? ? ? ? 48 8B D0 48 8B CB", -0xA);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetCharge, "E8 ? ? ? ? 0F 28 F0 48 8B CD F3 0F 5F 35 ? ? ? ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(ExtraDataList, GetEffectiveMod, "48 83 EC ? B2 ? 48 C7 44 24 ? ? ? ? ?"); //  sub_1409ED630 item card logic
	RELOC_MEMBER_FN_EX(ExtraDataList, GetExtraEnchantmentItem, "E8 ? ? ? ? 48 8B D0 48 8D 8D ? ? ? ? E8 ? ? ? ? 48 8B 4D ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ExtraTextDisplayData, GetReferenceName, "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC ? 48 8D 79 ?");

	RELOC_MEMBER_FN(TESObjectREFR, GetReferenceName, "48 8B E9 0F 29 74 24 ? 48 8B 89 ? ? ? ?", -0x12);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetReferenceName, "48 8B E9 0F 29 74 24 ? 48 8B 89 ? ? ? ?", -0x12);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetCurrentLocation, "83 79 ? ? 48 8B D9 75 ? 48 8B 05 ? ? ? ?", -0x6);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetEncounterZone, "E8 ? ? ? ? 48 85 C0 75 ? 49 8B 86 ? ? ? ? 48 85 C0", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetWorldspace, "48 83 EC ? 48 8B 81 ? ? ? ? 48 85 C0 75 ? 48 8B 01");

	RELOC_MEMBER_FN_EX(TESObjectREFR, GetInventoryWeight, "E8 ? ? ? ? 0F 2F C6 41 0F 43 DC", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectREFR, GetCarryWeight, "E8 ? ? ? ? F3 0F 5D C6 48 8B 74 24 ?", 0, 1, 5);

	//DEFINE_MEMBER_FN(GetInventoryWeight, float, 0x004B3B20);
	//DEFINE_MEMBER_FN(GetCarryWeight, float, 0x00BEACF0);

	RELOC_MEMBER_FN_EX(SimpleLock, LockThis, "E8 ? ? ? ? 41 8B 16 48 8D 4C 24 ? 45 33 C0 33 F6", 0, 1, 5);
	RELOC_MEMBER_FN_EX(SimpleLock, ReleaseThis, "E8 ? ? ? ? 41 0F B6 C6 48 83 C4 ? 41 5E 5F C3", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, LockForRead, "E8 ? ? ? ? 41 8B 16 48 8D 4C 24 ? 45 33 C0 33 F6", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, UnlockRead, "E8 ? ? ? ? 41 0F B6 C6 48 83 C4 ? 41 5E 5F C3", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, LockForWrite, "E8 ? ? ? ? 48 8B 85 ? ? ? ? 48 8D 4D ? 48 83 C0 ?", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BSReadWriteLock, UnlockWrite, "E8 ? ? ? ? 48 8B 44 24 ? 48 8B 7C 24 ? 48 8B 5C 24 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(ActorValueInfo, GetDisplayName, "E8 ? ? ? ? 48 8B F0 48 85 C0 0F 84 ? ? ? ? 66 41 39 7E ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(BGSInventoryItem, CollectDamageData, "E8 ? ? ? ? 44 8B 85 ? ? ? ? 41 0F 28 F2", 0, 1, 5);
	RELOC_MEMBER_FN_EX(BGSInventoryItem, CollectResistData, "48 8B C4 F3 0F 11 58 ? 56");
	RELOC_MEMBER_FN_EX(BGSInventoryItem::SimpleDataCollector, Release, "40 53 48 83 EC ? 48 8B 51 ? 48 8B D9 48 85 D2 74 ? 48 8B 09 E8 ? ? ? ?");

	RELOC_MEMBER_FN_EX(TESDescription, Get, "E8 ? ? ? ? 48 8B 93 ? ? ? ? 48 8D 45 ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESObjectWEAP, GetShootSpeed, "E8 ? ? ? ? 0F 28 F8 48 85 FF 74 ? 48 85 DB", 0, 1, 5);
	RELOC_MEMBER_FN_EX(TESObjectWEAP, GetAttackSpeed, "E8 ? ? ? ? 8B D8 48 85 FF 74 ? 48 8B CF", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESObjectARMO, GetFullHealth, "E8 ? ? ? ? 85 C0 74 ? F3 0F 10 4F ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESAmmo, IsJunk, "E8 ? ? ? ? 84 C0 74 ? 41 8B F6", 0, 1, 5);

	RELOC_MEMBER_FN_EX(Actor, GetItemCount, "40 53 48 83 EC ? 48 8B DA 48 85 D2 74 ? 48 8B 81 ? ? ? ?");
	RELOC_MEMBER_FN_EX(Actor, HasPerk, "E8 ? ? ? ? 48 8B 0D ? ? ? ? 45 33 C0 48 8B D5", 0, 1, 5);
	RELOC_MEMBER_FN_EX(Actor, GetLevel, "E8 ? ? ? ? 0F B7 D0 F3 0F 2C 05 ? ? ? ?", 0, 1, 5);

	RELOC_MEMBER_FN_EX(TESForm, GetWeight, "40 53 48 83 EC ? 0F 29 74 24 ? 48 8B D9 48 85 D2 74 ? 48 8B 02");
	RELOC_MEMBER_FN_EX(TESForm, GetValue, "E8 ? ? ? ? 40 84 ED 74 ? 0F AF 43 ?", 0, 1, 5);

	RELOC_GLOBAL_VAL(CalculateCRC32_64, "48 89 5C 24 ? 48 89 7C 24 ? 41 8B C0 48 8D 3D ? ? ? ?"); // new
	RELOC_GLOBAL_VAL(CalculateCRC32_32, "41 8B C0 44 8B CA 4C 33 C8");
	RELOC_GLOBAL_VAL(LookupREFRByHandle, "E8 ? ? ? ? 48 8B BD ? ? ? ? 48 8B 0B", 0, 1, 5);
	RELOC_GLOBAL_VAL(LookupFormByID, "E8 ? ? ? ? 48 8B F8 48 85 C0 74 ? 80 78 ? ? 75 ? 48 8D 48 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(EvaluationConditions, "48 8B C4 53 57 48 81 EC ? ? ? ? 48 8B 19");

	RELOC_GLOBAL_VAL(CalcMagicEffect, "48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 55 56"); // new
	RELOC_GLOBAL_VAL(GetRestoredHPPercent, "48 8B C4 53 48 83 EC ? 0F 29 70 ? 48 8B DA"); 
	RELOC_GLOBAL_VAL(GetModifiedValueName, "57 41 56 41 57 48 83 EC ? 0F B7 42 ?", -0xF);
	RELOC_GLOBAL_VAL(GetActorValueCollector, "E8 ? ? ? ? 48 8B 5C 24 ? 48 8B 88 ? ? ? ? 48 89 0D ? ? ? ?", 0, 1, 5); // new detail: sub_14020E8F0
	RELOC_GLOBAL_VAL(GetChargeKW, "E8 ? ? ? ? 48 8B D0 48 C7 44 24 ? ? ? ? ? 45 33 C9 45 33 C0", 0, 1, 5);
	RELOC_GLOBAL_VAL(GeHealthtKW, "E8 ? ? ? ? 48 8B D0 49 8D 8C 24 ? ? ? ? 45 33 C0", 0, 1, 5);
	// RELOC_GLOBAL_VAL(ReleaseSimpleColllector, "40 53 48 83 EC ? 48 8B 51 ? 48 8B D9 48 85 D2 74 ? 48 8B 09 E8 ? ? ? ?");

	RELOC_GLOBAL_VAL(GetWeaponRange, "40 53 48 83 EC ? 48 83 39 ? 48 8B D9 0F 57 C9"); 
	RELOC_GLOBAL_VAL(GetWeaponAccuracy, "40 53 48 83 EC ? 0F 29 74 24 ? 48 8B D9 E8 ? ? ? ? 48 8B 4B ?");
	RELOC_GLOBAL_VAL(GetWeaponSpeedDesc, "E8 ? ? ? ? 48 8B D0 48 8D 8D ? ? ? ? E8 ? ? ? ? 4C 8B C0", 0, 1, 5); // this function in main logic.
	RELOC_GLOBAL_VAL(GetStringFromHash, "E8 ? ? ? ? 4C 8B F8 48 85 C0 74 ? 80 38 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(VisitPerks, "E8 ? ? ? ? 48 8B 44 24 ? 48 85 C0 74 ? 0F B6 40 ?", 0, 1, 5);
	RELOC_GLOBAL_VAL(CalcInstanceData, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 89 11 49 8B F8");
	RELOC_GLOBAL_VAL(RegisterMenuOpenCloseEvent, "E8 ? ? ? ? B0 ? E9 ? ? ? ? 4D 85 FF", 0, 1, 5);

	RELOC_GLOBAL_VAL(g_consoleHandle, "8B 1D ? ? ? ? 45 8B C7", 0, 2, 6);
	RELOC_GLOBAL_VAL(g_dataHandler, "48 8B 05 ? ? ? ? 8B 13", 0, 3, 7); // new 
	RELOC_GLOBAL_VAL(g_scaleformHeap, "48 8B 0D ? ? ? ? 48 8B 01 FF 50 ? 48 83 EF ? 48 83 EE ? 75 ? 48 8B 55 ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_scaleformManager, "48 8B 0D ? ? ? ? 48 8B 49 ? E8 ? ? ? ? 48 8B 45 ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_mainHeap, "48 8D 0D ? ? ? ? E8 ? ? ? ? B0 01 48 83 C4 28", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_player, "48 3B 1D ? ? ? ? 75 ? E8 ? ? ? ? 84 C0", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_playerCamera, "48 8B 0D ? ? ? ? 88 05 ? ? ? ?", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_ui, "48 8B 0D ? ? ? ? 89 1C 2E", 0, 3, 7);

	RELOC_GLOBAL_VAL(g_menuStackLock, "48 8D 0D ? ? ? ? 48 89 45 ? 44 8B 38", 0, 3, 7);
	RELOC_GLOBAL_VAL(g_menuTableLock, "48 8D 0D ? ? ? ? E8 ? ? ? ? 4D 8B 85 ? ? ? ?", 0, 3, 7);

	Console::InitHook_AE();
}