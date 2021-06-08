#pragma once
#include <vector>

class Board
{
	int n_rows, n_columns;	// number of rows and columns

	std::vector<std::vector<int>> _pokemons;	// pokemons

public:
	int _x = -1, _y = -1;	// selected pokemon row and column

	Board(int n_rows, int n_columns, int n_types, std::vector<int> count);

	int getNRows();

	int getNColumns();

	void addPokemon(int x, int y, int type);

	int getPokemon(int x, int y);

	void removePokemon(int x, int y);

	bool selectPokemon(int x, int y);

	bool canConnect(int _x, int _y, int x, int y);

	std::vector<std::pair<int, int>> findPath(int _x, int _y, int x, int y);
};