#include <Extender/Shared/CustomRequirements.h>
#include <ScriptHelpers.h>
#include <Lua/Shared/LuaMethodHelpers.h>

BEGIN_SE()

CustomRequirementCallbacks* CustomRequirementDescriptor::GetCallbacks()
{
	LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
	if (lua) {
		return lua->GetCustomRequirementCallbacks().GetOrRegister(RequirementId);
	} else {
		return nullptr;
	}
}


CustomRequirementDescriptor* CustomRequirementRegistry::AssignRequirement(FixedString const& name)
{
	auto id = EnumInfo<stats::RequirementType>::Find(name);
	if (!id) {
		OsiError("Tried to register requirement info for nonexistent requirement type '" << name << "'!");
		return nullptr;
	}

	auto desc = requirements_.insert(std::make_pair((uint32_t)*id, CustomRequirementDescriptor{}));
	desc.first->second.RequirementId = (uint32_t)*id;
	desc.first->second.EnumLabel = name;

	names_.insert(std::make_pair(name, &desc.first->second));
	return &desc.first->second;
}

CustomRequirementDescriptor* CustomRequirementRegistry::RegisterNewRequirement(FixedString const& name)
{
	auto requirementType = EnumInfo<stats::RequirementType>::Find(name);
	if (requirementType) {
		if (*requirementType <= stats::RequirementType::TALENT_MagicCycles) {
			OsiError("Tried to register requirement type '" << name << "' multiple times!");
			return nullptr;
		}

		auto it = requirements_.find((uint32_t)*requirementType);
		assert(it != requirements_.end());
		return &it->second;
	}

	auto id = nextRequirementId_++;

	auto desc = requirements_.insert(std::make_pair(id, CustomRequirementDescriptor{}));
	desc.first->second.RequirementId = id;
	desc.first->second.EnumLabel = name;

	names_.insert(std::make_pair(name, &desc.first->second));
	EnumInfo<stats::RequirementType>::Store->Add(id, name.GetStringOrDefault());

	return &desc.first->second;
}

void CustomRequirementRegistry::Clear()
{
	EnumInfo<stats::RequirementType>::Store->Labels.resize((uint32_t)stats::RequirementType::TALENT_MagicCycles + 1);
	auto& enumInfo = EnumInfo<stats::RequirementType>::Store->Values;
	for (auto const& ty : names_) {
		auto it = enumInfo.find(ty.first);
		if (it != enumInfo.end()) {
			enumInfo.erase(it);
		}
	}

	requirements_.clear();
	names_.clear();
	nextRequirementId_ = (uint32_t)stats::RequirementType::TALENT_MagicCycles + 1;
}

CustomRequirementDescriptor* CustomRequirementRegistry::Get(uint32_t requirementId)
{
	auto it = requirements_.find(requirementId);
	if (it != requirements_.end()) {
		return &it->second;
	}

	return nullptr;
}

CustomRequirementDescriptor* CustomRequirementRegistry::Get(FixedString const& name)
{
	auto it = names_.find(name);
	if (it != names_.end()) {
		return it->second;
	}

	return nullptr;
}


CustomRequirementCallbacks* CustomRequirementCallbackManager::GetOrRegister(uint32_t requirementId)
{
	auto it = requirements_.find(requirementId);
	if (it != requirements_.end()) {
		return &it->second;
	}

	auto desc = requirements_.insert(std::make_pair(requirementId, CustomRequirementCallbacks{}));
	desc.first->second.RequirementId = requirementId;
	return &desc.first->second;
}

void CustomRequirementCallbackManager::Clear()
{
	requirements_.clear();
}

std::optional<bool> CustomRequirementCallbackManager::Evaluate(stats::Character* character, stats::Requirement const& requirement)
{
	auto it = requirements_.find((uint32_t)requirement.RequirementId);
	if (it != requirements_.end() && *it->second.EvaluateCallback) {
		auto ctx = &gExtender->GetCurrentExtensionState()->GetCustomRequirementContext();
		auto result = lua::ProtectedCallFunction<bool>(*it->second.EvaluateCallback, const_cast<stats::Requirement*>(&requirement), ctx);
		if (result) {
			return *result;
		}
	}

	return {};
}

END_SE()
