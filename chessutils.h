#ifndef CHESSUTILS
#define CHESSUTILS

bool isWhitePiece(uint8_t piece);
bool hasMoved(uint8_t piece);
unsigned long lrand();
int getPieceValue(uint8_t piece);
uint8_t getPieceFromBoard(uint8_t board[8][8], int x, int y);
void setPieceOnBoard(uint8_t board[8][8], int x, int y, uint8_t piece);
std::vector<std::pair<int, int>> getPossibleMoves(uint8_t board[8][8], int xfrom, int yfrom);

#endif
