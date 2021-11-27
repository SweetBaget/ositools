#include <GameDefinitions/PropertyMaps/Events.inl>
#include <GameDefinitions/PropertyMaps/Statuses.inl>
#include <GameDefinitions/PropertyMaps/Character.inl>
#include <GameDefinitions/PropertyMaps/Item.inl>
#include <GameDefinitions/PropertyMaps/Stats.inl>
#include <GameDefinitions/PropertyMaps/Surface.inl>
#include <GameDefinitions/PropertyMaps/RootTemplates.inl>
#include <GameDefinitions/PropertyMaps/Projectile.inl>


BEGIN_CLS(BaseComponent)
P_RO(EntityObjectHandle)
END_CLS()


BEGIN_CLS(IGameObject)
P_REF(Base)
P_RO(MyGuid)
P_RO(NetID)

P_FUN(IsTagged, LuaIsTagged)
P_FUN(HasTag, LuaIsTagged)
P_FUN(GetTags, LuaGetTags)
END_CLS()

BEGIN_CLS(IEoCServerObject)
INHERIT(IGameObject)
P_FUN(GetStatus, LuaGetStatus)
P_FUN(GetStatusByType, LuaGetStatusByType)
P_FUN(GetStatuses, LuaGetStatusIds)
P_FUN(GetStatusObjects, LuaGetStatuses)
END_CLS()

BEGIN_CLS(IEoCClientObject)
INHERIT(IGameObject)
P_FUN(GetStatus, LuaGetStatus)
P_FUN(GetStatusByType, LuaGetStatusByType)
P_FUN(GetStatuses, LuaGetStatusIds)
P_FUN(GetStatusObjects, LuaGetStatuses)
END_CLS()

BEGIN_CLS(stats::HitDamageInfo)
P(Equipment)
P(TotalDamageDone)
P(DamageDealt)
P(DeathType)
P(DamageType)
P(AttackDirection)
P(ArmorAbsorption)
P(LifeSteal)
P_BITMASK(EffectFlags)
P(HitWithWeapon)
P_REF(DamageList)

// EffectFlags is an integer in v55
#if defined(GENERATING_PROPMAP)
pm.AddProperty("EffectFlags",
	[](lua_State* L, LifetimeHolder const& lifetime, stats::HitDamageInfo* hit, std::size_t offset, uint64_t flag) {
		push(L, hit->EffectFlags);
		return true;
	},
	[](lua_State* L, LifetimeHolder const& lifetime, stats::HitDamageInfo* hit, int index, std::size_t offset, uint64_t flag) {
		hit->EffectFlags = (stats::HitFlag)get<uint32_t>(L, index);
		return true;
	}
);
#endif

END_CLS()


BEGIN_CLS(esv::DamageHelpers)
P(SimulateHit)
P(HitType)
P(NoHitRoll)
P(ProcWindWalker)
P(ForceReduceDurability)
P(HighGround)
P(CriticalRoll)
P(HitReason)
P(DamageSourceType)
P(Strength)
END_CLS()


BEGIN_CLS(Trigger)
// P_RO(Handle) // Defunct, use GetObjectHandle() instead
// P_RO(UUID) // Defunct, use GetGuid() instead
P_RO(SyncFlags)
P_RO(Translate)
P_RO(TriggerType)
P_RO(IsGlobal)
P_RO(Level)
END_CLS()


// FIXME - stub!
BEGIN_CLS(esv::AtmosphereTrigger)
END_CLS()


BEGIN_CLS(SoundVolumeTriggerData)
P_RO(AmbientSound)
P_RO(Occlusion)
P_RO(AuxBus1)
P_RO(AuxBus2)
P_RO(AuxBus3)
P_RO(AuxBus4)
END_CLS()


BEGIN_CLS(esv::ASAttack)
P_RO(TargetHandle)
P_RO(TargetPosition)
P_RO(IsFinished)
P_RO(AlwaysHit)
P_RO(TimeRemaining)
P_RO(AnimationFinished)
P_RO(TotalHits)
P_RO(TotalHitOffHand)
P_RO(TotalShoots)
P_RO(TotalShootsOffHand)
P_RO(HitCount)
P_RO(HitCountOffHand)
P_RO(ShootCount)
P_RO(ShootCountOffHand)
P_RO(MainWeaponHandle)
P_RO(OffWeaponHandle)
P_RO(MainHandHitType)
P_RO(OffHandHitType)
P_RO(ProjectileUsesHitObject)
P_RO(ProjectileStartPosition)
P_RO(ProjectileTargetPosition)
P_RO(DamageDurability)
END_CLS()


BEGIN_CLS(esv::ASPrepareSkill)
P_RO(SkillId)
P_RO(PrepareAnimationInit)
P_RO(PrepareAnimationLoop)
P_RO(IsFinished)
P_RO(IsEntered)
END_CLS()


BEGIN_CLS(esv::SkillState)
P_RO(SkillId)
P_RO(CharacterHandle)
P_RO(SourceItemHandle)
P_RO(CanEnter)
P_RO(IsFinished)
P_RO(IgnoreChecks)
P_RO(IsStealthed)
P_RO(PrepareTimerRemaining)
P_RO(ShouldExit)
P_RO(CleanseStatuses)
P_RO(StatusClearChance)
P_RO(CharacterHasSkill)
END_CLS()

// FIXME - placeholders
BEGIN_CLS(eoc::AiGrid)
END_CLS()

// FIXME - placeholders
BEGIN_CLS(UIObject)
P_RO(Flags)
P_RO(Path)
P_RO(IsDragging)
P_RO(ChildUIHandle)
P_RO(ParentUIHandle)
P_RO(Layer)
P_RO(RenderOrder)
P_RO(MovieLayout)
P_RO(FlashSize)
P_RO(FlashMovieSize)
P_RO(SysPanelX)
P_RO(SysPanelY)
P_RO(SysPanelW)
P_RO(SysPanelH)
P_RO(Left)
P_RO(Top)
P_RO(Right)
P_RO(MinWidth)
P_RO(MinHeight)
P_RO(UIScale)
P_RO(CustomScale)
P_RO(AnchorObjectName)
P_RO(AnchorId)
P_RO(AnchorTarget)
P_RO(AnchorPos)
P_RO(AnchorTPos)
P_RO(UIScaling)
P_RO(IsUIMoving)
P_RO(IsDragging2)
P_RO(IsMoving2)
P_RO(RenderDataPrepared)
P_RO(InputFocused)
P_RO(HasAnchorPos)
P_RO(UIObjectHandle)
P_RO(Type)
P_RO(PlayerId)

P_FUN(SetPosition, LuaSetPosition)
P_FUN(Resize, LuaResize)
P_FUN(Show, LuaShow)
P_FUN(Hide, LuaHide)
P_FUN(Invoke, LuaInvoke)
P_FUN(GotoFrame, LuaGotoFrame)
P_FUN(GetValue, GetValue)
P_FUN(SetValue, SetValue)
P_FUN(GetHandle, LuaGetHandle)
P_FUN(GetPlayerHandle, LuaGetPlayerHandle)
P_FUN(GetTypeId, GetTypeId)
P_FUN(GetRoot, LuaGetRoot)
P_FUN(Destroy, LuaDestroy)
P_FUN(ExternalInterfaceCall, LuaExternalInterfaceCall)
P_FUN(CaptureExternalInterfaceCalls, CaptureExternalInterfaceCalls)
P_FUN(CaptureInvokes, CaptureInvokes)
P_FUN(EnableCustomDraw, EnableCustomDraw)
P_FUN(SetCustomIcon, SetCustomIcon)
P_FUN(ClearCustomIcon, ClearCustomIcon)
END_CLS()

BEGIN_CLS(Transform)
// TODO - P(Matrix)
// TODO - P(Rotate) -- no 3x3 mat serialization support yet!
P(Translate)
P(Scale)
END_CLS()

// FIXME - placeholders
BEGIN_CLS(stats::DamagePairList)
P_FUN(GetByType, GetByType)
P_FUN(Add, AddDamage)
P_FUN(Clear, ClearAll)
P_FUN(Multiply, Multiply)
P_FUN(Merge, LuaMerge)
P_FUN(ConvertDamageType, ConvertDamageType)
P_FUN(AggregateSameTypeDamages, AggregateSameTypeDamages)
P_FUN(ToTable, LuaToTable)
END_CLS()

BEGIN_CLS(esv::Trigger)
END_CLS()
