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
};

template<typename K, typename T, unsigned int size>
class HashMap {
public:
	HashMap();
	T find(unsigned int hash);
	T find(std::string name);
	void insert(T item);

	virtual unsigned int get_key_hash(K) = 0;
	virtual K get_key(T) = 0;

private:
	std::array<T, size> items;
	std::array<bool, size> item_used;
};

//template<typename K, typename T>
//class OrderedMap {
//public://
//	Set();
//	T find(T value);
//
//};


template<unsigned int N>
class PlayerHashMap : public HashMap<int, Player, N> {
public:
	PlayerHashMap();
	virtual unsigned int get_key_hash(int);
	virtual int get_key(Player);
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


#endif /* TYPES_H */