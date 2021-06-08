#pragma once
#include <cocos2d.h>
#include <Board.h>

USING_NS_CC;

class BoardView : public Layer
{
	Board* board;

	float _squareSize, _width, _height;

	std::vector<std::vector<Sprite*>> _pokemons;

public:

	static Layer* createBoardView(Board* board);

	void showBoard();

	Sprite* addPokemon(int row, int column, int type);

	Vec2 positionOf(int row, int column);

	std::pair<int, int> findRowAndColumnOfSprite(Node* node);

	bool removePokemon(int row, int column);

	bool onTouchPokemon(Touch* touch, Event* event);

	void connectPokemons(int x, int y, int _x, int _y);

	FiniteTimeAction* getConnectEffect(int x, int y, int _x, int _y);

	void createChoosePokemonEffect(Node* pokemon);

	void removeChoosePokemonEffect();

	CREATE_FUNC(BoardView);
};