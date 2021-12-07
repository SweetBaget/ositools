#pragma once
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(esv::lua::surface)

using namespace dse::lua::surface;

SurfaceAction* CreateSurfaceAction(lua_State* L)
{
	auto type = get<SurfaceActionType>(L, 1);

	auto const& sym = GetStaticSymbols();
	if (!sym.esv__SurfaceActionFactory || !*sym.esv__SurfaceActionFactory || !sym.esv__SurfaceActionFactory__CreateAction) {
		OsiError("esv::SurfaceActionFactory not mapped!");
		return 0;
	}

	auto action = sym.esv__SurfaceActionFactory__CreateAction(*sym.esv__SurfaceActionFactory, type, 0);
	if (!action) {
		OsiError("Couldn't create surface action for some reason.");
		return 0;
	}

	return action;
}

void ExecuteSurfaceAction(ProxyParam<SurfaceAction> action)
{
	if (action->Level) {
		OsiError("Attempted to execute surface action more than once!");
		return;
	}

	auto level = GetStaticSymbols().GetCurrentServerLevel();
	if (!level) {
		OsiError("Attempted to execute surface action while no level is loaded!");
		return;
	}

	action->Level = level;

	if (action->VMT->GetTypeId(action) == SurfaceActionType::TransformSurfaceAction) {
		auto transform = reinterpret_cast<TransformSurfaceAction*>(action.Object);
		GetStaticSymbols().esv__TransformSurfaceAction__Init(
			transform, transform->SurfaceTransformAction, transform->SurfaceLayer, transform->OriginSurface
		);
	}

	action->VMT->Enter(action);
	level->SurfaceManager->SurfaceActions.Add(action);
}

void CancelSurfaceAction(ComponentHandle actionHandle)
{
	if (actionHandle.GetType() != (uint32_t)ObjectType::ServerSurfaceAction) {
		OsiError("Expected a surface action handle, got type " << actionHandle.GetType());
		return;
	}

	auto factory = GetStaticSymbols().esv__SurfaceActionFactory;
	if (!factory || !*factory) {
		OsiError("SurfaceActionFactory not mapped!");
		return;
	}

	auto action = (*factory)->Get(actionHandle);
	if (!action) {
		OsiWarn("No surface action found with handle " << std::hex << actionHandle.Handle << "; maybe it already expired?");
		return;
	}

	switch (action->VMT->GetTypeId(action)) {
	case SurfaceActionType::CreateSurfaceAction:
	{
		auto act = static_cast<esv::CreateSurfaceAction*>(action);
		act->CurrentCellCount = act->SurfaceCells.Size;
		break;
	}
	case SurfaceActionType::CreatePuddleAction:
	{
		auto act = static_cast<esv::CreatePuddleAction*>(action);
		act->IsFinished = true;
		break;
	}
	case SurfaceActionType::ExtinguishFireAction:
	{
		auto act = static_cast<esv::ExtinguishFireAction*>(action);
		act->Percentage = 0.0f;
		break;
	}
	case SurfaceActionType::ZoneAction:
	{
		auto act = static_cast<esv::ZoneAction*>(action);
		act->CurrentCellCount = act->SurfaceCells.Size;
		break;
	}
	case SurfaceActionType::ChangeSurfaceOnPathAction:
	{
		auto act = static_cast<esv::ChangeSurfaceOnPathAction*>(action);
		act->IsFinished = true;
		break;
	}
	case SurfaceActionType::RectangleSurfaceAction:
	{
		auto act = static_cast<esv::RectangleSurfaceAction*>(action);
		act->CurrentCellCount = act->SurfaceCells.Size;
		break;
	}
	case SurfaceActionType::PolygonSurfaceAction:
	{
		auto act = static_cast<esv::PolygonSurfaceAction*>(action);
		act->LastSurfaceCellCount = act->SurfaceCells.Size;
		break;
	}
	case SurfaceActionType::SwapSurfaceAction:
	{
		auto act = static_cast<esv::SwapSurfaceAction*>(action);
		act->CurrentCellCount = act->SurfaceCells.Size;
		break;
	}
	case SurfaceActionType::TransformSurfaceAction:
	{
		auto act = static_cast<esv::TransformSurfaceAction*>(action);
		act->Finished = true;
		break;
	}
	default:
		OsiError("CancelSurfaceAction() not implemented for this surface action type!");
	}
}

void RegisterSurfaceActionLib(lua_State* L)
{
	static const luaL_Reg lib[] = {
		{"Create", LuaWrapFunction(&CreateSurfaceAction)},
		// FIXME - move to SurfaceAction::Execute()
		{"Execute", LuaWrapFunction(&ExecuteSurfaceAction)},
		// FIXME - move to SurfaceAction::Cancel() ?
		// need to add GetAction first to resolve by handle
		{"Cancel", LuaWrapFunction(&CancelSurfaceAction)},
		{0,0}
	};

	RegisterLib(L, "Surface", "Action", lib);
}

END_NS()
