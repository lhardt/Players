
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
HashMap<K,T,sz>::HashMap() : items(), item_used() {

}

template<typename K, typename T, unsigned int sz>
T HashMap<K, T,sz>::find(unsigned int hash) {
	return items[0];
}

template<typename K, typename T, unsigned int sz>
T HashMap<K, T,sz>::find(std::string name) {
	return items[0];
}

template<typename K, typename T, unsigned int sz>
void HashMap<K, T,sz>::insert(T item) {
	unsigned int hash = get_key_hash(get_key(item)) % sz;

	while(this->item_used[hash]) {
		hash = (hash + 1) % sz;
	}
	std::cout << "Inserting item at " << hash << "\n";

	this->items[hash] = item;
	this->item_used[hash] = true;

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
