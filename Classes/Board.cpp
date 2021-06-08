#include "Board.h"
#include <map>
#include <deque>

Board::Board(int n_rows, int n_columns, int n_types, std::vector<int> count) :
	n_rows(n_rows), n_columns(n_columns),
	_pokemons(std::vector<std::vector<int>>(n_rows, std::vector<int>(n_columns, -1)))
{
	std::map<int, int> countType; // countType[x] counts number of type x
	for (int i = 0; i < n_rows; ++i) {
		for (int j = 0; j < n_columns; ++j) {
			int type;
			do {
				type = rand() % n_types;
			} while (countType[type] >= count[type]);
			countType[type] += 1;
			addPokemon(i, j, type + 1);
		}
	}
}

int Board::getNRows()
{
	return n_rows;
}

int Board::getNColumns()
{
	return n_columns;
}

void Board::addPokemon(int x, int y, int type)
{
	_pokemons[x][y] = type;
}

int Board::getPokemon(int x, int y)
{
	return _pokemons[x][y];
}

void Board::removePokemon(int x, int y)
{
	_pokemons[x][y] = -1;
}

bool Board::selectPokemon(int x, int y) {

	if (_x == -1 && _y == -1 || _pokemons[x][y] != _pokemons[_x][_y] || !canConnect(_x, _y, x, y)) {
		return false;
	}
	return true;
}

bool Board::canConnect(int _x, int _y, int x, int y) {

	auto path = findPath(_x, _y, x, y);
	return path.size() >= 2 && path.size() <= 4;
}

std::vector<std::pair<int, int>> Board::findPath(int _x, int _y, int x, int y)
{
	//INIT Graph
	std::vector<std::vector<int>> e(n_rows + 2, std::vector<int>(n_columns + 2, 0));
	for (int i = 0; i < n_rows; ++i)
	{
		for (int j = 0; j < n_columns; ++j)
		{
			e[i + 1][j + 1] = _pokemons[i][j] != -1;
		}
	}
	std::pair<int, int> s = { _x + 1, _y + 1 };
	std::pair<int, int> t = { x + 1, y + 1 };

	//BFS
	const int dx[4] = { -1, 0, 1, 0 };
	const int dy[4] = { 0, 1, 0, -1 };
	std::deque<std::pair<int, int>> q;
	std::vector<std::vector<std::pair<int, int>>> trace(e.size(), std::vector<std::pair<int, int>>(e[0].size(), std::make_pair(-1, -1)));
	q.push_back(t);
	trace[t.first][t.second] = std::make_pair(-2, -2);
	e[s.first][s.second] = 0;
	e[t.first][t.second] = 0;
	while (!q.empty()) {
		auto u = q.front();
		q.pop_front();
		if (u == s) break;
		for (int i = 0; i < 4; ++i) {
			int x = u.first + dx[i];
			int y = u.second + dy[i];
			while (x >= 0 && x < e.size() && y >= 0 && y < e[0].size() && e[x][y] == 0) {
				if (trace[x][y].first == -1) {
					trace[x][y] = u;
					q.push_back({ x, y });
				}
				x += dx[i];
				y += dy[i];
			}
		}
	}

	//trace back
	std::vector<std::pair<int, int>> res;
	if (trace[s.first][s.second].first != -1) {
		while (s.first != -2) {
			res.push_back({ s.first - 1, s.second - 1 });
			s = trace[s.first][s.second];
		}
	}
	return res;
}
