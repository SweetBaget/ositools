BEGIN_CLS(BaseComponent)
P_RO(EntityObjectHandle)
END_CLS()


BEGIN_CLS(IGameObject)
P_REF(Base)
P_RO(MyGuid)
P_RO(NetID)

P_GETTER(Handle, LuaGetHandle)
// FIXME - re-add after entity proxies are done
// P_GETTER(EntityHandle, LuaGetEntityHandle)
P_GETTER(Translate, LuaGetTranslate)
P_GETTER(Scale, LuaGetScale)
P_GETTER(Velocity, LuaGetVelocity)
P_GETTER(Height, LuaGetHeight)

P_FUN(IsTagged, LuaIsTagged)
P_FUN(HasTag, LuaIsTagged)
P_FUN(GetTags, LuaGetTags)
END_CLS()

BEGIN_CLS(IEoCServerObject)
INHERIT(IGameObject)
P_GETTER(DisplayName, LuaGetDisplayName)
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
