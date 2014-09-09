#include "tetris.hpp"
#include <iostream>

void dump(const Tetris::Board& board)
{
	for(int y = 0;y < 20;y++){
		for(int x = 0;x < 10;x++){
			//if(board.isFallingTetriminoBlockHere(x, y))	std::cout << "F ";
			//else 
			if(board.getFixedBlockColor(x, y))	std::cout << "X ";
			else	std::cout << "* ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int main()
{
	Tetris::Board board(10, 21);
	while(!board.isFullStacked() && board.updateFallingTetrimino(Tetris::TetriminoManager::getInstance().getTetrimino(0))){
		//board.moveDown();
		//board.rotateToLeft();
		while(board.moveDown())	;
		board.fix();
		board.clearLines();
	}
	dump(board);
}

