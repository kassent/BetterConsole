#pragma once

#include "F4SE/PluginAPI.h"
class GFxMovieView;
class GFxValue;

void F4SEMessageHandler(F4SEMessagingInterface::Message* msg);
bool ScaleformCallback(GFxMovieView * view, GFxValue * value);

void InitRuntimeData();

void InitRuntimeData_NG();

