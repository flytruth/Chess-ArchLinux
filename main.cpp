#include <raylib.h>
#include <cstdint>
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <cstring>
#include "chessutils.h"
#include "LRUCache.hpp"

#define SEARCH_DEPTH 4
#define TRANS_TABLE_SIZE 32000

void drawBoard() {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      Color c;
      if ((x + y) % 2 == 0) {
        c = (Color) {255, 255, 255, 255};
      }
      else {
        c = (Color) {100, 100, 100, 255};
      }
      DrawRectangle(x * 90, y * 90, 90, 90, c);
    }
  }
}

Texture2D loadImageAsTexture(const char* path) {
  Image image = LoadImage(path);
  if (!IsImageValid(image)) {
    printf("invalid image\n");
    exit(1);
  }
  Texture2D texture = LoadTextureFromImage(image);
  if (!IsTextureValid(texture)) {
    printf("invalid texture\n");
    exit(1);
  }
  UnloadImage(image);
  return texture;
}

struct BoardData {
  uint8_t board[8][8];
  int eval;
  unsigned long hash;
};

unsigned long zobristTable[64][12];
unsigned long zobristWhiteTurn;
LRUCache<unsigned long, int> transposTable(TRANS_TABLE_SIZE);

void initHashFunc() {
  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 12; j++) {
      unsigned long r = lrand();
      zobristTable[i][j] = r;
    }
  }
  zobristWhiteTurn = lrand();
}

void hashBoard(BoardData *board, bool isWhiteTurn) {
  unsigned long hash = 0;
  if (isWhiteTurn) {
    hash = hash ^ zobristWhiteTurn;
  }

  for (int i = 0; i < 64; i++) {
    uint8_t piece = getPieceFromBoard(board->board, i & 0b111, i >> 3);
    if (piece != 0b10000000) {
      hash = hash ^ zobristTable[i][piece & 0b1111];
    }
  }

  board->hash = hash;
}

void movePiece(struct BoardData *board, int xfrom, int yfrom, int xto, int yto) {
  uint8_t piece = getPieceFromBoard(board->board, xfrom, yfrom);
  uint8_t oldPiece = getPieceFromBoard(board->board, xto, yto);
  setPieceOnBoard(board->board, xfrom, yfrom, 0b10000000);
  setPieceOnBoard(board->board, xto, yto, piece | 0b00010000);
  if (isWhitePiece(oldPiece)) {
    board->eval += getPieceValue(oldPiece);
  } else {
    board->eval -= getPieceValue(oldPiece);
  }
  board->hash = board->hash ^ zobristTable[(yfrom << 3) | xfrom][piece & 0b1111];
  if (oldPiece != 0b10000000) {
    board->hash = board->hash ^ zobristTable[(yto << 3) | xto][oldPiece & 0b1111];
  }
  board->hash = board->hash ^ zobristTable[(yto << 3) | xto][piece & 0b1111];
}


int evalBoard(BoardData *board) {
  int eval = 0;
  for (int i = 0; i < 64; i++) {
    uint8_t piece = getPieceFromBoard(board->board, i & 0b111, i >> 3);
    int pieceValue = getPieceValue(piece);
    if (isWhitePiece(piece)) {
      eval -= pieceValue;
    } else {
      eval += pieceValue;
    }
  }
  board->eval = eval - 5000;
  return eval - 5000;
}

struct MoveData {
  int xfrom;
  int yfrom;
  int xto;
  int yto;
};

std::vector<MoveData> getAllMoves(uint8_t board[8][8], bool white) {
  std::vector<MoveData> moves;
  for (int i = 0; i < 64; i++) {
    int x = i & 0b111;
    int y = i >> 3;
    uint8_t piece = getPieceFromBoard(board, x, y);
    if (piece == 0b10000000) {
      continue;
    }
    if (isWhitePiece(piece) == white) {
      for (std::pair<int, int> move : getPossibleMoves(board, x, y)) {
        moves.push_back({x, y, move.first, move.second});
      }
    }
  }
  return moves;
}

int minimax(BoardData *board, int depth, int alpha, int beta, bool maximizing) {
  if (depth == 0) {
    return board->eval;
  }

  int hashedEval;
  bool hashFound = transposTable.get(board->hash, hashedEval);
  if (hashFound) {
    return hashedEval;
  }

  if (maximizing) {
    int val = -2147000000;
    for (MoveData move : getAllMoves(board->board, false)) {
      BoardData newBoard;
      memcpy(newBoard.board, board->board, sizeof(uint8_t) * 64);
      newBoard.eval = board->eval;
      newBoard.hash = board->hash;
      newBoard.hash ^= zobristWhiteTurn;
      movePiece(&newBoard, move.xfrom, move.yfrom, move.xto, move.yto);
      int val1 = minimax(&newBoard, depth - 1, alpha, beta, false);
      if (val1 > val) {
        val = val1;
      }
      if (val >= beta) {
        break;
      }
      if (val > alpha) {
        alpha = val;
      }
    }
    transposTable.set(board->hash, val);
    return val;
  }
  else {
    int val = 2147000000;
    for (MoveData move : getAllMoves(board->board, true)) {
      BoardData newBoard;
      memcpy(newBoard.board, board->board, sizeof(uint8_t) * 64);
      newBoard.eval = board->eval;
      newBoard.hash = board->hash;
      movePiece(&newBoard, move.xfrom, move.yfrom, move.xto, move.yto);
      newBoard.hash ^= zobristWhiteTurn;
      int val1 = minimax(&newBoard, depth - 1, alpha, beta, true);
      if (val1 < val) {
        val = val1;
      }
      if (val <= alpha) {
        break;
      }
      if (val < beta) {
        beta = val;
      }
    }
    transposTable.set(board->hash, val);
    return val;
  }
}

MoveData getBestMove(BoardData *board, int depth) {
  MoveData bestMove = {0, 0, 0, 0};
  int bestEval = -2147000000;
  for (MoveData move : getAllMoves(board->board, false)) {
    BoardData newBoard;
    memcpy(newBoard.board, board->board, sizeof(uint8_t) * 64);
    newBoard.eval = board->eval;
    newBoard.hash = board->hash;
    movePiece(&newBoard, move.xfrom, move.yfrom, move.xto, move.yto);
    int eval = minimax(&newBoard, depth, -2147000000, 2147000000, false);
    if (eval > bestEval) {
      bestEval = eval;
      bestMove = move;
    }
  }
  return bestMove;
}

void loadTextures(Texture2D &whitePawnTexture, Texture2D &blackPawnTexture, Texture2D &whiteKnightTexture, Texture2D &blackKnightTexture,
		Texture2D &whiteBishopTexture, Texture2D &blackBishopTexture, Texture2D &whiteRookTexture, Texture2D &blackRookTexture,
		Texture2D &whiteQueenTexture, Texture2D &blackQueenTexture, Texture2D &whiteKingTexture, Texture2D &blackKingTexture) {
  whitePawnTexture = loadImageAsTexture("assets/white-pawn.png");
  blackPawnTexture = loadImageAsTexture("assets/black-pawn.png");
  whiteKnightTexture = loadImageAsTexture("assets/white-knight.png");
  blackKnightTexture = loadImageAsTexture("assets/black-knight.png");
  whiteBishopTexture = loadImageAsTexture("assets/white-bishop.png");
  blackBishopTexture = loadImageAsTexture("assets/black-bishop.png");
  whiteRookTexture = loadImageAsTexture("assets/white-rook.png");
  blackRookTexture = loadImageAsTexture("assets/black-rook.png");
  whiteQueenTexture = loadImageAsTexture("assets/white-queen.png");
  blackQueenTexture = loadImageAsTexture("assets/black-queen.png");
  whiteKingTexture = loadImageAsTexture("assets/white-king.png");
  blackKingTexture = loadImageAsTexture("assets/black-king.png");
}

void renderPiece(uint8_t piece, float x, float y, Texture2D &whitePawnTexture, Texture2D &blackPawnTexture, Texture2D &whiteKnightTexture, Texture2D &blackKnightTexture,
		Texture2D &whiteBishopTexture, Texture2D &blackBishopTexture, Texture2D &whiteRookTexture, Texture2D &blackRookTexture,
		Texture2D &whiteQueenTexture, Texture2D &blackQueenTexture, Texture2D &whiteKingTexture, Texture2D &blackKingTexture) {
  if (piece == 0b10000000) {
    return;
  }
  if ((piece & 0b00001111) == 0b00000000) {
    DrawTextureEx(whitePawnTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00000001) {
    DrawTextureEx(blackPawnTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00000010) {
    DrawTextureEx(whiteKnightTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00000011) {
    DrawTextureEx(blackKnightTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00000100) {
    DrawTextureEx(whiteBishopTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00000101) {
    DrawTextureEx(blackBishopTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00000110) {
    DrawTextureEx(whiteRookTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00000111) {
    DrawTextureEx(blackRookTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00001000) {
    DrawTextureEx(whiteQueenTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00001001) {
    DrawTextureEx(blackQueenTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00001010) {
    DrawTextureEx(whiteKingTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
  if ((piece & 0b00001111) == 0b00001011) {
    DrawTextureEx(blackKingTexture, (Vector2) {x, y}, 0, 0.703125, WHITE);
    return;
  }
}

int main() {
  const int w = 900;
  const int h = 720;

  BoardData board = {{
    {0b00000111, 0b00000011, 0b00000101, 0b00001011, 0b00001001, 0b00000101, 0b00000011, 0b00000111},
    {0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000001},
    {0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000},
    {0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000},
    {0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000},
    {0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000},
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},
    {0b00000110, 0b00000010, 0b00000100, 0b00001010, 0b00001000, 0b00000100, 0b00000010, 0b00000110}
  }, 0};
  initHashFunc();
  int startHoldX = -1;
  int startHoldY = -1;
  int offsetX = -1;
  int offsetY = -1;
  uint8_t pieceHeld = 0b10000000;
  int cureval = 0;
  char evalText[32] = "board eval: 0";
  std::vector<std::pair<int, int>> possibleMoves;

  InitWindow(w, h, "chess");

  Texture2D whitePawnTexture;     // 00000000
  Texture2D blackPawnTexture;     // 00000001
  Texture2D whiteKnightTexture;   // 00000010
  Texture2D blackKnightTexture;   // 00000011
  Texture2D whiteBishopTexture;   // 00000100
  Texture2D blackBishopTexture;   // 00000101
  Texture2D whiteRookTexture;     // 00000110
  Texture2D blackRookTexture;     // 00000111
  Texture2D whiteQueenTexture;    // 00001000
  Texture2D blackQueenTexture;    // 00001001
  Texture2D whiteKingTexture;     // 00001010
  Texture2D blackKingTexture;     // 00001011
  // empty - 10000000 
  loadTextures(whitePawnTexture, blackPawnTexture, whiteKnightTexture, blackKnightTexture, whiteBishopTexture, blackBishopTexture, whiteRookTexture, blackRookTexture,
		  whiteQueenTexture, blackQueenTexture, whiteKingTexture, blackKingTexture);

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing();
    drawBoard();
    DrawRectangle(720, 0, 180, 720, BLACK);
    DrawText(evalText, 720, 0, 18, WHITE);

    for (int y = 0; y < 8; y ++) {
      for (int x = 0; x < 8; x ++) {
        uint8_t piece = getPieceFromBoard(board.board, x, y);
        renderPiece(piece, x * 90.0f, y * 90.0f, whitePawnTexture, blackPawnTexture, whiteKnightTexture, blackKnightTexture, whiteBishopTexture, blackBishopTexture,
			whiteRookTexture, blackRookTexture, whiteQueenTexture, blackQueenTexture, whiteKingTexture, blackKingTexture);
      }
    }

    if (IsMouseButtonPressed(0)) {
      int mouseX = GetMouseX();
      int mouseY = GetMouseY();
      startHoldX = mouseX / 90;
      startHoldY = mouseY / 90;
      pieceHeld = getPieceFromBoard(board.board, startHoldX, startHoldY);
      if (isWhitePiece(pieceHeld)) {
        possibleMoves = getPossibleMoves(board.board, startHoldX, startHoldY);
	setPieceOnBoard(board.board, startHoldX, startHoldY, 0b10000000);
        offsetX = mouseX - startHoldX * 90;
        offsetY = mouseY - startHoldY * 90;
      }
      else {
        pieceHeld = 0b10000000;
      }
    }

    if (IsMouseButtonReleased(0) && pieceHeld != 0b10000000) {
      int x = GetMouseX() / 90;
      int y = GetMouseY() / 90;
      bool isAllowed = false;
      for (std::pair<int, int> move : possibleMoves) {
        if (move.first == x && move.second == y) {
	  isAllowed = true;
	  break;
	}
      }
      if (isAllowed) {
        setPieceOnBoard(board.board, x, y, pieceHeld | 0b00010000);
	evalBoard(&board);
	hashBoard(&board, false);
	MoveData bestMove = getBestMove(&board, SEARCH_DEPTH);
	movePiece(&board, bestMove.xfrom, bestMove.yfrom, bestMove.xto, bestMove.yto);
	cureval = evalBoard(&board);
	sprintf(evalText, "board eval: %d", cureval);
      }
      else {
        setPieceOnBoard(board.board, startHoldX, startHoldY, pieceHeld);
      }
    }

    if (IsMouseButtonDown(0) && pieceHeld != 0b10000000) {
      for (std::pair<int, int> move : possibleMoves) {
        DrawCircle(move.first * 90 + 45, move.second * 90 + 45, 10, LIGHTGRAY);
      }

      int x = GetMouseX() - offsetX;
      int y = GetMouseY() - offsetY;
      renderPiece(pieceHeld, x, y, whitePawnTexture, blackPawnTexture, whiteKnightTexture, blackKnightTexture, whiteBishopTexture, blackBishopTexture,
			whiteRookTexture, blackRookTexture, whiteQueenTexture, blackQueenTexture, whiteKingTexture, blackKingTexture);
    }

    EndDrawing();
  }
  CloseWindow();

  return 0;
}
