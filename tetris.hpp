#pragma once
#ifndef ___TETRIS_HPP___
#define ___TETRIS_HPP___

#include "hoolib.hpp"
#include <vector>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace Tetris
{

using BlockBitMap = HooLib::uint64;

class TetriminoCore;
class Tetrimino;
class Board;

class TetriminoCore
{
private:
	const int color_;
	const int width_, height_;
	const std::vector<BlockBitMap> blockMaps_;
public:
	TetriminoCore(int color, int width, int height, const std::vector<BlockBitMap>& blockMaps);

	inline int color() const;
	inline int width() const;
	inline int height() const;
	inline BlockBitMap bits(int index) const;
};

class Tetrimino
{
private:
	const TetriminoCore *tetriminoCoreData_;
	int rotationIndex_;
public:
	Tetrimino();

	inline bool isValid() const;
	inline int color() const;
	inline int width() const;
	inline int height() const;
	inline BlockBitMap bits() const;
inline void update(const TetriminoCore *tetriminoCoreData);
	inline void rotateToLeft();
	inline void rotateToRight();
};

class Board : private boost::noncopyable
{
public:
	const int BOARD_WIDTH, BOARD_HEIGHT;
private:
	struct {
		int posX, posY;
		Tetrimino tetrimino;
	} fallingTetriminoData_;
	std::vector<boost::optional<int>> fixedBlockColorData_;
private:
	bool isValid() const;
	BlockBitMap getFixedBlockColorDataAtFallingTetriminoPos(int posX, int posY) const;
	BlockBitMap getFixedBlockColorDataAtFallingTetriminoPos() const;
	void fixFallingTetrimino();
	bool canFallingTetriminoPlaced(int posX, int posY) const;
	bool canFallingTetriminoPlaced() const;

	/*
	template<class Prepare, class Cleanup>
	bool tryChangeFallingTetriminoData(Prepare prepare, Cleanup cleanup)
	{
		prepare();
		if(!(getBlockColorDataAtFallingTetriminoPos() & fallingTetriminoData_.tetrimino.bits()))	return true;
		cleanup();
		return false;
	}
	*/
public:
	Board(int width, int height);

	bool isFallingTetriminoBlockHere(int x, int y) const;
	int getFallingTetriminoBlockColor() const;
	const boost::optional<int>& getFixedBlockColor(int x, int y) const;
	int getDistanceToShadowFromFallingTetrimino() const;

	bool isFullStacked() const;
	bool updateFallingTetrimino(const TetriminoCore *newTetrimino);

	bool listClearingBlockLines(std::vector<int>& clearingLines) const;
	void clearLines(const std::vector<int>& clearingLines);
	void clearLines();

	// To operate the falling tetrimino
	void fix();
	bool canMove(int toPosX, int toPosY) const;
	bool moveUp();
	bool moveDown();
	bool moveLeft();
	bool moveRight();
	bool rotateToLeft();
	bool rotateToRight();
};

class TetriminoManager : private boost::noncopyable
{
private:
	std::vector<boost::shared_ptr<TetriminoCore>> tetriminoes_;
private:
	TetriminoManager();
	~TetriminoManager(){}

	void loadTetriminoes();
public:
	static TetriminoManager& getInstance();

	void addTetrimino(int color, int width, int height, const std::vector<std::vector<int>>& data);
	const TetriminoCore* getTetrimino(int index) const;
	int getTetriminoNumber() const;
	void clearTetriminoes();
};

}

#endif // ___TETRIS_HPP___

