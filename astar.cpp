#include "astar.hpp"

//Calcule la distance euclidienne
inline float heuristic(const Maze& maze, const Tile& t1, const Tile& t2){

	int x_start = t1.index % maze.width;
	int y_start = t1.index / maze.width;

	int x_end = t2.index % maze.width;
	int y_end = t2.index / maze.width;

	float dist = sqrt((x_end - x_start)*(x_end - x_start) + (y_end - y_start)*(y_end - y_start));

	return sqrt((t2.altitude - t1.altitude)*(t2.altitude - t1.altitude) + dist*dist);
}

// Fonction de comparaison pour la prio
bool compare_heuristic(const std::pair<float, int>& t1, const std::pair<float ,int>& t2){
	return t1.first <= t1.first;
}

ASNODE* astar(const Maze& maze, int start_index, int end_index, PathData& path_data, float& search_time, bool step_by_step){

	/* Initilise l'affichage du chemin (fait avant la première
	 * mesure du temps pour ne pas les fausser */
	path_data.status = new PathStatus[maze.tile_size];
	path_data.data_size = maze.tile_size ;


	//On initialise le tableau de noeud et path_data
	for(int i = 0; i < maze.tile_size; i++){
		path_data.status[i] = MAZE_PATH_UNKNOWN;
	}

	struct timespec ta, tb;
	getRealTime(&ta);

	ASNODE* nodes = new ASNODE[maze.tile_size];

	// Initialise les noeuds
	for(int i = 0; i < maze.tile_size; i++){
		nodes[i].color = WHITE;
		nodes[i].parent_index = -1;
		nodes[i].g_cost = 0;
		nodes[i].h_cost = 0;
	}

	// File de prio (par tas binaire)
	std::priority_queue<std::pair<float,int>,
						std::vector<std::pair<float,int> >,
						decltype(&compare_heuristic)> list_grey(compare_heuristic);

	// Commence avec la première case
	list_grey.push(std::make_pair(0, start_index));

	bool way_founded = false;
	float curr_g_cost;

	// g(x) distance estimée (heuristique) depuis la case x à la case d'arrivée
	// h(x) distance de la case de départ à la case x
	// f(x) = h(x) + g(x)

	while(!list_grey.empty()){

		// Selection de la case ayant l'heuristique la plus faible
		Tile& curr_tile = maze.tiles[list_grey.top().second];
		ASNODE& curr_node = nodes[curr_tile.index];

		if(curr_tile.index == end_index){
			way_founded = true;
			break;
		}

		list_grey.pop();
		curr_node.color = BLACK;
		path_data.status[curr_tile.index] = MAZE_PATH_EXPLORED;


		// Parcours des voisins de la case
		for(int i = 0; i < curr_tile.neighbor_size; i++){

			// Case hors grille ou derrière un mur
			if(curr_tile.neighbors[i] == NULL || curr_tile.walls[i] != 0){
				continue;
			}

			Tile& curr_neighbor_tile = *curr_tile.neighbors[i];
			ASNODE& curr_neighbor_node = nodes[curr_neighbor_tile.index];

			// Case déjà visitée
			if(curr_neighbor_node.color == BLACK){
				continue;
			}

			curr_neighbor_node.h_cost = heuristic(maze, curr_neighbor_tile, maze.tiles[end_index]);
			//La distance depuis le départ du noeud courant
			curr_g_cost = curr_node.g_cost
				+ sqrt(1 + pow(curr_neighbor_tile.altitude - curr_tile.altitude, 2));

			// Cette case voisine n'a pas encore été visitée ou offre un coût inférieur
			if(curr_neighbor_node.color != GREY ||
				curr_g_cost < curr_neighbor_node.g_cost){

				// On met a jour la distance g
				curr_neighbor_node.g_cost = curr_g_cost;


				if(curr_neighbor_node.color != GREY){
					list_grey.push(std::make_pair(
						curr_neighbor_node.g_cost + curr_neighbor_node.h_cost,
						curr_neighbor_tile.index
					));
				}

				curr_neighbor_node.parent_index = curr_tile.index;
				curr_neighbor_node.color = GREY;
				path_data.status[curr_neighbor_tile.index] = MAZE_PATH_SEARCHED;

				if(step_by_step){
					system("clear");

					path_data.status[start_index] = MAZE_PATH_START;
					path_data.status[end_index] = MAZE_PATH_END;
					maze_grid_print_path(maze, maze.height, maze.width, path_data);
					std::cout << std::endl << std::endl << "[Appuyez ou laisser appuyez sur entrée pour afficher l'étape suivante]\n";
					std::cout << "Appuyez sur autre touche pour terminer la recherche";

					system("stty -icanon");

					if(getchar() != 10){
						step_by_step = false;
						system("clear");
					}

					system("stty icanon");
				}
			}
		}
	}

	path_data.status[start_index] = MAZE_PATH_START;
	path_data.status[end_index] = MAZE_PATH_END;

	if(!way_founded){
		return nodes;
	}

	// Enregistre le chemin trouvé par remontée depuis le dernier
	int curr_index = nodes[end_index].parent_index;

	// Construction du chemin
	while(nodes[curr_index].parent_index != -1){
		path_data.status[curr_index] = MAZE_PATH_IN;
		curr_index = nodes[curr_index].parent_index;

	};

	getRealTime(&tb);

	search_time	= (tb.tv_sec + tb.tv_nsec - ta.tv_sec - ta.tv_nsec)/1000000.0;

	return nodes;
}

void getRealTime(struct timespec* t){
	#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	t->tv_sec = mts.tv_sec;
	t->tv_nsec = mts.tv_nsec;

	#else
	clock_gettime(CLOCK_REALTIME, &t);
	#endif
}
