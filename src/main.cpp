#include <stdio.h>
#include <future>
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <iomanip>

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
		}
	}
	std::cout << "N-players: " << n_players << '\n';
}

void generate_name_trie(Repo & repo) {
	for (auto& id : repo.player_ids) {
		Player * p = repo.players.find(id);
		std::string name = p->name;

		repo.players_trie.insert(name, id);
	}
}

void read_ratings_file(Repo& repo) {
	// Using FILE here because ifstream would be too slow.

	FILE* c_file;
	int error = fopen_s(&c_file, "rating.csv", "r");
	int n_ratings = 0;

	if (c_file == NULL) {
		std::cout << "Error!\n";
		return;
	}
	// ignore the first line
	fscanf_s(c_file, "%*s");

	bool success = true;
	std::string this_line;
	while (success) {
		int id_user, id_player; double rating;
		int ret_val = fscanf_s(c_file, "%d, %d, %lf\n ", &id_user, &id_player, &rating);

		success = ret_val == 3;

		if (success) {
			Player* p = repo.players.find(id_player);
			p->rating += rating;
			p->rating_count += 1;
			++n_ratings;
		}
	}

	{
		Clock this_clock("Fixing Ratings");
		for (auto& id : repo.player_ids) {
			Player* p = repo.players.find(id);
			p->rating = p->rating / p->rating_count;

			//		std::cout << "Rating is " << p->rating << "\n";
		}

	}
	std::cout << "N-ratings: " << n_ratings << '\n';

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
	{
		Clock this_clock("Load Ratings");
		read_ratings_file(*repo);
	}

	while (true) {
		std::string query_string;
		std::cout << "Consulta [player,user,topN,tags]:\n>>>\t";
		std::getline(std::cin, query_string);

		int first_space = query_string.find_first_of(' ');
		if (first_space == -1) {
			std::cout << "Tente Novamente.\n";
			continue;
		}

		std::string until_first_space = query_string.substr(0, first_space);
	
		std::string param_str = query_string.substr(first_space + 1);
		
		std::cout << std::setiosflags(std::ios::left);

		if (until_first_space == "player") {

			std::vector<int> player_ids = repo->players_trie.find_all(param_str);
			std::cout
				<< std::setw(7) << "ID"
				<< std::setw(40) << "Player Name"
				<< std::setw(20) << "Positions"
				<< std::setw(12) << "Rating"
				<< std::setw(6) << "Count"
				<< std::endl;
			for (auto& id : player_ids) {
				Player p = * (repo->players.find(id));
				std::cout
					<< std::setw(7) << p.id
					<< std::setw(40) << p.name
					<< std::setw(20) << p.get_positions_str()
					<< std::setw(12) << p.rating
					<< std::setw(6) << p.rating_count
					<< std::endl;
			}
		}
	
	}
	return 0;
}