//#include "Console.h"
//#include "GameData.h"
//#include "F4SE/ObScript.h"
//
//
//#include "F4SE_common/SafeWrite.h"
//
//#include <vector>
//
//bool DumpClass_Execute(const ObScriptParam * paramInfo, ObScriptCommand::ScriptData * scriptData, TESObjectREFR * thisObj, void * containingObj, void * scriptObj, void * locals, double& result, UInt32& opcodeOffsetPtr)
//{
//	ObScriptCommand::StringChunk *strChunk = (ObScriptCommand::StringChunk*)scriptData->GetChunk();
//
//	std::string target = strChunk->GetString();
//
//	char * pTerminal = &target[target.size() - 1];
//
//	long iTarget = strtol(&target[0], &pTerminal, 0x10);
//
//	ObScriptCommand::IntegerChunk *intChunk = (ObScriptCommand::IntegerChunk*)strChunk->GetNext();
//	int val = intChunk->GetInteger();
//
//	_MESSAGE("%s   %08X   %d", __FUNCTION__, iTarget, val);
//
//	DumpClass(reinterpret_cast<void*>(iTarget), val);
//
//	return true;
//}
//
//
//
//
//void CollectEffects(Actor* actor)
//{
//	auto pMaigicTarget = &actor->magicTarget;
//	auto fnGetEffectList = (BSSimpleList<ActiveEffect*> * (**)(MagicTarget*))(*(uintptr_t*)pMaigicTarget + 0x38);
//	BSSimpleList<ActiveEffect*>	* pEffectList = (*fnGetEffectList)(pMaigicTarget);
//
//	if (pEffectList != nullptr)
//	{
//		++pEffectList->refCount;
//		_MESSAGE("");
//		for (size_t i = 0; i < pEffectList->count; ++i)
//		{
//			_MESSAGE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> DumpIndex: %d", i);
//			auto pEffect = pEffectList->items[i];
//			if (pEffect != nullptr)
//			{
//				_MESSAGE("className: %s VTBL: %016I64X", GetObjectClassName(pEffect), *(uintptr_t*)pEffect);
//				//_MESSAGE(" %.2f | %d | %08X      %.2f | %d | %08X      %.2f | %d | %08X      %.2f | %d | %08X", pEffect->effect->magnitude, pEffect->effect->magnitude, pEffect->effect->magnitude, pEffect->effect->area, pEffect->effect->area, pEffect->effect->area, pEffect->effect->duration, pEffect->effect->duration, pEffect->effect->duration, pEffect->effect->unk0C, pEffect->effect->unk0C, pEffect->effect->unk0C);		
//				//_MESSAGE(" %.2f | %d | %08X      %.2f | %d | %08X      %.2f | %d | %08X      %.2f | %d | %08X", pEffect->elapsed, pEffect->elapsed, pEffect->elapsed, pEffect->duration, pEffect->duration, pEffect->duration, pEffect->magnitude, pEffect->magnitude, pEffect->magnitude, pEffect->flags, pEffect->flags, pEffect->flags);
//				RelocAddr<EffectSetting*(*)(void*)> fnGetEffectSetting = 0xC351A0;
//				auto pEffectSetting = fnGetEffectSetting(pEffect);
//				if (pEffectSetting != nullptr)
//				{
//					RelocAddr<bool(*)(UInt32&, TESObjectREFR*&)> fnLookUpRefByHandle = 0x23870;
//					_MESSAGE("MagicEffect:");
//					_MESSAGE("duratin:		%.2f", pEffect->duration);
//					_MESSAGE("elapsed:		%.2f", pEffect->elapsed);
//					_MESSAGE("magnitude:	%.2f", pEffect->magnitude);
//					_MESSAGE("flags:		%08X", pEffect->flags);
//					if (pEffect->item)
//					{
//						TESFullName* pFullName = DYNAMIC_CAST(pEffect->item, MagicItem, TESFullName);
//						if (pFullName != nullptr)
//						{
//							_MESSAGE("spellID:		%08X", pEffect->item->formID);
//							_MESSAGE("spellName:	%s", pFullName->name.c_str());
//						}
//					}
//					TESFullName* pFullName = DYNAMIC_CAST(pEffectSetting, EffectSetting, TESFullName);
//					if (pFullName != nullptr)
//					{
//						_MESSAGE("EffectSetting:");
//						_MESSAGE("mgefName:		%s", pFullName->name.c_str());
//						_MESSAGE("effectFlag:	%08X", pEffectSetting->flags);
//						_MESSAGE("formID:		%08X", pEffectSetting->formID);
//					}
//				}
//				_MESSAGE("");
//			}
//		}
//		--pEffectList->refCount;
//	}
//}
//
//bool DumpConsoleTarget_Execute(const ObScriptParam * paramInfo, ObScriptCommand::ScriptData * scriptData, TESObjectREFR * thisObj, void * containingObj, void * scriptObj, void * locals, double& result, UInt32& opcodeOffsetPtr)
//{
//	TESObjectREFR* pRef = nullptr;
//	LookupREFRByHandle(g_consoleHandle, &pRef);
//	if (pRef != nullptr)
//	{
//		Console_Print(" ");
//		Console_Print("> refFormID:			%08X", pRef->formID);
//		Console_Print("> refFormType:		%d", pRef->formType);
//		Console_Print("> refAddress:		%016I64X", (uintptr_t)pRef);
//		Console_Print("> fullName:			%s", CALL_MEMBER_FN(pRef, GetReferenceName)());
//		Console_Print("> refEditorID:		%s", pRef->GetEditorID());
//	}
//
//	return true;
//}
//
//
//void RegisterCommands()
//{
//	RelocAddr<ObScriptCommand*(*)(uint32_t)> fnDumpScriptCommands = 0x4EB660;
//	for (uint32_t i = 0; i < 0x1333; ++i)
//	{
//		ObScriptCommand* pCommand = fnDumpScriptCommands(i);
//
//		if (pCommand != nullptr && pCommand->longName && pCommand->longName[0] && pCommand->opcode)
//		{
//			_MESSAGE("");
//			_MESSAGE("longName : %s", pCommand->longName);
//			_MESSAGE("shortName: %s", pCommand->shortName);
//			_MESSAGE("opCode:    %d", pCommand->opcode);
//			_MESSAGE("command:   %08I64X", (uintptr_t)pCommand - (uintptr_t)GetModuleHandle(NULL));
//		}
//
//		if (pCommand != nullptr && pCommand->longName && !strcmp(pCommand->longName, "CellInfo"))
//		{
//			static ObScriptParam params[] = {
//				{ "String", ObScriptParam::kType_String, 0 },
//				{ "Integer", ObScriptParam::kType_Integer, 0 }
//			};
//
//			ObScriptCommand cmd = *pCommand;
//
//			cmd.longName = "DumpClass";
//			cmd.shortName = "dc";
//			cmd.helpText = "";
//			cmd.needsParent = 0;
//			cmd.numParams = 2;
//			cmd.params = params;
//			cmd.execute = (ObScript_Execute)DumpClass_Execute;
//			cmd.flags = 0;
//
//			SafeWriteBuf((uintptr_t)pCommand, &cmd, sizeof(cmd));
//		}
//
//		if (pCommand != nullptr && pCommand->longName && !strcmp(pCommand->longName, "SetGamma"))
//		{
//			ObScriptCommand cmd = *pCommand;
//
//			cmd.longName = "DumpConsoleClass";
//			cmd.shortName = "dcc";
//			cmd.helpText = "";
//			cmd.needsParent = 0;
//			cmd.numParams = 0;
//			cmd.params = nullptr;
//			cmd.execute = (ObScript_Execute)DumpConsoleTarget_Execute;
//			cmd.flags = 0;
//
//			SafeWriteBuf((uintptr_t)pCommand, &cmd, sizeof(cmd));
//		}
//	}
//
//
//}