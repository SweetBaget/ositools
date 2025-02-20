BEGIN_CLS(ecl::MultiEffectHandler)
P_RO(Position)
P_RO(TargetObjectHandle)
P_RO(ListenForTextKeysHandle)
P_RO(WeaponBones)
P_REF(TextKeyEffects)
P_REF(Effects)
P_REF(AttachedVisualComponents)
P_REF(Visuals)
P_REF(WeaponAttachments)
P_RO(ListeningOnTextKeys)
END_CLS()


BEGIN_CLS(ecl::MultiEffectHandler::EffectInfo)
P_RO(Effect)
P_REF(BoneNames)
P_BITMASK(Flags)
END_CLS()


BEGIN_CLS(ecl::MultiEffectHandler::MultiEffectVisual)
// FIXME - P_RO(VisualEntityHandle)
P_REF(MultiEffectHandler)
P_REF(OS_FS)
END_CLS()


BEGIN_CLS(ecl::MultiEffectHandler::WeaponAttachmentInfo)
P_RO(EffectName)
P_RO(BoneNames)
P_RO(VisualId)
END_CLS()


BEGIN_CLS(WeaponAnimData)
P(FirstTextEventTime)
P(TextEventTime)
P(TimeDelta)
END_CLS()


BEGIN_CLS(ecl::BeamEffectHandler)
P(SourcePosition)
P(SourceHandle)
P(TargetHandle)
P(SourceHandle2)
P_REF(TextKeyEffectDescs)
P_REF(TextKeyEffects)
P_REF(Effects)
P(ListeningOnTextKeys)
END_CLS()


BEGIN_CLS(ecl::lua::visual::ClientMultiVisual)
INHERIT(ecl::MultiEffectHandler)
P_REF(AttachedVisuals)
P_RO(Handle)
P_FUN(Delete, Delete)
P_FUN(ParseFromStats, ParseFromStats)
P_FUN(AddVisual, AddVisual)
END_CLS()


BEGIN_CLS(ecl::Effect)
INHERIT(Visual)
P_RO(EffectFlags)
P(SoundObjectHandle)
P_REF(CachedWorldTransform)
P(EffectName)
P_REF(ParentEffect)
P(CachedParentFadeOpacity)
P(FreezeFadeOpacity)
P_RO(RefCount)
END_CLS()


BEGIN_CLS(esv::Effect)
INHERIT(BaseComponent)
P_RO(NetID)
P(Loop)
P(ForgetEffect)
P_RO(IsForgotten)
P_RO(IsDeleted)
P_RO(Duration)
P(EffectName)
P(Target)
P(BeamTarget)
P(Position)
P(Rotation)
P(Scale)
P(Bone)
P(BeamTargetBone)
P(BeamTargetPos)
P(DetachBeam)
P_FUN(Delete, Delete)
END_CLS()