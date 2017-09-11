#pragma once
#include "F4SE/ScaleformCallbacks.h"
#include "F4SE/ScaleformValue.h"

class BetterConsole_GetBaseData : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args);
};

class BetterConsole_WriteLog : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args);
};

class BetterConsole_GetPerkData : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args);
};

class BetterConsole_GetExtraData : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args);
};