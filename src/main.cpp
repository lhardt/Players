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
	Trie players_trie;
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
			repo.player_ids.push_back(p.id);
			++n_players;
			//std::cout << std::string(p);
		}
	}
	std::cout << "N-players: " << n_players << '\n';
}

void generate_name_trie(Repo & repo) {
	for (auto& id : repo.player_ids) {
		Player p = repo.players.find(id);
		std::string name = p.name;

		repo.players_trie.insert(name, id);
	}
}
	
int main() {

	std::shared_ptr<Repo> repo(new Repo());
    //auto a = std::async(std::launch::async, &get_number, 5);

	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//std::cout << "Before getting the result\n";

	//int result = a.get();

	{
		Clock this_clock("Read Players File");
		read_players_file(*repo);
	}
	{
		Clock this_clock("Generate Name Trie");
		generate_name_trie(*repo);
	}

	while (true) {
		std::string name_str;
		std::cout << "Digite o nome:\n>>>\t";
		std::getline(std::cin, name_str);

		int player_id = repo->players_trie.find(name_str);
		std::cout << "Player id is " << player_id << "\n";
		
	}

	//printf("Hello, world!\n");
	return 0;
}