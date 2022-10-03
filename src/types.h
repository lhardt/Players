#ifndef TYPES_H
#define TYPES_H

#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>
#include <array>

class Player {
public:
	int id;
	std::string name;
	std::vector<std::string> positions;
	double rating;
	int rating_count;
		
	Player();

	operator std::string() const;
	std::string get_positions_str();
};

class Rating {
public:
	int id_user;
	int id_player;
	int rating;
};

template<typename K, typename T, unsigned int size>
class HashMap {
public:
	HashMap();
	T* find(K name);
	void insert(T item);

	virtual unsigned int get_key_hash(K) = 0;
	virtual K get_key(T) = 0;

private:
	std::array<T, size> items;
	std::array<K, size> keys;
	std::array<bool, size> item_used;
};

//template<typename K, typename T>
//class OrderedMap {
//public:
//	OrderedMap();
//	T find(K key);
//	void add(K key, T value);
//
//private:
//	std::vector<T> items;
//};

class Trie {
public:
	static const int N_SUBTREES = 27;

	Trie();
	// Recursively copying the string would be unnecessary
	int find(const std::string & key, int key_start_pos = 0);
	std::vector<int> find_all(const std::string& key, int key_start_pos = 0);
	std::vector<int> get_all();
	void insert(const std::string&  key, int value, int key_start_pos = 0 );

	bool has_value;
	int value;

	Trie* subtrees[27];
};

// a HashMap from player_id into Player.
template<unsigned int N>
class PlayerHashMap : public HashMap<int, Player, N> {
public:
	PlayerHashMap();
	virtual unsigned int get_key_hash(int);
	virtual int get_key(Player);
};

template<unsigned int N>
class PositionHashMap : public HashMap<std::string, int, N> {
public: 
	PositionHashMap();
	virtual unsigned int get_key_hash(std::string);
	virtual int get_key();
};

// Quick class that times its existence from constructor to destructor.
class Clock {
public:
	Clock(std::string funcname);
	~Clock();
	clock_t m_clock;
	std::string m_funcname;
};


bool get_next_player(std::ifstream &file, Player & p);
bool get_next_rating(std::ifstream& file, Rating& r);


#endif /* TYPES_H */