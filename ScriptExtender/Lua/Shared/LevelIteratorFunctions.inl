template <class Predicate>
void GetCharactersGenericOld(lua_State* L, FixedString const& requestedLevel, Predicate pred)
{
	int index{ 1 };

	lua_newtable(L);
	FixedString levelName = requestedLevel;
	if (!levelName) {
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (level == nullptr) {
			OsiError("No current level!");
			return;
		}

		levelName = level->LevelDesc->LevelName;
	}

	auto& helpers = GetEoCServer()->EntityManager->CharacterConversionHelpers;
	auto characters = helpers.RegisteredCharacters.Find(levelName);
	if (characters == nullptr) {
		OsiError("No characters registered for level: " << levelName);
		return;
	}

	for (auto character : **characters) {
		if (pred(character)) {
			settable(L, index++, character->MyGuid);
		}
	}
}


template <class Predicate>
void GetCharactersGeneric(ObjectSet<Character *>& characters, FixedString const& requestedLevel, Predicate pred)
{
	FixedString levelName = requestedLevel;
	if (!levelName) {
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (level == nullptr) {
			OsiError("No current level!");
			return;
		}

		levelName = level->LevelDesc->LevelName;
	}

	auto& helpers = GetEoCServer()->EntityManager->CharacterConversionHelpers;
	auto levelCharacters = helpers.RegisteredCharacters.Find(levelName);
	if (levelCharacters == nullptr) {
		OsiError("No characters registered for level: " << levelName);
		return;
	}

	for (auto character : **levelCharacters) {
		if (pred(character)) {
			characters.push_back(character);
		}
	}
}

int GetAllCharacters(lua_State* L)
{
	FixedString levelName;
	if (lua_gettop(L) >= 1) {
		levelName = get<FixedString>(L, 1);
	}

	GetCharactersGenericOld(L, levelName, [](esv::Character*) { return true; });
	return 1;
}

int GetCharactersAroundPosition(lua_State* L)
{
	glm::vec3 pos(
		get<float>(L, 1),
		get<float>(L, 2),
		get<float>(L, 3)
	);
	float distance = get<float>(L, 4);

	GetCharactersGenericOld(L, FixedString{}, [pos, distance](esv::Character* c) {
		return abs(glm::length(pos - c->WorldPos)) < distance;
	});
	return 1;
}

template <class Predicate>
void GetItemsGeneric(lua_State* L, FixedString const& requestedLevel, Predicate pred)
{
	int index{ 1 };

	lua_newtable(L);
	FixedString levelName = requestedLevel;
	if (!levelName) {
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (level == nullptr) {
			OsiError("No current level!");
			return;
		}

		levelName = level->LevelDesc->LevelName;
	}

	auto& helpers = GetEoCServer()->EntityManager->ItemConversionHelpers;
	auto items = helpers.RegisteredItems.Find(levelName);
	if (items == nullptr) {
		OsiError("No items registered for level: " << levelName);
		return;
	}

	for (auto item : **items) {
		if (pred(item)) {
			settable(L, index++, item->MyGuid);
		}
	}
}

int GetAllItems(lua_State* L)
{
	FixedString levelName;
	if (lua_gettop(L) >= 1) {
		levelName = get<FixedString>(L, 1);
	}

	GetItemsGeneric(L, levelName, [](esv::Item*) { return true; });
	return 1;
}

int GetItemsAroundPosition(lua_State* L)
{
	glm::vec3 pos(
		get<float>(L, 1),
		get<float>(L, 2),
		get<float>(L, 3)
	);
	float distance = get<float>(L, 4);

	GetItemsGeneric(L, FixedString{}, [pos, distance](esv::Item* c) {
		return abs(glm::length(pos - c->WorldPos)) < distance;
	});
	return 1;
}
