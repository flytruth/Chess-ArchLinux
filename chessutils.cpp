#include <cstdlib>
#include <cstdint>
#include <vector>
#include "chessutils.h"

bool isWhitePiece(uint8_t piece) {
  return (piece & 0b00000001) == 0;
}

bool hasMoved(uint8_t piece) {
  return (piece & 0b00010000) == 0b00010000;
}

unsigned long lrand() {
  return (((long) rand()) << 32) | rand();
}

int getPieceValue(uint8_t piece) {
  if (piece == 0b10000000) {
    return 0;
  }
  if ((piece & 0b00001111) == 0b00000000 || (piece & 0b00001111) == 0b00000001) {
    return 10;
  }
  if ((piece & 0b00001111) == 0b00000010 || (piece & 0b00001111) == 0b00000011) {
    return 20;
  }
  if ((piece & 0b00001111) == 0b00000100 || (piece & 0b00001111) == 0b00000101) {
    return 30;
  }
  if ((piece & 0b00001111) == 0b00000110 || (piece & 0b00001111) == 0b00000111) {
    return 50;
  }
  if ((piece & 0b00001111) == 0b00001000 || (piece & 0b00001111) == 0b00001001) {
    return 90;
  }
  if ((piece & 0b00001111) == 0b00001010) {
    return 5000;
  }
  if ((piece & 0b00001111) == 0b00001011) {
    return 10000;
  }
  return 0;
}

uint8_t getPieceFromBoard(uint8_t board[8][8], int x, int y) {
  if (x > 7) {
    x = 7;
  }
  if (x < 0) {
    x = 0;
  }
  if (y > 7) {
    y = 7;
  }
  if (y < 0) {
    y = 0;
  }
  uint8_t piece = board[y][x];
  return piece;
}

void setPieceOnBoard(uint8_t board[8][8], int x, int y, uint8_t piece) {
  if (x > 7) {
    x = 7;
  }
  if (x < 0) {
    x = 0;
  }
  if (y > 7) {
    y = 7;
  }
  if (y < 0) {
    y = 0;
  }
  board[y][x] = piece;
}

std::vector<std::pair<int, int>> getPossibleMoves(uint8_t board[8][8], int xfrom, int yfrom) {
  std::vector<std::pair<int, int>> moves;
  uint8_t piece = getPieceFromBoard(board, xfrom, yfrom);
  if ((piece & 0b00001111) == 0b00000000) { // white pawn
    bool free1 = yfrom - 1 >= 0 && getPieceFromBoard(board, xfrom, yfrom - 1) == 0b10000000;
    bool free2 = yfrom - 2 >= 0 && getPieceFromBoard(board, xfrom, yfrom - 2) == 0b10000000 && !hasMoved(piece);
    if (free1) {
      moves.push_back({xfrom, yfrom - 1}); // 1 square forward
    }
    if (free1 && free2) {
      moves.push_back({xfrom, yfrom - 2}); // 2 squares forward
    }
    if (xfrom - 1 >= 0 && yfrom - 1 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom - 1, yfrom - 1);
      if (pieceCapt != 0b10000000 && !isWhitePiece(pieceCapt)) {
        moves.push_back({xfrom - 1, yfrom - 1}); // capture to right
      }
    }
    if (xfrom + 1 < 8 && yfrom - 1 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom + 1, yfrom - 1);
      if (pieceCapt != 0b10000000 && !isWhitePiece(pieceCapt)) {
        moves.push_back({xfrom + 1, yfrom - 1}); // capture to left
      }
    }
  }
  else if ((piece & 0b00001111) == 0b00000001) { // black pawn
    bool free1 = yfrom + 1 < 8 && getPieceFromBoard(board, xfrom, yfrom + 1) == 0b10000000;
    bool free2 = yfrom + 2 < 8 && getPieceFromBoard(board, xfrom, yfrom + 2) == 0b10000000 && !hasMoved(piece);
    if (free1) {
      moves.push_back({xfrom, yfrom + 1}); // 1 square forward
    }
    if (free1 && free2) {
      moves.push_back({xfrom, yfrom + 2}); // 2 squares forward
    }
    if (xfrom - 1 >= 0 && yfrom + 1 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom - 1, yfrom + 1);
      if (pieceCapt != 0b10000000 && isWhitePiece(pieceCapt)) {
        moves.push_back({xfrom - 1, yfrom + 1}); // capture to right
      }
    }
    if (xfrom + 1 < 8 && yfrom + 1 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom + 1, yfrom + 1);
      if (pieceCapt != 0b10000000 && isWhitePiece(pieceCapt)) {
        moves.push_back({xfrom + 1, yfrom + 1}); // capture to left
      }
    }
  }
  else if ((piece & 0b00001111) == 0b00000010 || (piece & 0b00001111) == 0b00000011) { // knights
    if (xfrom - 2 >= 0 && yfrom - 1 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom - 2, yfrom - 1);
      if (pieceCapt == 0b10000000 || isWhitePiece(pieceCapt) != isWhitePiece(piece)) {
        moves.push_back({xfrom - 2, yfrom - 1});
      }
    }
    if (xfrom - 1 >= 0 && yfrom - 2 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom - 1, yfrom - 2);
      if (pieceCapt == 0b10000000 || isWhitePiece(pieceCapt) != isWhitePiece(piece)) {
        moves.push_back({xfrom - 1, yfrom - 2});
      }
    }
    if (xfrom - 2 >= 0 && yfrom + 1 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom - 2, yfrom + 1);
      if (pieceCapt == 0b10000000 || isWhitePiece(pieceCapt) != isWhitePiece(piece)) {
        moves.push_back({xfrom - 2, yfrom + 1});
      }
    }
    if (xfrom - 1 >= 0 && yfrom + 2 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom - 1, yfrom + 2);
      if (pieceCapt == 0b10000000 || isWhitePiece(pieceCapt) != isWhitePiece(piece)) {
        moves.push_back({xfrom - 1, yfrom + 2});
      }
    }
    if (xfrom + 2 < 8 && yfrom - 1 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom + 2, yfrom - 1);
      if (pieceCapt == 0b10000000 || isWhitePiece(pieceCapt) != isWhitePiece(piece)) {
        moves.push_back({xfrom + 2, yfrom - 1});
      }
    }
    if (xfrom + 1 < 8 && yfrom - 2 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom + 1, yfrom - 2);
      if (pieceCapt == 0b10000000 || isWhitePiece(pieceCapt) != isWhitePiece(piece)) {
        moves.push_back({xfrom + 1, yfrom - 2});
      }
    }
    if (xfrom + 2 < 8 && yfrom + 1 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom + 2, yfrom + 1);
      if (pieceCapt == 0b10000000 || isWhitePiece(pieceCapt) != isWhitePiece(piece)) {
        moves.push_back({xfrom + 2, yfrom + 1});
      }
    }
    if (xfrom + 1 < 8 && yfrom + 2 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom + 1, yfrom + 2);
      if (pieceCapt == 0b10000000 || isWhitePiece(pieceCapt) != isWhitePiece(piece)) {
        moves.push_back({xfrom + 1, yfrom + 2});
      }
    }
  }
  else if ((piece & 0b00001111) == 0b00000100 || (piece & 0b00001111) == 0b00000101) { // bishops
    for (int d = 1; d < 8; d ++) {
      if (xfrom + d >= 8 || yfrom + d >= 8) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom + d, yfrom + d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom + d, yfrom + d});
      	continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
      	moves.push_back({xfrom + d, yfrom + d});
      	break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (xfrom - d < 0 || yfrom + d >= 8) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom - d, yfrom + d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom - d, yfrom + d});
      	continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
	      moves.push_back({xfrom - d, yfrom + d});
	      break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (xfrom + d >= 8 || yfrom - d < 0) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom + d, yfrom - d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom + d, yfrom - d});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
      	moves.push_back({xfrom + d, yfrom - d});
	      break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (xfrom - d < 0 || yfrom - d < 0) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom - d, yfrom - d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom - d, yfrom - d});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
	      moves.push_back({xfrom - d, yfrom - d});
	      break;
      }
    }
  }
  else if ((piece & 0b00001111) == 0b00000110 || (piece & 0b00001111) == 0b00000111) { // rooks
    for (int d = 1; d < 8; d ++) {
      if (xfrom + d >= 8) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom + d, yfrom);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom + d, yfrom});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
	      moves.push_back({xfrom + d, yfrom});
	      break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (xfrom - d < 0) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom - d, yfrom);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom - d, yfrom});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
      	moves.push_back({xfrom - d, yfrom});
      	break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (yfrom + d >= 8) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom, yfrom + d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom, yfrom + d});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
	      moves.push_back({xfrom, yfrom + d});
	      break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (yfrom - d < 0) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom, yfrom - d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom, yfrom - d});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
      	moves.push_back({xfrom, yfrom - d});
	      break;
      }
    }
  }
  else if ((piece & 0b00001111) == 0b00001000 || (piece & 0b00001111) == 0b00001001) { // queens
    for (int d = 1; d < 8; d ++) {
      if (xfrom + d >= 8 || yfrom + d >= 8) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom + d, yfrom + d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom + d, yfrom + d});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
      	moves.push_back({xfrom + d, yfrom + d});
      	break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (xfrom - d < 0 || yfrom + d >= 8) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom - d, yfrom + d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom - d, yfrom + d});
      	continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
      	moves.push_back({xfrom - d, yfrom + d});
      	break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (xfrom + d >= 8 || yfrom - d < 0) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom + d, yfrom - d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom + d, yfrom - d});
      	continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
      	moves.push_back({xfrom + d, yfrom - d});
      	break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (xfrom - d < 0 || yfrom - d < 0) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom - d, yfrom - d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom - d, yfrom - d});
      	continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
      	moves.push_back({xfrom - d, yfrom - d});
      	break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (xfrom + d >= 8) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom + d, yfrom);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom + d, yfrom});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
	      moves.push_back({xfrom + d, yfrom});
      	break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (xfrom - d < 0) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom - d, yfrom);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom - d, yfrom});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
	      moves.push_back({xfrom - d, yfrom});
	      break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (yfrom + d >= 8) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom, yfrom + d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom, yfrom + d});
	      continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
	      moves.push_back({xfrom, yfrom + d});
	      break;
      }
    }
    for (int d = 1; d < 8; d ++) {
      if (yfrom - d < 0) {
        break;
      }
      uint8_t pieceOn = getPieceFromBoard(board, xfrom, yfrom - d);
      if (pieceOn == 0b10000000) {
        moves.push_back({xfrom, yfrom - d});
      	continue;
      }
      if (isWhitePiece(pieceOn) == isWhitePiece(piece)) {
        break;
      }
      if (isWhitePiece(pieceOn) != isWhitePiece(piece)) {
	      moves.push_back({xfrom, yfrom - d});
	      break;
      }
    }
  }
  else if ((piece & 0b00001111) == 0b00001010 || (piece & 0b00001111) == 0b00001011) { // kings
    if (xfrom - 1 >= 0 && yfrom - 1 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom - 1, yfrom - 1);
      if (isWhitePiece(pieceCapt) != isWhitePiece(piece) || pieceCapt == 0b10000000) {
        moves.push_back({xfrom - 1, yfrom - 1});
      }
    }
    if (xfrom - 1 >= 0 && yfrom + 1 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom - 1, yfrom + 1);
      if (isWhitePiece(pieceCapt) != isWhitePiece(piece) || pieceCapt == 0b10000000) {
        moves.push_back({xfrom - 1, yfrom + 1});
      }
    } 
    if (xfrom + 1 < 8 && yfrom - 1 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom + 1, yfrom - 1);
      if (isWhitePiece(pieceCapt) != isWhitePiece(piece) || pieceCapt == 0b10000000) {
        moves.push_back({xfrom + 1, yfrom - 1});
      }
    } 
    if (xfrom + 1 < 8 && yfrom + 1 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom + 1, yfrom + 1);
      if (isWhitePiece(pieceCapt) != isWhitePiece(piece) || pieceCapt == 0b10000000) {
        moves.push_back({xfrom + 1, yfrom + 1});
      }
    } 
    if (xfrom - 1 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom - 1, yfrom);
      if (isWhitePiece(pieceCapt) != isWhitePiece(piece) || pieceCapt == 0b10000000) {
        moves.push_back({xfrom - 1, yfrom});
      }
    }
    if (xfrom + 1 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom + 1, yfrom);
      if (isWhitePiece(pieceCapt) != isWhitePiece(piece) || pieceCapt == 0b10000000) {
        moves.push_back({xfrom + 1, yfrom});
      }
    } 
    if (yfrom - 1 >= 0) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom, yfrom - 1);
      if (isWhitePiece(pieceCapt) != isWhitePiece(piece) || pieceCapt == 0b10000000) {
        moves.push_back({xfrom, yfrom - 1});
      }
    } 
    if (yfrom + 1 < 8) {
      uint8_t pieceCapt = getPieceFromBoard(board, xfrom, yfrom + 1);
      if (isWhitePiece(pieceCapt) != isWhitePiece(piece) || pieceCapt == 0b10000000) {
        moves.push_back({xfrom, yfrom + 1});
      }
    }
  }
  return moves;
}

