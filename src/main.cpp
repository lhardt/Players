#include <stdio.h>
#include <future>
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>

#include "types.h"



class Repo {
public:
	Repo();
	PlayerHashMap<20000> players;
	
	// For reference so that we can 
	// create the tree asynchronously.
	std::vector<int> player_ids;

};

Repo::Repo(){ }

int get_number(int x) {
	Clock this_clock("get_number");

	std::cout << "Inside get_number\n";
	return x + x;
}

void read_players_file(Repo & repo) {
	std::ifstream file;
	file.open("players.csv", std::ifstream::in);

	if (!file.good()) {
		std::cout << "Could'nt open players file!\ns";
	}
	int n_players = 0;
	std::string first_line;
	std::getline(file, first_line);

	bool success = true;
	while (file.good() && success) {
		Player p;
		success = get_next_player(file, p);
		if (success) {
			repo.players.insert(p);
			++n_players;
			//std::cout << std::string(p);
		}
	}
	std::cout << "N-players: " << n_players << '\n';
}
	
int main() {

	std::shared_ptr<Repo> repo(new Repo());
    //auto a = std::async(std::launch::async, &get_number, 5);

	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//std::cout << "Before getting the result\n";

	//int result = a.get();

	read_players_file(*repo);

	//printf("Hello, world!\n");
	return 0;
}