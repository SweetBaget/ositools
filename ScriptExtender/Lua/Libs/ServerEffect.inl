#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(esv)

void Effect::Delete()
{
	auto effectMgr = GetStaticSymbols().GetCurrentServerLevel()->EffectManager;
	auto deleteFunc = GetStaticSymbols().esv__EffectManager__DestroyEffect;
	deleteFunc(effectMgr, Component.Handle.Handle);
}

END_NS()

BEGIN_NS(esv::lua::effect)

ObjectSet<Effect*> GetAllEffects()
{
	return GetStaticSymbols().GetCurrentServerLevel()->EffectManager->ActiveEffects;
}

ObjectSet<ComponentHandle> GetAllEffectHandles()
{
	ObjectSet<ComponentHandle> handles;
	auto effects = GetStaticSymbols().GetCurrentServerLevel()->EffectManager->ActiveEffects;
	for (auto const& effect : effects) {
		handles.push_back(effect->Component.Handle);
	}
	return handles;
}

Effect* GetEffect(ComponentHandle handle)
{
	return GetStaticSymbols().GetCurrentServerLevel()->EffectManager->EffectFactory->Get(handle);
}

Effect* CreateEffect(FixedString const& effectName, ComponentHandle sourceHandle, FixedString const& castBone)
{
	auto effectMgr = GetStaticSymbols().GetCurrentServerLevel()->EffectManager;
	auto createFunc = GetStaticSymbols().esv__EffectManager__CreateEffect;
	return createFunc(effectMgr, effectName, sourceHandle.Handle, castBone);
}

void RegisterEffectLib()
{
	DECLARE_MODULE(Effect, Server)
	BEGIN_MODULE()
	// FIXME - needs by-ref array element serialization
	// MODULE_FUNCTION(GetAllEffects)
	MODULE_FUNCTION(GetAllEffectHandles)
	MODULE_FUNCTION(GetEffect)
	MODULE_FUNCTION(CreateEffect)
	END_MODULE()
}

END_NS()
