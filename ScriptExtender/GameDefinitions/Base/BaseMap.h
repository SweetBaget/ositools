#pragma once

#include <GameDefinitions/Base/BaseTypes.h>
#include <GameDefinitions/Base/BaseMemory.h>
#include <span>

BEGIN_SE()

unsigned int GetNearestLowerPrime(unsigned int num);

template <class TKey, class TValue>
struct MapInternals
{
	using KeyType = TKey;
	using ValueType = TValue;

	struct Node
	{
		Node* Next{ nullptr };
		TKey Key;
		TValue Value;

		Node() {}

		Node(TKey const& key, TValue const& value)
			: Key(key), Value(value)
		{}

		Node(TKey const& key, TValue && value)
			: Key(key), Value(std::move(value))
		{}
	};

	uint32_t HashSize{ 0 };
	Node** HashTable{ nullptr };
	uint32_t ItemCount{ 0 };
};

template <class TKey, class TValue>
struct RefMapInternals
{
	using KeyType = TKey;
	using ValueType = TValue;

	struct Node
	{
		Node* Next{ nullptr };
		TKey Key;
		TValue Value;

		Node() {}

		Node(TKey const& key, TValue const& value)
			: Key(key), Value(value)
		{}

		Node(TKey const& key, TValue&& value)
			: Key(key), Value(std::move(value))
		{}
	};

	uint32_t ItemCount{ 0 };
	uint32_t HashSize{ 0 };
	Node** HashTable{ nullptr };
};

template <class TInternals>
class MapBase : private TInternals
{
public:
	using KeyType = TInternals::KeyType;
	using ValueType = TInternals::ValueType;
	using Node = TInternals::Node;

	class Iterator
	{
	public:
		Iterator(MapBase& map)
			: CurrentNode(map.HashTable), NodeListEnd(map.HashTable + map.HashSize), Element(nullptr)
		{
			while (CurrentNode < NodeListEnd && *CurrentNode == nullptr) {
				CurrentNode++;
			}

			if (CurrentNode < NodeListEnd && *CurrentNode) {
				Element = *CurrentNode;
			}
		}
			
		Iterator(MapBase& map, Node** node, Node* element)
			: CurrentNode(node), NodeListEnd(map.HashTable + map.HashSize), Element(element)
		{}

		Iterator operator ++ ()
		{
			Iterator it(*this);

			Element = Element->Next;
			if (Element == nullptr) {
				do {
					CurrentNode++;
				} while (CurrentNode < NodeListEnd && *CurrentNode == nullptr);

				if (CurrentNode < NodeListEnd && *CurrentNode) {
					Element = *CurrentNode;
				}
			}

			return it;
		}

		Iterator& operator ++ (int)
		{
			Element = Element->Next;
			if (Element == nullptr) {
				do {
					CurrentNode++;
				} while (CurrentNode < NodeListEnd && *CurrentNode == nullptr);

				if (CurrentNode < NodeListEnd && *CurrentNode) {
					Element = *CurrentNode;
				}
			}

			return *this;
		}

		bool operator == (Iterator const& it)
		{
			return it.CurrentNode == CurrentNode && it.Element == Element;
		}

		bool operator != (Iterator const& it)
		{
			return it.CurrentNode != CurrentNode || it.Element != Element;
		}

		KeyType & Key () const
		{
			return Element->Key;
		}

		ValueType& Value () const
		{
			return Element->Value;
		}

		Node& operator * () const
		{
			return *Element;
		}

		Node& operator -> () const
		{
			return *Element;
		}

		operator bool() const
		{
			return Element != nullptr;
		}

		bool operator !() const
		{
			return Element == nullptr;
		}

	protected:
		friend class MapBase<TInternals>;

		Node** CurrentNode, ** NodeListEnd;
		Node* Element;
	};

	class ConstIterator
	{
	public:
		ConstIterator(MapBase const& map)
			: CurrentNode(map.HashTable), NodeListEnd(map.HashTable + map.HashSize), Element(nullptr)
		{
			while (CurrentNode < NodeListEnd && *CurrentNode == nullptr) {
				CurrentNode++;
			}

			if (CurrentNode < NodeListEnd && *CurrentNode) {
				Element = *CurrentNode;
			}
		}

		ConstIterator(MapBase const& map, Node* const* node, Node const* element)
			: CurrentNode(node), NodeListEnd(map.HashTable + map.HashSize), Element(element)
		{}

		ConstIterator operator ++ ()
		{
			ConstIterator it(*this);

			Element = Element->Next;
			if (Element == nullptr) {
				do {
					CurrentNode++;
				} while (CurrentNode < NodeListEnd && *CurrentNode == nullptr);

				if (CurrentNode < NodeListEnd && *CurrentNode) {
					Element = *CurrentNode;
				}
			}

			return it;
		}

		ConstIterator& operator ++ (int)
		{
			Element = Element->Next;
			if (Element == nullptr) {
				do {
					CurrentNode++;
				} while (CurrentNode < NodeListEnd && *CurrentNode == nullptr);

				if (CurrentNode < NodeListEnd && *CurrentNode) {
					Element = *CurrentNode;
				}
			}

			return *this;
		}

		bool operator == (ConstIterator const& it)
		{
			return it.CurrentNode == CurrentNode && it.Element == Element;
		}

		bool operator != (ConstIterator const& it)
		{
			return it.CurrentNode != CurrentNode || it.Element != Element;
		}

		KeyType const& Key() const
		{
			return Element->Key;
		}

		ValueType const& Value() const
		{
			return Element->Value;
		}

		Node const& operator * () const
		{
			return *Element;
		}

		Node const& operator -> () const
		{
			return *Element;
		}

		operator bool() const
		{
			return Element != nullptr;
		}

		bool operator !() const
		{
			return Element == nullptr;
		}

	private:
		Node* const * CurrentNode, * const * NodeListEnd;
		Node const* Element;
	};

	MapBase(uint32_t hashSize = 31)
	{
		ResizeHashtable(hashSize);
	}

	MapBase(MapBase<TInternals> const& other)
	{
		ResizeHashtable(other.HashSize);

		for (auto const& pair : other) {
			insert(pair.Key, pair.Value);
		}
	}

	~MapBase()
	{
		if (this->HashTable) {
			clear();
			GameFree(this->HashTable);
		}
	}

	MapBase<TInternals>& operator =(MapBase<TInternals> const& other)
	{
		ResizeHashtable(other.HashSize);
		for (auto const& pair : other) {
			insert(pair.Key, pair.Value);
		}

		return *this;
	}

	void ResizeHashtable(uint32_t hashSize)
	{
		if (this->HashTable) {
			clear();
		}

		if (this->HashSize != hashSize) {
			if (this->HashTable) {
				GameFree(this->HashTable);
			}

			this->HashSize = hashSize;
			this->HashTable = GameAllocArray<Node*>(this->HashSize);
			memset(this->HashTable, 0, sizeof(Node*) * this->HashSize);
		}
	}

	void clear()
	{
		this->ItemCount = 0;
		for (uint32_t i = 0; i < this->HashSize; i++) {
			auto item = this->HashTable[i];
			if (item != nullptr) {
				FreeHashChain(item);
				this->HashTable[i] = nullptr;
			}
		}
	}

	ValueType* insert(KeyType const& key, ValueType const& value) requires std::copyable<ValueType>
	{
		auto item = this->HashTable[Hash(key) % this->HashSize];
		auto last = item;
		while (item != nullptr) {
			if (key == item->Key) {
				item->Value = value;
				return &item->Value;
			}

			last = item;
			item = item->Next;
		}

		auto node = GameAlloc<Node>(key, value);
		if (last == nullptr) {
			this->HashTable[Hash(key) % this->HashSize] = node;
		} else {
			last->Next = node;
		}

		this->ItemCount++;
		return &node->Value;
	}

	ValueType* insert(KeyType && key, ValueType && value)
	{
		auto item = this->HashTable[Hash(key) % this->HashSize];
		auto last = item;
		while (item != nullptr) {
			if (key == item->Key) {
				item->Value = std::move(value);
				return &item->Value;
			}

			last = item;
			item = item->Next;
		}

		auto node = GameAlloc<Node>(std::move(key), std::move(value));
		if (last == nullptr) {
			this->HashTable[Hash(key) % this->HashSize] = node;
		} else {
			last->Next = node;
		}

		this->ItemCount++;
		return &node->Value;
	}

	ValueType* insert(std::pair<KeyType, ValueType> const& v) requires std::copyable<ValueType>
	{
		return insert(v.first, v.second);
	}

	ValueType* insert(std::pair<KeyType, ValueType>&& v)
	{
		return insert(std::move(v.first), std::move(v.second));
	}

	void erase(Iterator const& it)
	{
		auto elem = *it.CurrentNode;
		if (elem == it.Element) {
			*it.CurrentNode = it.Element;
		} else {
			while (elem->Next != it.Element) {
				elem = elem->Next;
			}

			elem->Next = it.Element->Next;
		}
	}

	Iterator FindByValue(ValueType const& value)
	{
		for (uint32_t bucket = 0; bucket < this->HashSize; bucket++) {
			Node* item = this->HashTable[bucket];
			while (item != nullptr) {
				if (value == item->Value) {
					return Iterator(*this, this->HashTable + bucket, item);
				}

				item = item->Next;
			}
		}

		return end();
	}

	ConstIterator FindByValue(ValueType const& value) const
	{
		for (uint32_t bucket = 0; bucket < this->HashSize; bucket++) {
			Node* item = this->HashTable[bucket];
			while (item != nullptr) {
				if (value == item->Value) {
					return ConstIterator(*this, this->HashTable + bucket, item);
				}

				item = item->Next;
			}
		}

		return end();
	}

	ConstIterator find(KeyType const& key) const
	{
		auto slot = Hash(key) % this->HashSize;
		auto item = this->HashTable[slot];
		while (item != nullptr) {
			if (key == item->Key) {
				return ConstIterator(*this, this->HashTable + slot, item);
			}

			item = item->Next;
		}

		return end();
	}

	Iterator find(KeyType const& key)
	{
		auto slot = Hash(key) % this->HashSize;
		auto item = this->HashTable[slot];
		while (item != nullptr) {
			if (key == item->Key) {
				return Iterator(*this, this->HashTable + slot, item);
			}

			item = item->Next;
		}

		return end();
	}

	ValueType const* FindValueRef(KeyType const& key) const
	{
		auto it = find(key);
		if (it) {
			return &it.Value();
		} else {
			return nullptr;
		}
	}

	ValueType* FindValueRef(KeyType const& key)
	{
		auto it = find(key);
		if (it) {
			return &it.Value();
		} else {
			return nullptr;
		}
	}

	ValueType TryGet(KeyType const& key, ValueType defaultValue = {}) const
	{
		auto it = find(key);
		if (it) {
			return it.Value();
		} else {
			return defaultValue;
		}
	}

	Iterator begin()
	{
		return Iterator(*this);
	}

	Iterator end()
	{
		return Iterator(*this, this->HashTable + this->HashSize, nullptr);
	}

	ConstIterator begin() const
	{
		return ConstIterator(*this);
	}

	ConstIterator end() const
	{
		return ConstIterator(*this, this->HashTable + this->HashSize, nullptr);
	}

	inline uint32_t size() const
	{
		return this->ItemCount;
	}

private:
	void FreeHashChain(Node* node)
	{
		do {
			auto next = node->Next;
			GameDelete(node);
			node = next;
		} while (node != nullptr);
	}
};

template <class TKey, class TValue>
using Map = MapBase<MapInternals<TKey, TValue>>;

template <class TKey, class TValue>
using RefMap = MapBase<RefMapInternals<TKey, TValue>>;

END_SE()
