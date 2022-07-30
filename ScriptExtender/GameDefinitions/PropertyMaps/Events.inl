BEGIN_CLS(lua::EventBase)
P_RO(Name)
P_RO(CanPreventAction)
P_RO(ActionPrevented)
P_RO(Stopped)
P_FUN(StopPropagation, StopPropagation)
P_FUN(PreventAction, PreventAction)
END_CLS()

BEGIN_CLS(lua::EmptyEvent)
INHERIT(lua::EventBase)
END_CLS()

BEGIN_CLS(lua::DoConsoleCommandEvent)
INHERIT(lua::EventBase)
P_RO(Command)
END_CLS()

BEGIN_CLS(lua::TickEvent)
INHERIT(lua::EventBase)
P_REF(Time)
END_CLS()

BEGIN_CLS(lua::NetMessageEvent)
INHERIT(lua::EventBase)
P_RO(Channel)
P_RO(Payload)
P_RO(UserID)
END_CLS()

BEGIN_CLS(lua::GetHitChanceEvent)
INHERIT(lua::EventBase)
P_REF(Attacker)
P_REF(Target)
P(HitChance)
END_CLS()

BEGIN_CLS(lua::GetSkillDamageEvent)
INHERIT(lua::EventBase)
P_REF(Skill)
P_REF(Attacker)
P(IsFromItem)
P(Stealthed)
P(AttackerPosition)
P(TargetPosition)
P(Level)
P(NoRandomization)
P_REF(DamageList)
P(DeathType)
END_CLS()

BEGIN_CLS(lua::GetSkillAPCostEvent)
INHERIT(lua::EventBase)
P_REF(Skill)
P_REF(Character)
P_REF(AiGrid)
P_RO(Position)
P_RO(Radius)
P(AP)
P(ElementalAffinity)
END_CLS()

BEGIN_CLS(esv::lua::GameStateChangedEvent)
INHERIT(lua::EventBase)
P_RO(FromState)
P_RO(ToState)
END_CLS()

BEGIN_CLS(esv::lua::StatusGetEnterChanceEvent)
INHERIT(lua::EventBase)
P_REF(Status)
P_RO(IsEnterCheck)
P(EnterChance)
END_CLS()

BEGIN_CLS(esv::lua::BeforeStatusApplyEvent)
INHERIT(lua::EventBase)
P_REF(Owner)
P_REF(Status)
P(PreventStatusApply)
END_CLS()

BEGIN_CLS(esv::lua::StatusDeleteEvent)
INHERIT(lua::EventBase)
P_REF(Status)
END_CLS()

BEGIN_CLS(esv::lua::ComputeCharacterHitEvent)
INHERIT(lua::EventBase)
P_REF(Target)
P_REF(Attacker)
P_REF(Weapon)
P_REF(DamageList)
P(HitType)
P(NoHitRoll)
P(ForceReduceDurability)
P_REF(Hit)
P_REF(SkillProperties)
P(AlwaysBackstab)
P(HighGround)
P(CriticalRoll)
P(Handled)
END_CLS()

BEGIN_CLS(esv::lua::StatusHitEnterEvent)
INHERIT(lua::EventBase)
P_REF(Hit)
P_REF(Context)
END_CLS()

BEGIN_CLS(esv::lua::BeforeCharacterApplyDamageEvent)
INHERIT(lua::EventBase)
P_REF(Target)
P_REF(Attacker)
P_REF(Hit)
P(Cause)
P(ImpactDirection)
P_REF(Context)
P(Handled)
END_CLS()

BEGIN_CLS(esv::lua::TreasureItemGeneratedEvent)
INHERIT(lua::EventBase)
P_REF(Item)
P_REF(ResultingItem)
END_CLS()

BEGIN_CLS(esv::lua::BeforeCraftingExecuteCombinationEvent)
INHERIT(lua::EventBase)
P_REF(Character)
P(CraftingStation)
P(CombinationId)
P(Quantity)
P_REF(Items)
P(Processed)
END_CLS()

BEGIN_CLS(esv::lua::AfterCraftingExecuteCombinationEvent)
INHERIT(lua::EventBase)
P_REF(Character)
P(CraftingStation)
P(CombinationId)
P(Quantity)
P(Succeeded)
P_REF(Items)
END_CLS()

BEGIN_CLS(esv::lua::BeforeShootProjectileEvent)
INHERIT(lua::EventBase)
P_REF(Projectile)
END_CLS()

BEGIN_CLS(esv::lua::ShootProjectileEvent)
INHERIT(lua::EventBase)
P_REF(Projectile)
END_CLS()

BEGIN_CLS(esv::lua::ProjectileHitEvent)
INHERIT(lua::EventBase)
P_REF(Projectile)
P_REF(HitObject)
P(Position)
END_CLS()

BEGIN_CLS(esv::lua::GroundHitEvent)
INHERIT(lua::EventBase)
P(Position)
P_REF(Caster)
P_REF(DamageList)
END_CLS()

BEGIN_CLS(esv::lua::OnExecutePropertyDataOnTargetEvent)
INHERIT(lua::EventBase)
P_REF(Property)
P_REF(Attacker)
P_REF(Target)
P(ImpactOrigin)
P(IsFromItem)
P_REF(Skill)
P_REF(Hit)
END_CLS()

BEGIN_CLS(esv::lua::OnExecutePropertyDataOnPositionEvent)
INHERIT(lua::EventBase)
P_REF(Property)
P_REF(Attacker)
P(Position)
P(AreaRadius)
P(IsFromItem)
P_REF(Skill)
P_REF(Hit)
END_CLS()

BEGIN_CLS(esv::lua::AiRequestSortEvent)
INHERIT(lua::EventBase)
P(CharacterHandle)
P_REF(Request)
END_CLS()

BEGIN_CLS(esv::lua::OnPeekAiActionEvent)
INHERIT(lua::EventBase)
P(CharacterHandle)
P_REF(Request)
P(ActionType)
P(IsFinished)
END_CLS()

BEGIN_CLS(ecl::lua::GameStateChangedEvent)
INHERIT(lua::EventBase)
P_RO(FromState)
P_RO(ToState)
END_CLS()

BEGIN_CLS(ecl::lua::UIObjectCreatedEvent)
INHERIT(lua::EventBase)
P_REF(UI)
END_CLS()

BEGIN_CLS(ecl::lua::UICallEvent)
INHERIT(lua::EventBase)
P_REF(UI)
P_RO(Function)
P_RO(When)
P_REF(Args)
END_CLS()

BEGIN_CLS(ecl::lua::SkillGetDescriptionParamEvent)
INHERIT(lua::EventBase)
P_REF(Skill)
P_REF(Character)
P_REF(Params)
P(IsFromItem)
P(Description)
END_CLS()

BEGIN_CLS(ecl::lua::StatusGetDescriptionParamEvent)
INHERIT(lua::EventBase)
P_REF(Status)
P_REF(Owner)
P_REF(StatusSource)
P_REF(Params)
P(Description)
END_CLS()

BEGIN_CLS(ecl::lua::SkillGetPropertyDescriptionEvent)
INHERIT(lua::EventBase)
P_REF(Property)
P(Description)
END_CLS()

BEGIN_CLS(ecl::lua::InputEvent)
INHERIT(lua::EventBase)
P_REF(Event)
END_CLS()

BEGIN_CLS(ecl::lua::RawInputEvent)
INHERIT(lua::EventBase)
P_REF(Input)
END_CLS()
