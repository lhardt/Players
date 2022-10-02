
#include "types.h"


unsigned int str_get_hash(std::string str) {
	constexpr unsigned int ADLER_MOD = 65521;
	// Adler32 implementation
	unsigned int  a = 1, b = 0;
	for (int i = 0; i < str.length(); ++i) {
		a = (a + str[i]) % ADLER_MOD;
		b = (b + a) % ADLER_MOD;
	}
	// a já é garantidamente menor que ADLER_MOD
	return (b << 16) | a;
}

Player::operator std::string() const {
	std::string final_str = ("[");
	final_str += std::to_string(this->id);
	final_str += " ";
	final_str += this->name;
	final_str += " [";
	for (std::string str : this->positions) {
		final_str += " " + str;
	}
	final_str += " ]]";

	return final_str;
}
Player::Player() : id(0), name(""), positions(), rating(0), rating_count(0) {
}


template<typename K, typename T, unsigned int sz>
HashMap<K,T,sz>::HashMap() : items(), item_used(), keys() {

}

template<typename K, typename T, unsigned int sz>
T HashMap<K, T,sz>::find(K key) {
	int hash = get_key_hash(key) % sz;

	while (this->item_used[hash]) {
		if (keys[hash] == key) {
			return items[hash % sz];
		}

		hash = (hash + 1) % sz;
	}
	return items[0];
}

template<typename K, typename T, unsigned int sz>
void HashMap<K, T,sz>::insert(T item) {
	unsigned int hash = get_key_hash(get_key(item)) % sz;

	while(this->item_used[hash]) {
		hash = (hash + 1) % sz;
	}
	//std::cout << "Inserting item at " << hash << "\n";

	this->items[hash] = item;
	this->item_used[hash] = true;
	this->keys[hash] = get_key(item);
}


int get_key_subarray(const std::string & str, int str_start_pos) {
	if (str_start_pos >= str.length()) return -1;
	if (str_start_pos < -1) {
		std::cout << "WTF!!!!\n";
		return -1;
	}

	char chr = str[str_start_pos];
	
	if (chr >= 'a' && chr <= 'z') return chr - 'a';
	if (chr >= 'A' && chr <= 'Z') return chr - 'A';
	if (chr >= '0' && chr <= '9') return 26;
	if (chr >= '\'' || chr <= ' ' || chr == '-') return 26;

	printf("Warning: Ignoring Character %x(%c)\n", chr, chr);
	return 26;
}

Trie::Trie() {
	this->has_value = false;
	this->value = -1;
	for (int i = 0; i < 27; ++i) {
		this->subtrees[i] = NULL;
	}
}

int Trie::find(const std::string & key, int key_start_pos) {
	if (key_start_pos == key.length()) {
		printf("Found node in Trie!");
		return this->value;
	}
	else {
		int subtree_key = get_key_subarray(key, key_start_pos);

		if (subtree_key >= 0 && subtrees[subtree_key] != NULL) {
			return subtrees[subtree_key]->find(key, key_start_pos + 1);
		} else {
			std::cout << "Not Found! " << key << " in Trie.\n";
		}
	}

	return -1;
}

void  Trie::insert(const std::string& key, int value, int key_start_pos) {
	if (key_start_pos == key.length()) {
		if (this->value != -1) {
//			std::cout << "Trying to insert already inserted? New: " << value << ", old: " << this->value << "\n";
		}
//		std::cout << "Inserted key <" << key << ">\n";
		this->value = value;
	} else {
		int subtree_key = get_key_subarray(key, key_start_pos);
		if (subtree_key >= 0) {
			if (subtrees[subtree_key] == NULL) {
				subtrees[subtree_key] = new Trie();
			}
			subtrees[subtree_key]->insert(key, value, key_start_pos + 1);
		}
	}
}


template<unsigned int N>
int PlayerHashMap<N>::get_key(Player p) {
	return p.id;
}

template<unsigned int N>
unsigned int PlayerHashMap<N>::get_key_hash(int id) {
	return id;
}

template class HashMap<int, Player, 20000>;
template class PlayerHashMap<20000>;

template<unsigned int N>
PlayerHashMap<N>::PlayerHashMap() {

}
