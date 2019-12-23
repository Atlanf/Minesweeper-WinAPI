#pragma once

#define MENU_RESTART 1000
#define MENU_SMALL_FIELD 1001
#define MENU_MEDIUM_FIELD 1002
#define MENU_LARGE_FIELD 1003

#define BTN_RESTART 1010

#define EMPTY_CELL 1110
#define MINE_CELL 1111
#define OPENED_CELL 1112
#define FIELD_BORDER 9999

const int SMALL_FIELD_HEIGHT = 400;
const int SMALL_FIELD_WIDTH = 340;
const int SMALL_FIELD_ROWS = 10;
const int SMALL_FIELD_COLS = 10;
const int SMALL_FIELD_MINES_COUNT = 15;

const int MEDIUM_FIELD_HEIGHT = 550;
const int MEDIUM_FIELD_WIDTH = 490;
const int MEDIUM_FIELD_ROWS = 15;
const int MEDIUM_FIELD_COLS = 15;
const int MEDIUM_FIELD_MINES_COUNT = 25;

const int LARGE_FIELD_HEIGHT = 700;
const int LARGE_FIELD_WIDTH = 640;
const int LARGE_FIELD_ROWS = 20;
const int LARGE_FIELD_COLS = 20;
const int LARGE_FIELD_MINES_COUNT = 35;

const int CELL_SIZE = 30;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateGameField(int cols, int rows);
void InitGameField(int cols, int rows);
void CreateGameMenu(HWND hWnd);
void PlaceMines(int cols, int rows);
void GameOver(HWND hWnd);
void PlayerWin(HWND hWnd);
void OpenCell(HWND hWnd, int posI, int posJ);
void CreateGameField(HWND hWnd, int cellSize, int fieldHeight, int fieldWidth, int cols, int rows);
void ResetField(int cols, int rows);
void CheckCell(HWND hWnd, int row, int col);
void PlaceFlag(HWND hWnd, int row, int col);
static void OnRButtonClick(HWND hWnd, BOOL bDblClick, int x, int y, UINT uKey);
void DrawCells(HWND hWnd);
void RestartGame(HWND hWnd);
void ConvertTime(int seconds);
void LoadGame(HWND hWnd, int cols, int rows, int height, int width, int mines);
void FillEdit(HWND hWnd, char *text);
