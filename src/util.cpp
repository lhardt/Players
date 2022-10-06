
#include "types.h"


unsigned int str_get_hash(std::string str) {
	constexpr unsigned int ADLER_MOD = 65521;
	// Adler32 implementation
	unsigned int  a = 1, b = 0;
	for (int i = 0; i < str.length(); ++i) {
		a = (a + str[i]) % ADLER_MOD;
		b = (b + a) % ADLER_MOD;
	}
	// a j� � garantidamente menor que ADLER_MOD
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
std::string Player::get_positions_str() {
	std::string str= "";
	if (positions.size() > 0) {
		str += positions[0];
		for (int i = 1; i < positions.size(); ++i) {
			str += ',';
			str += positions[i];
		}

	}
	return str;
}


User::User() : id(-1), ratings() {

}

User::User(int _id) : id(_id), ratings() {

}

void User::add_rating(int id_player, double rating) {
	Rating r;
	r.id_player = id_player;
	r.rating = rating;

	// too slow
	//ratings.ord_insert(r);
	ratings.push_back(r);
}



template<typename K, typename T, unsigned int sz>
HashMap<K,T,sz>::HashMap() : items(), item_used(), keys() {

}

template<typename K, typename T, unsigned int sz>
T* HashMap<K, T,sz>::find(K key) {
	int hash = get_key_hash(key) % sz;

	while (this->item_used[hash]) {
		if (keys[hash] == key) {
			return & items[hash % sz];
		}

		hash = (hash + 1) % sz;
	}
	return NULL;
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


template<typename T>
void OrderedVector<T>::ord_insert(const T& val) {
	int place_to_insert = binary_search(val, 0, this->size()-1);
	this->insert(this->begin() + place_to_insert, val);
}

template<typename T>
int OrderedVector<T>::binary_search(const T& value, int low, int high) {
	while (low <= high) {
		int mid = (low + high) / 2;
		
		if (compare(value, this->at(mid))) {
			low = mid;
		} else {
			high = mid;
		}
	}
	return low;
}

bool OrderedRatingVector::compare(const Rating& r1, const Rating& r2) {
	return r1.rating < r2.rating;
}


//template<typename T>
//bool OrderedVector<T>::compare(const T& a, const T& b) {
//
//}




int get_key_subarray(const std::string & str, int str_start_pos) {
	if (str_start_pos >= str.length()) return -1;
	if (str_start_pos < -1) {
		std::cout << "WTF!!!!\n";
		return -1;
	}

	char chr = str[str_start_pos];
	
	if (chr >= 'a' && chr <= 'z') return 1 + chr - 'a';
	if (chr >= 'A' && chr <= 'Z') return 1 + chr - 'A';
	if (chr >= '0' && chr <= '9') return 0;
	if (chr >= '\'' || chr <= ' ' || chr == '-') return 0;

	printf("Warning: Ignoring Character %x(%c)\n", chr, chr);
	return 0;
}

Trie::Trie() {
	this->has_value = false;
	this->value = -1;
	for (int i = 0; i < N_SUBTREES; ++i) {
		this->subtrees[i] = NULL;
	}
}

int Trie::find(const std::string & key, int key_start_pos) {
	if (key_start_pos == key.length()) {
//		printf("Found node in Trie!");
		// Does not necessarily mean it found anything.
		return this->value;
	} else {
		int subtree_key = get_key_subarray(key, key_start_pos);

		if (subtree_key >= 0 && subtrees[subtree_key] != NULL) {
			return subtrees[subtree_key]->find(key, key_start_pos + 1);
		} else {
			std::cout << "Not Found! " << key << " in Trie.\n";
		}
	}

	return -1;
}

std::vector<int> Trie::get_all() {
	std::vector<int> ret_val;
	
	if (this->value != -1) { ret_val.push_back(this->value); }
	
	for (int i = 0; i < N_SUBTREES; ++i) {
		if (subtrees[i] != NULL) {
			std::vector<int> that_result = subtrees[i]->get_all();
			
			for (auto& el : that_result) {
				ret_val.push_back(el);
			}
		}
	}

	return ret_val;
}

std::vector<int> Trie::find_all(const std::string& key, int key_start_pos) {
	if (key_start_pos == key.length()) {
		return get_all();
	} else {
		int subtree_key = get_key_subarray(key, key_start_pos);

		if (subtree_key >= 0 && subtrees[subtree_key] != NULL) {
			return subtrees[subtree_key]->find_all(key, key_start_pos + 1);
		} else {
			return std::vector<int>();
		}

	}	
	std::cout << "Unreachable?!\n";
	return std::vector<int>();
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
template class HashMap<int, User, 300000>;
template class PlayerHashMap<20000>;
template class UserHashMap<300000>;

template<unsigned int N>
PlayerHashMap<N>::PlayerHashMap() {

}

template<unsigned int N>
UserHashMap<N>::UserHashMap() {

}

template<unsigned int N>
int UserHashMap<N>::get_key(User u) {
	return u.id;
}

template<unsigned int N>
unsigned int UserHashMap<N>::get_key_hash(int id) {
	return id;
}

