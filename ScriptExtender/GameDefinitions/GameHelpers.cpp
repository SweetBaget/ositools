#include <stdafx.h>

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Ai.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

namespace dse
{
	StaticSymbols & GetStaticSymbols()
	{
		static auto sSymbols = new StaticSymbols();
		return *sSymbols;
	}

	RuntimeStringHandle::RuntimeStringHandle()
	{
		Handle = GFS.strNullStringHandle;
	}

	void StaticSymbols::CanonicalizePath(STDString & path) const
	{
		if (path.find('\\') != std::string::npos) {
			WARN("Path contains backslashes: \"%s\"; canonical paths should only contain forward slashes.", path.c_str());
		}

		std::replace(path.begin(), path.end(), '\\', '/');
	}

#if defined(OSI_EOCAPP)
	STDString StaticSymbols::ToPath(StringView path, PathRootType root, bool canonicalize) const
	{
		if (PathRoots == nullptr) {
			ERR("LibraryManager::ToPath(): Path root API not available!");
			return "";
		}

		STDString canonicalPath(path);
		if (canonicalize) {
			CanonicalizePath(canonicalPath);
		}

		auto rootPath = PathRoots[(unsigned)root];
		if (rootPath == nullptr) {
			ERR("StaticSymbols::ToPath(): Path roots not initialized!");
			return "";
		}

		STDString absolutePath(*rootPath);
		absolutePath += "/" + canonicalPath;
		return absolutePath;
	}

	FileReaderPin StaticSymbols::MakeFileReader(StringView path, PathRootType root, bool canonicalize) const
	{
		if (PathRoots == nullptr || FileReaderCtor == nullptr) {
			ERR("StaticSymbols::MakeFileReader(): File reader API not available!");
			return FileReaderPin(nullptr);
		}

		auto absolutePath = ToPath(path, root, canonicalize);

		Path lsPath;
		lsPath.Name = absolutePath;

		auto reader = new FileReader();
		FileReaderCtor(reader, &lsPath, 2);
		return FileReaderPin(reader);
	}
#else
	STDString StaticSymbols::ToPath(StringView path, PathRootType root, bool canonicalize) const
	{
		if (GetPrefixForRoot == nullptr) {
			ERR("LibraryManager::ToPath(): Path root API not available!");
			return "";
		}

		STDString canonicalPath(path);
		if (canonicalize) {
			CanonicalizePath(canonicalPath);
		}

		StringView rootPath;
		GetPrefixForRoot(&rootPath, (unsigned)root);

		if (rootPath.empty()) {
			ERR("StaticSymbols::ToPath(): Path roots not initialized!");
			return STDString();
		}

		STDString absolutePath(rootPath);
		absolutePath += "/";
		absolutePath += canonicalPath;
		return absolutePath;
	}

	FileReaderPin StaticSymbols::MakeFileReader(StringView path, PathRootType root, bool canonicalize) const
	{
		if (GetPrefixForRoot == nullptr || FileReaderCtor == nullptr) {
			ERR("StaticSymbols::MakeFileReader(): File reader API not available!");
			return FileReaderPin(nullptr);
		}

		Path lsPath;
		lsPath.Name = ToPath(path, root, canonicalize);

		auto reader = new FileReader();
		FileReaderCtor(reader, &lsPath, 2);
		return FileReaderPin(reader);
	}
#endif

	bool StaticSymbols::FileExists(StringView path, PathRootType root, bool canonicalize) const
	{
		// TODO - implement using proper FS file exists call
		auto reader = MakeFileReader(path, root, canonicalize);
		return reader.IsLoaded();
	}

	void StaticSymbols::DestroyFileReader(FileReader * reader)
	{
		if (FileReaderDtor != nullptr) {
			FileReaderDtor(reader);
		}
	}


	FileReaderPin::~FileReaderPin()
	{
		if (reader_ != nullptr) {
			GetStaticSymbols().DestroyFileReader(reader_);
		}
	}

	STDString FileReaderPin::ToString() const
	{
		if (!IsLoaded()) {
			OsiErrorS("File not loaded!");
			return "";
		}

		STDString contents;
		contents.resize(Size());
		memcpy(contents.data(), Buf(), Size());
		return contents;
	}

	void TextBuffer::Replace(WStringView replacement)
	{
		if (Buf) {
			GameFree(Buf);
		}

		Buf = GameAllocArray<wchar_t>(replacement.size() + 1);
		memcpy(Buf, replacement.data(), sizeof(wchar_t) * (replacement.size() + 1));
		Capacity = replacement.size() + 1;
		Length = replacement.size();
	}

	void eoc::Text::ReplaceParam(int index, WStringView replacement)
	{
		if (index < 1 || index > std::size(Params)) {
			OsiWarnS("Param index out of bounds");
			return;
		}

		auto & param = Params[index - 1];
		if (param.PlaceholderOffset == -1 || param.PlaceholderSize == -1) {
			return;
		}

		STDWString s;
		s.resize((std::size_t)Buf->Length);
		memcpy(s.data(), Buf->Buf, sizeof(wchar_t) * Buf->Length);

		STDWString newS = s.substr(0, param.PlaceholderOffset);
		newS += replacement;
		newS += s.substr(param.PlaceholderOffset + param.PlaceholderSize);
		Buf->Replace(newS);

		for (auto i = 0; i < 8; i++) {
			if (i != index - 1
				&& Params[i].PlaceholderSize != -1
				&& Params[i].PlaceholderOffset > param.PlaceholderOffset) {
				Params[i].PlaceholderOffset = Params[i].PlaceholderOffset - param.PlaceholderSize + (uint32_t)replacement.size();
			}
		}

		param.PlaceholderOffset = -1;
		param.PlaceholderSize = -1;
	}

	Module const * ModManager::FindModByNameGuid(char const * nameGuid) const
	{
		auto modUuidFS = NameGuidToFixedString(nameGuid);
		if (!modUuidFS) {
			return nullptr;
		}

		for (auto const& mod : BaseModule.LoadOrderedModules) {
			if (mod.Info.ModuleUUID == modUuidFS) {
				return &mod;
			}
		}

		return nullptr;
	}


	void esv::ProxyProjectileHit::Destroy(bool b)
	{
		if (WrappedHit) {
			WrappedHit->Destroy(b);
		}

		this->~ProxyProjectileHit();
		GameDelete(this);
	}

	void esv::ProxyProjectileHit::OnHit(glm::vec3 const& position, ObjectHandle const& hitObject, Projectile* projectile)
	{
		if (WrappedHit) {
			DisableCrashReporting _;
			WrappedHit->OnHit(position, hitObject, projectile);
		}

		LuaServerPin lua(ExtensionState::Get());
		if (lua) {
			lua->OnProjectileHit(projectile, hitObject, position);
		}
	}

	void esv::ProxyProjectileHit::Visit(ObjectVisitor* visitor)
	{
		if (WrappedHit) {
			DisableCrashReporting _;
			WrappedHit->Visit(visitor);
		}
	}

	int esv::ProxyProjectileHit::GetTypeId()
	{
		if (WrappedHit) {
			return WrappedHit->GetTypeId();
		} else {
			// Use nonexistent ID to prevent instantiation after reload if our hook
			// somehow got into a savegame.
			return 255;
		}
	}


	void PendingStatuses::Add(esv::Status * status)
	{
		PendingStatus pend { status, false };
		statuses_.insert(std::make_pair(status->StatusHandle, pend));
	}

	void PendingStatuses::Remove(esv::Status * status)
	{
		auto it = statuses_.find(status->StatusHandle);
		if (it != statuses_.end()) {
			statuses_.erase(it);
		} else {
			OsiError("Attempted to remove non-pending status " << std::hex << (int64_t)status->StatusHandle);
		}
	}

	PendingStatus * PendingStatuses::Find(ObjectHandle owner, ObjectHandle handle)
	{
		auto it = statuses_.find(handle);
		if (it != statuses_.end()) {
			auto & status = it->second;
			if (owner == status.Status->TargetHandle) {
				return &status;
			} else {
				OsiError("Attempted to retrieve pending status " << std::hex << (int64_t)status.Status->StatusHandle
					<< " on wrong character!");
				return nullptr;
			}
		} else {
			return nullptr;
		}
	}

	esv::Status * esv::StatusMachine::GetStatus(ObjectHandle handle, bool returnUnapplied) const
	{
		if (returnUnapplied) {
			for (auto statusPtr : Components) {
#pragma message("FIXME - reintroduce typing to factories!")
				auto status = reinterpret_cast<Status*>(statusPtr);
				if (status->StatusHandle == handle) {
					return status;
				}
			}
		} else {
			for (auto status : Statuses) {
				if (status->StatusHandle == handle) {
					return status;
				}
			}
		}

		return nullptr;
	}

	esv::Status* esv::StatusMachine::GetStatus(NetId netId) const
	{
		return (esv::Status * )FindByNetId(netId);
	}

	esv::Status* esv::StatusMachine::GetStatus(FixedString const& statusId) const
	{
		for (auto status : Statuses) {
			if (status->StatusId == statusId) {
				return status;
			}
		}

		return nullptr;
	}

	ecl::Status* ecl::StatusMachine::GetStatus(StatusType type) const
	{
		for (auto status : Statuses) {
			if (status->GetStatusId() == type) {
				return status;
			}
		}

		return nullptr;
	}

	ecl::Status* ecl::StatusMachine::GetStatus(FixedString const& statusId) const
	{
		for (auto status : Statuses) {
			if (status->StatusId == statusId) {
				return status;
			}
		}

		return nullptr;
	}

	ecl::Status* ecl::StatusMachine::GetStatus(NetId netId) const
	{
		return (ecl::Status *) FindByNetId(netId);
	}


	esv::Status * esv::Character::GetStatus(ObjectHandle statusHandle, bool returnPending, bool returnUnapplied) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		auto status = StatusMachine->GetStatus(statusHandle, returnUnapplied);
		if (status != nullptr) {
			return status;
		}

		if (returnPending) {
			ObjectHandle ownerHandle;
			this->GetObjectHandle(ownerHandle);

			auto pendingStatus = ExtensionState::Get().PendingStatuses.Find(ownerHandle, statusHandle);
			if (pendingStatus != nullptr) {
				return pendingStatus->Status;
			}
		}

		return nullptr;
	}

	esv::Status* esv::Character::GetStatus(NetId netId) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		// Pending statuses have no NetID, so we can't check them here
		return StatusMachine->GetStatus(netId);
	}

	esv::Status * esv::Item::GetStatus(ObjectHandle statusHandle, bool returnPending, bool returnUnapplied) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		auto status = StatusMachine->GetStatus(statusHandle, returnUnapplied);
		if (status != nullptr) {
			return status;
		}

		if (returnPending) {
			ObjectHandle ownerHandle;
			this->GetObjectHandle(ownerHandle);

			auto pendingStatus = ExtensionState::Get().PendingStatuses.Find(ownerHandle, statusHandle);
			if (pendingStatus != nullptr) {
				return pendingStatus->Status;
			}
		}

		return nullptr;
	}

	esv::Status* esv::Item::GetStatus(NetId netId) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		return StatusMachine->GetStatus(netId);
	}

	ecl::Status* ecl::Character::GetStatus(ObjectHandle statusHandle) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		return (ecl::Status*)StatusMachine->Get(statusHandle);
	}

	ecl::Status* ecl::Character::GetStatus(NetId netId) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		return StatusMachine->GetStatus(netId);
	}

	void HitDamageInfo::ClearDamage()
	{
		TotalDamage = 0;
		ArmorAbsorption = 0;
		DamageList.Clear();
	}

	void HitDamageInfo::ClearDamage(dse::DamageType damageType)
	{
		for (auto const& dmg : DamageList) {
			if (dmg.DamageType == damageType) {
				TotalDamage -= dmg.Amount;
			}
		}

		DamageList.ClearDamage(damageType);
	}

	void HitDamageInfo::AddDamage(dse::DamageType damageType, int32_t amount)
	{
		TotalDamage += amount;
		DamageList.AddDamage(damageType, amount);
	}

	void HitDamageInfo::CopyFrom(HitDamageInfo const& src)
	{
		Equipment = src.Equipment;
		TotalDamage = src.TotalDamage;
		DamageDealt = src.DamageDealt;
		DeathType = src.DeathType;
		DamageType = src.DamageType;
		AttackDirection = src.AttackDirection;
		ArmorAbsorption = src.ArmorAbsorption;
		LifeSteal = src.LifeSteal;
		EffectFlags = src.EffectFlags;
		HitWithWeapon = src.HitWithWeapon;
		DamageList.CopyFrom(src.DamageList);
	}


	void DamagePairList::ClearDamage(dse::DamageType damageType)
	{
		for (uint32_t i = 0; i < Size; i++) {
			if (Buf[i].DamageType == damageType) {
				Remove(i);
				i--;
			}
		}
	}

	void DamagePairList::AddDamage(DamageType damageType, int32_t amount)
	{
		if (amount == 0) return;

		bool added{ false };
		for (uint32_t i = 0; i < Size; i++) {
			if (Buf[i].DamageType == damageType) {
				auto newAmount = Buf[i].Amount + amount;
				if (newAmount == 0) {
					Remove(i);
				} else {
					Buf[i].Amount = newAmount;
				}

				added = true;
				break;
			}
		}

		if (!added && amount != 0) {
			TDamagePair dmg;
			dmg.DamageType = damageType;
			dmg.Amount = amount;
			if (!SafeAdd(dmg)) {
				OsiErrorS("DamageList capacity exceeded!");
			}
		}
	}


	namespace esv
	{
		char const * const sActionStateNames[] = {
			"None",
			nullptr,
			"Attack",
			"Die",
			"Hit",
			"Idle",
			"Animation",
			"PrepareSkill",
			"UseSkill",
			"PickUp",
			"Drop",
			"MoveItem",
			"KnockedDown",
			"Summoning",
			"Polymorphing",
			"UseItem",
			"CombineItem",
			"TeleportFall",
			"Sheath",
			"Unsheath",
			"Identify",
			"Repair",
			"Lockpick",
			"DisarmTrap",
			"Incapacitated",
			"JumpFlight",
			nullptr
		};


		PropertyMapBase * ActionState::GetPropertyMap()
		{
			switch (GetType()) {
			case ActionStateType::ASAttack:
				return &gASAttackPropertyMap;

			case ActionStateType::ASPrepareSkill:
				return &gASPrepareSkillPropertyMap;

			default:
				return nullptr;
			}
		}

		char const * ActionState::GetTypeName()
		{
			auto type = GetType();
			if (type <= ActionStateType::ASJumpFlight) {
				return sActionStateNames[(unsigned)type];
			} else {
				return nullptr;
			}
		}
	}

	char const * TempStrings::Make(STDString const & str)
	{
		auto s = _strdup(str.c_str());
		pool_.push_back(s);
		return s;
	}

	char const * TempStrings::Make(std::string const & str)
	{
		auto s = _strdup(str.c_str());
		pool_.push_back(s);
		return s;
	}

	TempStrings gTempStrings;


	bool ig::FlashObject::GetValueWorkaround(char const* path, ig::DataType desiredType, InvokeDataValue& value, int arrayIndex)
	{
		switch (desiredType) {
		case ig::DataType::None:
			return GetValueWorkaround(path, ig::DataType::Bool, value, arrayIndex)
				|| GetValueWorkaround(path, ig::DataType::Double, value, arrayIndex)
				|| GetValueWorkaround(path, ig::DataType::String, value, arrayIndex);

		// Bool and Double are implemented correctly in EoCApp
		case ig::DataType::Bool:
		case ig::DataType::Double:
			return GetValue(path, desiredType, value, arrayIndex);

		// Strings returned by the vanilla GetValue() are truncated at 512 bytes; for some use cases (status description etc.)
		// this is too short, so we use a non-static buffer size instead
		case ig::DataType::String:
		{
			ig::IggyValuePath val;
			auto const& sym = GetStaticSymbols();
			if (!sym.IgValuePathMakeNameRef(&val, IggyValue, path)) {
				return false;
			}

			if (arrayIndex >= 0) {
				sym.IgValuePathSetArrayIndex(&val, arrayIndex);
			}

			int resultLength{ 0 };
			// Get length of string
			if (sym.IgValueGetStringUTF8(&val, 0, 0, 0x10000, nullptr, &resultLength)) {
				return false;
			}

			// Fetch string directly to STDString buffer
			value.TypeId = ig::DataType::String;
			value.StringVal.resize(resultLength);
			return sym.IgValueGetStringUTF8(&val, 0, 0, resultLength, value.StringVal.data(), &resultLength) == 0;
		}

		default:
			ERR("Attempted to fetch Flash value with unknown type %d", desiredType);
			return false;
		}
	}

	UIObject* UIObjectManager::GetByType(int typeId) const
	{
		for (auto ui : UIObjects) {
			if (ui->Type == typeId) {
				return ui;
			}
		}

		return nullptr;
	}

	void UIObject::OnFunctionCalled(const char * a1, unsigned int a2, ig::InvokeDataValue * a3)
	{
		return GetStaticSymbols().EoCUI__vftable->OnFunctionCalled(this, a1, a2, a3);
	}

	void UIObject::CustomDrawCallback(void * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->CustomDrawCallback(this, a1);
	}

	void UIObject::Destroy(bool a1)
	{
		return GetStaticSymbols().EoCUI__vftable->Destroy(this, a1);
	}

	void UIObject::SetHandle(ObjectHandle * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->SetHandle(this, a1);
	}

	ObjectHandle * UIObject::GetHandle(ObjectHandle * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->GetHandle(this, a1);
	}

	void UIObject::RequestDelete()
	{
		return GetStaticSymbols().EoCUI__vftable->RequestDelete(this);
	}

	void UIObject::SetOwnerPlayerId(uint64_t a1)
	{
		return GetStaticSymbols().EoCUI__vftable->SetOwnerPlayerId(this, a1);
	}

	void UIObject::SetPos(int * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->SetPos(this, a1);
	}

	void UIObject::KeepWithin(int a1, int a2)
	{
		return GetStaticSymbols().EoCUI__vftable->KeepWithin(this, a1, a2);
	}

	void UIObject::Show()
	{
		return GetStaticSymbols().EoCUI__vftable->Show(this);
	}

	void UIObject::Hide()
	{
		return GetStaticSymbols().EoCUI__vftable->Hide(this);
	}

	const char * UIObject::GetDebugName()
	{
		return GetStaticSymbols().EoCUI__vftable->GetDebugName(this);
	}

	bool UIObject::IsControllerUI()
	{
		return GetStaticSymbols().EoCUI__vftable->IsControllerUI(this);
	}

	void UIObject::Init()
	{
		return GetStaticSymbols().EoCUI__vftable->Init(this);
	}

	void UIObject::InitAPI()
	{
		return GetStaticSymbols().EoCUI__vftable->InitAPI(this);
	}

	void UIObject::Update(float a1)
	{
		return GetStaticSymbols().EoCUI__vftable->Update(this, a1);
	}

	void UIObject::PostUpdate(float a1)
	{
		return GetStaticSymbols().EoCUI__vftable->PostUpdate(this, a1);
	}

	void UIObject::Render(void * a1, void * a2)
	{
		return GetStaticSymbols().EoCUI__vftable->Render(this, a1, a2);
	}

	void UIObject::RegisterInvokeNames()
	{
		return GetStaticSymbols().EoCUI__vftable->RegisterInvokeNames(this);
	}

	void UIObject::Resize()
	{
		return GetStaticSymbols().EoCUI__vftable->Resize(this);
	}

	void * UIObject::OnInputEvent(void * a1, void * a2)
	{
		return GetStaticSymbols().EoCUI__vftable->OnInputEvent(this, a1, a2);
	}

	uint8_t * UIObject::SendEventToFlash(uint8_t * a2, void *a3, unsigned int a4)
	{
		return GetStaticSymbols().EoCUI__vftable->SendEventToFlash(this, a2, a3, a4);
	}

	void * UIObject::OnInputEventText(void * a1, void * a2)
	{
		return GetStaticSymbols().EoCUI__vftable->OnInputEventText(this, a1, a2);
	}

	uint16_t * UIObject::OnUnlinkedInput(uint16_t * a1, uint32_t a2, uint16_t a3)
	{
		return GetStaticSymbols().EoCUI__vftable->OnUnlinkedInput(this, a1, a2, a3);
	}

	void UIObject::SetModifierStates(bool a1, bool a2, bool a3, bool a4)
	{
		return GetStaticSymbols().EoCUI__vftable->SetModifierStates(this, a1, a2, a3, a4);
	}

	bool UIObject::OnAPIPreResetDevice(void * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->OnAPIPreResetDevice(this, a1);
	}

	bool UIObject::OnAPIPostResetDevice(void * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->OnAPIPostResetDevice(this, a1);
	}

	void UIObject::OnControllerModeChanged()
	{
		return GetStaticSymbols().EoCUI__vftable->OnControllerModeChanged(this);
	}

	void UIObject::OnPlayerDisconnect(int a1)
	{
		return GetStaticSymbols().EoCUI__vftable->OnPlayerDisconnect(this, a1);
	}

	void UIObject::ReleaseRenderData()
	{
		return GetStaticSymbols().EoCUI__vftable->ReleaseRenderData(this);
	}

	void UIObject::PrepareRenderData()
	{
		return GetStaticSymbols().EoCUI__vftable->PrepareRenderData(this);
	}

	void UIObject::DoPrepareRenderData()
	{
		return GetStaticSymbols().EoCUI__vftable->DoPrepareRenderData(this);
	}

	void UIObject::Activate()
	{
		return GetStaticSymbols().EoCUI__vftable->Activate(this);
	}

	void UIObject::Deactivate()
	{
		return GetStaticSymbols().EoCUI__vftable->Deactivate(this);
	}

	void UIObject::LoseFocus()
	{
		return GetStaticSymbols().EoCUI__vftable->LoseFocus(this);
	}

	int64_t UIObject::GetBitmapHeight()
	{
		return GetStaticSymbols().EoCUI__vftable->GetBitmapHeight(this);
	}

	int64_t UIObject::GetBitmapWidth()
	{
		return GetStaticSymbols().EoCUI__vftable->GetBitmapWidth(this);
	}

	void * UIObject::GetCharacter()
	{
		return GetStaticSymbols().EoCUI__vftable->GetCharacter(this);
	}

	bool UIObject::SetPlayerHandle(ObjectHandle * handle)
	{
		return GetStaticSymbols().EoCUI__vftable->SetPlayerHandle(this, handle);
	}

	ObjectHandle * UIObject::GetPlayerHandle(ObjectHandle * handle)
	{
		return GetStaticSymbols().EoCUI__vftable->GetPlayerHandle(this, handle);
	}

	bool UIObject::Unknown1()
	{
		return GetStaticSymbols().EoCUI__vftable->Unknown1(this);
	}

	void UIObject::Unknown2()
	{
		return GetStaticSymbols().EoCUI__vftable->Unknown2(this);
	}

	void * UIObject::Unknown3()
	{
		return GetStaticSymbols().EoCUI__vftable->Unknown3(this);
	}

	void UIObject::Unknown4(void * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->Unknown4(this, a1);
	}

	namespace ecl
	{
		EoCUI::EoCUI(dse::Path * path)
		{
			GetStaticSymbols().EoCUI__ctor(this, path);
		}
	}

	int16_t eoc::AiGrid::GetSurfaceIndex(AiGridTile* tile, uint8_t layer) const
	{
		if (tile->SurfaceIndexAndMeta == -1) {
			return -1;
		}

		int16_t surfaceIndex = -1;
		if (tile->SurfaceIndexAndMeta & 1) {
			uint64_t layerMask;
			if (layer == 0) {
				layerMask = AiGridTile::BaseSurfaceLayerMask;
			} else if (layer == 1) {
				layerMask = AiGridTile::CloudSurfaceLayerMask;
			} else {
				OsiError("Requested unknown surface layer!");
				layerMask = 0;
			}

			if (layerMask & tile->AiFlags) {
				surfaceIndex = tile->SurfaceIndexAndMeta >> 1;
			}
		} else {
			auto metadata = Surfaces[tile->SurfaceIndexAndMeta >> 1];
			if (layer == 0) {
				surfaceIndex = metadata->BaseSurfaceIndex;
			} else if (layer == 1) {
				surfaceIndex = metadata->CloudSurfaceIndex;
			} else {
				OsiError("Requested unknown surface layer!");
			}
		}

		return surfaceIndex;
	}

	eoc::AiMetaData* eoc::AiGrid::GetAiMetaData(AiGridTile* tile) const
	{
		if (tile->AiMetaDataIndex == -1) {
			return nullptr;
		}

		return AiMetaData[tile->AiMetaDataIndex];
	}
}
