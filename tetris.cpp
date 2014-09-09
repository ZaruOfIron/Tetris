#include "tetris.hpp"
#include <list>
#include <utility>

#include <iostream>

namespace Tetris
{

TetriminoCore::TetriminoCore(int color, int width, int height, const std::vector<BlockBitMap>& blockMaps)
	: color_(color), width_(width), height_(height), blockMaps_(std::move(blockMaps))
{
	THROW_ERROR_UNLESS(width * height <= 64);
}

int TetriminoCore::color() const
{
	return color_;
}

int TetriminoCore::width() const
{
	return width_;
}

int TetriminoCore::height() const
{
	return height_;
}

BlockBitMap TetriminoCore::bits(int index) const
{
	return blockMaps_.at(HooLib::rotateIndex(index, blockMaps_.size()));
}

///

Tetrimino::Tetrimino()
	: tetriminoCoreData_(nullptr), rotationIndex_(0)
{}

bool Tetrimino::isValid() const
{
	return tetriminoCoreData_ != nullptr;
}

int Tetrimino::color() const
{
	THROW_ERROR_UNLESS(isValid());
	return tetriminoCoreData_->color();
}

int Tetrimino::width() const
{
	THROW_ERROR_UNLESS(isValid());
	return tetriminoCoreData_->width();
}

int Tetrimino::height() const
{
	THROW_ERROR_UNLESS(isValid());
	return tetriminoCoreData_->height();
}

BlockBitMap Tetrimino::bits() const
{
	THROW_ERROR_UNLESS(isValid());
	return tetriminoCoreData_->bits(rotationIndex_);
}

void Tetrimino::update(const TetriminoCore *tetriminoCoreData)
{
	tetriminoCoreData_ = tetriminoCoreData;
	rotationIndex_ = 0;
}

void Tetrimino::rotateToLeft()
{
	THROW_ERROR_UNLESS(isValid());
	rotationIndex_--;
}

void Tetrimino::rotateToRight()
{
	THROW_ERROR_UNLESS(isValid());
	rotationIndex_++;
}

///

Board::Board(int width, int height)
	: BOARD_WIDTH(width), BOARD_HEIGHT(height), fixedBlockColorData_(width * height)
{
}

bool Board::isValid() const
{
	return fallingTetriminoData_.tetrimino.isValid();
}

BlockBitMap Board::getFixedBlockColorDataAtFallingTetriminoPos(int posX, int posY) const
{
	THROW_ERROR_UNLESS(isValid());

	BlockBitMap ret = 0;
	for(int y = posY;y < posY + fallingTetriminoData_.tetrimino.height();y++){
		for(int x = posX;x < posX + fallingTetriminoData_.tetrimino.width();x++){
			if(!(x >= 0 && y >= 0 && y < BOARD_HEIGHT && x < BOARD_WIDTH) || fixedBlockColorData_.at(y * BOARD_WIDTH + x))
				ret |= 1 << ((y - posY) * fallingTetriminoData_.tetrimino.width() + (x - posX));
		}
	}

	return ret;
}

BlockBitMap Board::getFixedBlockColorDataAtFallingTetriminoPos() const
{
	THROW_ERROR_UNLESS(isValid());

	return getFixedBlockColorDataAtFallingTetriminoPos(fallingTetriminoData_.posX, fallingTetriminoData_.posY);
}

void Board::fixFallingTetrimino()
{
	THROW_ERROR_UNLESS(isValid());

	auto& tet = fallingTetriminoData_.tetrimino;
	BlockBitMap bits = tet.bits();
	for(int y = fallingTetriminoData_.posY;y < fallingTetriminoData_.posY + tet.height();y++){
		for(int x = fallingTetriminoData_.posX;x < fallingTetriminoData_.posX + tet.width();x++){
			if(!(x >= 0 && y >= 0 && x < BOARD_WIDTH && y < BOARD_HEIGHT)
				|| (bits & 1 << ((y - fallingTetriminoData_.posY) * tet.width() + (x - fallingTetriminoData_.posX))) == 0)	continue;
			int pos = y * BOARD_WIDTH + x;
			THROW_ERROR_IF(fixedBlockColorData_.at(pos));
			fixedBlockColorData_.at(pos) = tet.color();
		}
	}

	tet.update(nullptr);
	fallingTetriminoData_.posX = fallingTetriminoData_.posY = 0;
}

bool Board::canFallingTetriminoPlaced(int posX, int posY) const
{
	THROW_ERROR_UNLESS(isValid());
	return !(getFixedBlockColorDataAtFallingTetriminoPos(posX, posY) & fallingTetriminoData_.tetrimino.bits());
}

bool Board::canFallingTetriminoPlaced() const
{
	THROW_ERROR_UNLESS(isValid());

	return canFallingTetriminoPlaced(fallingTetriminoData_.posX, fallingTetriminoData_.posY);
}

bool Board::isFallingTetriminoBlockHere(int x, int y) const
{
	THROW_ERROR_UNLESS(isValid());
	THROW_ERROR_UNLESS(x >= 0 && y >= 0 && x < BOARD_WIDTH && y < BOARD_HEIGHT);
	
	auto& data = fallingTetriminoData_;
	if(!(x >= data.posX && y >= data.posY && x < data.posX + data.tetrimino.width() && y < data.posY + data.tetrimino.height()))
		return false;
	return (data.tetrimino.bits() & 1 << ((y - data.posY) * data.tetrimino.width() + (x - data.posX))) != 0;
}

int Board::getFallingTetriminoBlockColor() const
{
	THROW_ERROR_UNLESS(isValid());

	return fallingTetriminoData_.tetrimino.color();
}

const boost::optional<int>& Board::getFixedBlockColor(int x, int y) const
{
	THROW_ERROR_UNLESS(x >= 0 && y >= 0 && x < BOARD_WIDTH && y < BOARD_HEIGHT);
	return fixedBlockColorData_.at(y * BOARD_WIDTH + x);
}

int Board::getDistanceToShadowFromFallingTetrimino() const
{
	THROW_ERROR_UNLESS(isValid());

	int x = fallingTetriminoData_.posX, y = fallingTetriminoData_.posY, cnt = 0;
	while(canFallingTetriminoPlaced(x, y + 1)){
		y++;
		cnt++;
	}
	return cnt;
}

bool Board::isFullStacked() const
{
	THROW_ERROR_IF(isValid());

	for(int x = 0;x < BOARD_WIDTH;x++)
		if(fixedBlockColorData_.at(x))	return true;
	return false;
}

bool Board::updateFallingTetrimino(const TetriminoCore *newTetrimino)
{
	auto& tet = fallingTetriminoData_.tetrimino;

	if(isValid())	fixFallingTetrimino();

	THROW_ERROR_UNLESS(BOARD_WIDTH >= newTetrimino->width());
	THROW_ERROR_UNLESS(BOARD_HEIGHT >= newTetrimino->height());
	fallingTetriminoData_.posX = (BOARD_WIDTH - newTetrimino->width()) / 2;
	fallingTetriminoData_.posY = 0;
	tet.update(newTetrimino);

	moveUp();

	for(int y = 0;y < fallingTetriminoData_.posY + tet.height();y++)
		for(int x = fallingTetriminoData_.posX;x < fallingTetriminoData_.posX + tet.width();x++)
			if(isFallingTetriminoBlockHere(x, y) && fixedBlockColorData_.at(y * BOARD_WIDTH + x))
				return false;
	return true;
}

bool Board::listClearingBlockLines(std::vector<int>& clearingLines) const
{
	auto& tet = fallingTetriminoData_.tetrimino;

	THROW_ERROR_IF(isValid());

	for(int y = 0;y < BOARD_HEIGHT;y++){
		bool isFull = true;
		for(int x = 0;x < BOARD_WIDTH;x++){
			if(!fixedBlockColorData_.at(y * BOARD_WIDTH + x))	isFull = false;
			if(isFull)	break;
		}
		if(isFull)	clearingLines.push_back(y);
	}

	return clearingLines.size() != 0;
}

void Board::clearLines(const std::vector<int>& clearingLines)
{
	THROW_ERROR_IF(isValid());

	// Too inefficient way but easy to read
	for(int i = 0;i < clearingLines.size();i++){
		fixedBlockColorData_.erase(fixedBlockColorData_.begin() + clearingLines.at(i) - i);
	}
	fixedBlockColorData_.insert(fixedBlockColorData_.begin(), clearingLines.size(), boost::optional<int>());
}

void Board::clearLines()
{
	THROW_ERROR_IF(isValid());

	std::vector<int> clearingLines;
	if(listClearingBlockLines(clearingLines) == 0)	return;
	clearLines(clearingLines);
}

void Board::fix()
{
	THROW_ERROR_UNLESS(isValid());

	while(moveDown())	;
	fixFallingTetrimino();
}

bool Board::canMove(int toPosX, int toPosY) const
{
	THROW_ERROR_UNLESS(isValid());

	int distanceX = std::abs(toPosX - fallingTetriminoData_.posX), distanceY = std::abs(toPosY - fallingTetriminoData_.posY);
	if(!((distanceX == 0 && distanceY == 1) || (distanceX == 1 && distanceY == 0)))	return false;
	if(canFallingTetriminoPlaced(toPosX, toPosY))	return true;
	return false;
}

bool Board::moveUp()
{
	THROW_ERROR_UNLESS(isValid());

	if(!canMove(fallingTetriminoData_.posX, fallingTetriminoData_.posY - 1))	return false;
	fallingTetriminoData_.posY--;
	return true;
}

bool Board::moveDown()
{
	THROW_ERROR_UNLESS(isValid());

	if(!canMove(fallingTetriminoData_.posX, fallingTetriminoData_.posY + 1))	return false;
	fallingTetriminoData_.posY++;
	return true;
}

bool Board::moveLeft()
{
	THROW_ERROR_UNLESS(isValid());

	if(!canMove(fallingTetriminoData_.posX - 1, fallingTetriminoData_.posY))	return false;
	fallingTetriminoData_.posX--;
	return true;
}

bool Board::moveRight()
{
	THROW_ERROR_UNLESS(isValid());

	if(!canMove(fallingTetriminoData_.posX + 1, fallingTetriminoData_.posY))	return false;
	fallingTetriminoData_.posX++;
	return true;
}

bool Board::rotateToLeft()
{
	THROW_ERROR_UNLESS(isValid());

	fallingTetriminoData_.tetrimino.rotateToLeft();
	if(canFallingTetriminoPlaced())	return true;
	fallingTetriminoData_.tetrimino.rotateToRight();
	return false;
}

bool Board::rotateToRight()
{
	THROW_ERROR_UNLESS(isValid());

	fallingTetriminoData_.tetrimino.rotateToRight();
	if(canFallingTetriminoPlaced())	return true;
	fallingTetriminoData_.tetrimino.rotateToLeft();
	return false;
}

///

TetriminoManager::TetriminoManager()
{
	loadTetriminoes();
}

TetriminoManager& TetriminoManager::getInstance()
{
	static TetriminoManager instance;
	return instance;
}

void TetriminoManager::loadTetriminoes()
{
	// I-Tetrimino
	addTetrimino(1, 4, 4, {
		{ 0, 0, 0, 0,
		  0, 0, 0, 0,
		  1, 1, 1, 1,
		  0, 0, 0, 0 },
		{ 0, 0, 1, 0,
		  0, 0, 1, 0,
		  0, 0, 1, 0,
		  0, 0, 1, 0 },
		{ 0, 0, 0, 0,
		  0, 0, 0, 0,
		  1, 1, 1, 1,
		  0, 0, 0, 0 },
		{ 0, 0, 1, 0,
		  0, 0, 1, 0,
		  0, 0, 1, 0,
		  0, 0, 1, 0 }
	});

	// O-Tetrimino
	addTetrimino(2, 2, 2, {
		{ 1, 1,
		  1, 1 },
		{ 1, 1,
		  1, 1 },
		{ 1, 1,
		  1, 1 },
		{ 1, 1,
		  1, 1 }
	});

	// S-Tetrimino
	addTetrimino(3, 3, 3, {
		{ 0, 0, 0,
		  0, 1, 1,
		  1, 1, 0 },
		{ 1, 0, 0,
		  1, 1, 0,
		  0, 1, 0 },
		{ 0, 0, 0,
		  0, 1, 1,
		  1, 1, 0 },
		{ 1, 0, 0,
		  1, 1, 0,
		  0, 1, 0 }
	});

	// Z-Tetrimino
	addTetrimino(4, 3, 3, {
		{ 0, 0, 0,
		  1, 1, 0,
		  0, 1, 1 },
		{ 0, 1, 0,
		  1, 1, 0,
		  1, 0, 0 },
		{ 0, 0, 0,
		  1, 1, 0,
		  0, 1, 1 },
		{ 0, 1, 0,
		  1, 1, 0,
		  1, 0, 0 }
	});

	// J-Tetrimino
	addTetrimino(5, 3, 3, {
		{ 0, 0, 0,
		  1, 0, 0,
		  1, 1, 1 },
		{ 1, 1, 0,
		  1, 0, 0,
		  1, 0, 0 },
		{ 0, 0, 0,
		  1, 1, 1,
		  0, 0, 1 },
		{ 0, 1, 0,
		  0, 1, 0,
		  1, 1, 0 }
	});

	// L-Tetrimino
	addTetrimino(6, 3, 3, {
		{ 0, 0, 0,
		  0, 0, 1,
		  1, 1, 1 },
		{ 1, 0, 0,
		  1, 0, 0,
		  1, 1, 0 },
		{ 0, 0, 0,
		  1, 1, 1,
		  1, 0, 0 },
		{ 1, 1, 0,
		  0, 1, 0,
		  0, 1, 0 }
	});

	// T-Tetrimino
	addTetrimino(7, 3, 3, {
		{ 0, 1, 0,
		  1, 1, 1,
		  0, 0, 0 },
		{ 0, 1, 0,
		  0, 1, 1,
		  0, 1, 0 },
		{ 0, 0, 0,
		  1, 1, 1,
		  0, 1, 0 },
		{ 0, 1, 0,
		  1, 1, 0,
		  0, 1, 0 }
	});
}

void TetriminoManager::addTetrimino(int color, int width, int height, const std::vector<std::vector<int>>& data)
{
	THROW_ERROR_UNLESS(width * height <= 64);

	std::vector<BlockBitMap> maps;
	for(auto& raw : data){
		THROW_ERROR_UNLESS(raw.size() == width * height);
		BlockBitMap bits = 0;
		for(int i = 0;i < raw.size();i++){
			if(raw.at(i))	bits |= 1 << i;
		}
		maps.push_back(bits);
	}

	tetriminoes_.push_back(boost::shared_ptr<TetriminoCore>(new TetriminoCore(color, width, height, maps)));
}

const TetriminoCore *TetriminoManager::getTetrimino(int index) const
{
	return tetriminoes_.at(index).get();
}

int TetriminoManager::getTetriminoNumber() const
{
	return tetriminoes_.size();
}

void TetriminoManager::clearTetriminoes()
{
	tetriminoes_.clear();
}

}

