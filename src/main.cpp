#include <stdio.h>
#include <future>
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <set>
#include <iomanip>
#include "types.h"

class Repo {
public:
	Repo();
	PlayerHashMap<20000> players;
	UserHashMap<300000> users;
	Trie players_trie;

	std::array<OrderedRatingVector, N_POSITIONS> players_in_position;
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
	// There are 18944 players.
	int n_players = 0;
	std::string first_line;
	std::getline(file, first_line);

	std::set<std::string> all_positions;

	bool success = true;
	while (file.good() && success) {
		Player p;
		success = get_next_player(file, p);
		if (success) {
			for (const std::string& pos : p.positions) {
				all_positions.insert(pos);
			}
	
			repo.players.insert(p);
			repo.player_ids.push_back(p.id);
			++n_players;
		}
	}
	for (const std::string& pos : all_positions) {
		std::cout << pos << ",";
	}
	std::cout << "\n";

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

	// There are 138493 different user ids.

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

			User* u = repo.users.find(id_user);
			if (u == NULL) {
				User user(id_user);
				user.add_rating(id_player, rating);
				repo.users.insert(user);
			} else {
				u->add_rating(id_player, rating);
			}
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

void classify_players_position(Repo & repo) {
	for (int id_player : repo.player_ids) {
		Player* player = repo.players.find(id_player);
		
		if (player == NULL) continue;
		if (player->rating_count < 1000) continue;
		
		for (int pos_id : player->position_ids) {
			if (pos_id < 0 || pos_id >= N_POSITIONS) {
				std::cout << "Invalid player position with pid " << player->id << "?\n";
			}

			Rating r;
			r.id_player = id_player;
			r.rating = player->rating;
			repo.players_in_position[pos_id].ord_insert(r);
		}
	}

	for (int i = 0; i < N_POSITIONS; ++i) {
		std::cout << "\tPlayers in position " << index_to_position(i) << ": " << repo.players_in_position[i].size() << "\n";
	}
}

void print_user_rating(int user_id, const std::vector<Rating> & ratings, const std::shared_ptr<Repo> & repo) {
	double nota = 5.0;

	while (nota >= 0.0) {
		for (const Rating& r : ratings) {
			if (r.rating == nota) {
				Player* p = repo->players.find(r.id_player);
				std::cout
					<< std::setw(7) << p->id
					<< std::setw(40) << p->name
					<< std::setw(20) << p->get_positions_str()
					<< std::setw(12) << p->rating
					<< std::setw(6) << r.rating
					<< std::endl;
			}
		}
		nota -= 0.5;
	}
}

void load_repo(Repo & repo ) {
	Clock all_clock("Loading Repo");

	{
		Clock this_clock("Read Players File");
		read_players_file(repo);
	}
	{
		Clock this_clock("Generate Name Trie");
		generate_name_trie(repo);
	}
	{
		Clock this_clock("Load Ratings");
		read_ratings_file(repo);
	}
	{
		Clock this_clock("Classify Players");
		classify_players_position(repo);
	}
}

int main() {

	std::shared_ptr<Repo> repo(new Repo());
	
	load_repo(*repo);

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
			
			if (player_ids.size() == 0) {
				std::cout << "No such players found.\n";
				continue;
			}

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
		if (until_first_space == "user") {
			int id_user = std::stoi(param_str);

			User* u = repo->users.find(id_user);

			if (u == NULL) {
				std::cout << "Not found.\n";
				continue;
			} else {
				if (u->ratings.size() == 0) {
					std::cout << "This user hasn't rated any player.\n";
					continue;
				}

				std::cout << "His ratings: \n";

				std::cout
					<< std::setw(7) << "ID"
					<< std::setw(40) << "Player Name"
					<< std::setw(20) << "Positions"
					<< std::setw(12) << "Av. Rating"
					<< std::setw(6) << "Usr. Rating"
					<< std::endl;

				print_user_rating(id_user, u->ratings, repo);
			}

		}
		
		if (until_first_space.find("top") == 0) {
			std::string n_top_str = until_first_space.substr(3);
			
			int n_top = std::stoi(n_top_str);
			

			int i_pos = position_to_index(param_str);
			if (i_pos == -1) { std::cout << "Did not find the position <" << param_str << ">.\n"; continue; }
			

			if (n_top > repo->players_in_position[i_pos].size()) {
				n_top = repo->players_in_position[i_pos].size();
				std::cout << "There are only " << n_top << " players in this position (w/ +1000 ratings).\n";
			}

			std::cout << "Top " << n_top << "of position " << param_str << ":\n";

			//std::vector<int> top_list = get_top_n(*repo, i_pos, n_top);


			for (int i = 0; i < n_top; ++i) {
				int id_player = repo->players_in_position[i_pos][i].id_player;

				Player p = *(repo->players.find(id_player));
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