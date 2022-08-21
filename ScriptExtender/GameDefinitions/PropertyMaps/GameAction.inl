BEGIN_CLS(esv::GameAction)
P_RO(NetID)
P_RO(Handle)
P_RO(ActionType)
P(Active)
P(Dirty)
P(ActivateTimer)
END_CLS()


BEGIN_CLS(esv::TornadoAction)
INHERIT(esv::GameAction)
P(SkillId)
P_RO(OwnerHandle)
P(Position)
P(Target)
P(TurnTimer)
P(Finished)
P(IsFromItem)
P(HitRadius)
P_REF(SkillProperties)
P(AnchorList)
P(Interpolation)
P_RO(SurfaceActionHandle)
P(HitCharacterHandles)
P(HitItemHandles)
P(CleanseStatuses)
P(StatusClearChance)
END_CLS()


BEGIN_CLS(esv::StormAction::Strike)
P(Object)
P(Target)
P(Source)
P(SkillId)
END_CLS()


BEGIN_CLS(esv::ProjectileTargetDesc)
P(Target)
P(TargetPosition)
P(TargetPosition2)
END_CLS()


BEGIN_CLS(esv::StormAction)
INHERIT(esv::GameAction)
P_RO(OwnerHandle)
P(Position)
P(LifeTime)
P(SkillId)
P(TurnTimer)
P(StrikeTimer)
P(Finished)
P(IsFromItem)
P_REF(ProjectileSkills)
P_REF(ProjectileTargets)
P_REF(Strikes)
END_CLS()


BEGIN_CLS(esv::RainAction)
INHERIT(esv::GameAction)
P_RO(OwnerHandle)
P(Position)
P(AreaRadius)
P(LifeTime)
P(Duration)
P(FirstTick)
P(SkillId)
P(ConsequencesStartTime)
P(TurnTimer)
P(Finished)
P(IsFromItem)
P(SkillProperties)
END_CLS()


BEGIN_CLS(esv::WallAction)
INHERIT(esv::GameAction)
P(SkillId)
P_RO(OwnerHandle)
P(Target)
P(Source)
P(LifeTime)
P_REF(Walls)
P(TurnTimer)
P(Finished)
P(IsFromItem)
P(NumWallsGrown)
P(TimeSinceLastWall)
P(GrowTimePerWall)
P(GrowTimeout)
P(State)
END_CLS()


BEGIN_CLS(esv::StatusDomeAction)
INHERIT(esv::GameAction)
P_RO(OwnerHandle)
P(Position)
P(LifeTime)
P(SkillId)
P(Finished)
// TODO - P_REF(SkillStatusAura)
END_CLS()


BEGIN_CLS(esv::PathAction)
INHERIT(esv::GameAction)
P(SkillId)
P(Owner)
P(Position)
P(Target)
P(TurnTimer)
P(Speed)
P(HitRadius)
P_REF(SkillProperties)
P_REF(Waypoints)
P_RO(Anchor)
P_RO(PreviousAnchor)
P(Interpolation)
P(Distance)
P_RO(SurfaceAction)
P_REF(HitCharacters)
P_REF(HitItems)
P_RO(Finished)
P_RO(IsFromItem)
END_CLS()


BEGIN_CLS(esv::GameObjectMoveAction)
INHERIT(esv::GameAction)
P_RO(ObjectToMove)
P(DoneMoving)
P_RO(CasterCharacterHandle)
P(BeamEffectName)
END_CLS()
