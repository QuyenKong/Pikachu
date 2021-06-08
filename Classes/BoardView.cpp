#include "BoardView.h"
#include "algorithm"

Layer* BoardView::createBoardView(Board* board)
{
	auto boardView = BoardView::create();
	boardView->board = board;
	boardView->showBoard();
	return boardView;
}

void BoardView::showBoard()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	_squareSize = visibleSize._width / (board->getNColumns() + 5);
	_width = _squareSize * board->getNColumns();
	_height = _squareSize * board->getNRows();
	setContentSize({ _width, _height });

	_pokemons.resize(board->getNRows());
	for (int i = 0; i < board->getNRows(); ++i) {
		_pokemons[i].resize(board->getNColumns());
		for (int j = 0; j < board->getNColumns(); ++j) {
			_pokemons[i][j] = addPokemon(i, j, board->getPokemon(i, j));
			addChild(_pokemons[i][j]);
		}
	}
}

Sprite* BoardView::addPokemon(int row, int column, int type)
{
	auto pokemon = Sprite::create("res/pokemons/" + std::to_string(type) + ".png");
	pokemon->setScaleX(_squareSize / pokemon->getContentSize()._width);
	pokemon->setScaleY(_squareSize / pokemon->getContentSize().height);
	Vec2 position = positionOf(row, column);
	pokemon->setPosition(position);


	//EventListener
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(BoardView::onTouchPokemon, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, pokemon);

	return pokemon;
}

Vec2 BoardView::positionOf(int row, int column)
{
	return Vec2(column * _squareSize + _squareSize / 2, _height - row * _squareSize - _squareSize / 2);
}

std::pair<int, int> BoardView::findRowAndColumnOfSprite(Node* node)
{
	for (int i = 0; i < board->getNRows(); ++i) {
		for (int j = 0; j < board->getNColumns(); ++j) {
			if (_pokemons[i][j] == node) {
				return { i, j };
			}
		}
	}
	return { -1, -1 };
}

bool BoardView::removePokemon(int row, int column)
{
	if (_pokemons[row][column] == nullptr) return false;
	board->removePokemon(row, column);
	_pokemons[row][column] = nullptr;
	return true;
}
bool BoardView::onTouchPokemon(Touch* touch, Event* event) {
	auto touchLocation = touch->getLocation() - this->getPosition();
	auto target = event->getCurrentTarget();
	if (target->getBoundingBox().containsPoint(touchLocation)) {
		auto p = findRowAndColumnOfSprite(target);
		removeChoosePokemonEffect();
		if (board->selectPokemon(p.first, p.second)) {
			connectPokemons(board->_x, board->_y, p.first, p.second);
			board->_x = board->_y = -1;
		}
		else {
			createChoosePokemonEffect(_pokemons[p.first][p.second]);
			board->_x = p.first;
			board->_y = p.second;
		}
		return true;
	}
	return false;
}
void BoardView::connectPokemons(int x, int y, int _x, int _y) {
	// 1: Hieu ung noi 2 pokemon
	auto connectEffect = getConnectEffect(x, y, _x, _y);

	// 2: Hieu ung lam mo 2 pokemon
	auto pokemonFade1 = TargetedAction::create(_pokemons[x][y], FadeOut::create(0.5));
	auto pokemonFade2 = TargetedAction::create(_pokemons[_x][_y], FadeOut::create(0.5));
	auto effectSpawn = Spawn::create(pokemonFade1, pokemonFade2, nullptr);

	// 3: Xoa 2 pokemon
	auto removePokemon1 = CallFunc::create(CC_CALLBACK_0(BoardView::removePokemon, this, x, y));
	auto removePokemon2 = CallFunc::create(CC_CALLBACK_0(BoardView::removePokemon, this, _x, _y));
	auto removePokemonSpawn = Spawn::create(removePokemon1, removePokemon2, nullptr);

	// Sequence(1, 2, 3)
	auto sequence = Sequence::create(connectEffect, effectSpawn, removePokemonSpawn, nullptr);
	this->runAction(sequence);
}	

FiniteTimeAction* BoardView::getConnectEffect(int x, int y, int _x, int _y) {
	auto path = board->findPath(x, y, _x, _y);
	std::vector<Vec2> _path;
	for (auto p : path) {
		_path.push_back(positionOf(p.first, p.second));
	}

	auto emitter = ParticleSystemQuad::create("res/path.plist");
	this->addChild(emitter);
	float duration = 0.3f;
	emitter->setDuration(duration);
	emitter->setPosition(_path[0]);
	Vector<FiniteTimeAction*> actions;
	for (int i = 1; i < _path.size(); ++i) {
		actions.pushBack((FiniteTimeAction*)MoveTo::create(duration / (_path.size() - 1), _path[i]));
	}
	return TargetedAction::create(emitter, Sequence::create(actions));
}

void BoardView::createChoosePokemonEffect(Node* pokemon)
{
	auto emitter = ParticleSystemQuad::create("res/path.plist");
	auto square = pokemon->getBoundingBox();
	emitter->setPosition(square.getMinX(), square.getMinY()); //Dat hieu ung ban dau o goc trai duoi pokemón

	// Tao hieu ung particle chay quanh pokemon
	auto moveUp = MoveBy::create(0.2, Vec2(0, _squareSize));
	auto moveRight = MoveBy::create(0.2, Vec2(_squareSize, 0));
	auto moveDown = MoveBy::create(0.2, Vec2(0, -_squareSize));
	auto moveLeft = MoveBy::create(0.2, Vec2(-_squareSize, 0));
	auto sequence = RepeatForever::create(Sequence::create(moveUp, moveRight, moveDown, moveLeft, nullptr));
	emitter->runAction(sequence);

	// Chay hieu ung
	this->addChild(emitter, 2);
	emitter->setName("choosePokemon");
}

void BoardView::removeChoosePokemonEffect() {
	if (this->getChildByName("choosePokemon") != nullptr)
		this->removeChildByName("choosePokemon");
}