#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <time.h>
#include "ConstParams.h"
#include <windowsx.h>
#include "resource.h"
#include <conio.h>

typedef struct CurrentGameParams 
{
	int fieldHeight;
	int fieldWidth;
	int cols;			
	int rows;			
	int minesCount;		
} CurrentGameParams;

CurrentGameParams cGP;	

char currTime[6];
int **gameField = NULL;	
int **btnID = NULL;		
HWND **btnGameField = NULL;

int minesWasFounded = 0;
int flagsPlaced = 0;
int gameTimeCounter = 0;
bool playAgain = false;
int turn = 0;
bool playerWin = false;

UINT_PTR IDT_TIMER = 1;
UINT_PTR IDT_GAME_TIMER = 2;
HWND restartBtn;
HWND gameTime;
HWND flagsLeft;
HWND smallFieldBtn, mediumFieldBtn, largeFieldBtn;
HMENU menuBar;
HINSTANCE hInst;
WNDPROC OldButtonProc;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	srand(time(NULL));

	WNDCLASSEX wcex; HWND hWnd; MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "MineSweeperClass";
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hInst = hInstance;

	hWnd = CreateWindow("MineSweeperClass", "Сапер", WS_SYSMENU, CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SetTimer(hWnd, IDT_GAME_TIMER, 200, NULL);

	MoveWindow(hWnd, 100, 100, 340, 400, NULL);

	CreateGameMenu(hWnd);

	BOOL bRet;

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			MessageBox(NULL, (LPCSTR)"Ошибка", NULL, MB_OK);
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK SubWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hWnd, WM_RBUTTONDOWN, OnRButtonClick);
	}
	return CallWindowProc(OldButtonProc, hWnd, message, wParam, lParam);
}

static void OnRButtonClick(HWND hWnd, BOOL bDblClick, int x, int y, UINT uKey) 
{
	char *buff = new char[3];
	bool brSrc = false;
	for (int i = 1; i < cGP.rows + 1; i++)
	{
		for (int j = 1; j < cGP.cols + 1; j++)
		{
			if (hWnd == btnGameField[i][j])
			{
				if (flagsPlaced < cGP.minesCount)
				{
					PlaceFlag(hWnd, i, j);
					_itoa(cGP.minesCount - flagsPlaced, buff, 10);
					if (buff[2] != '\0')
						buff[2] = '\0';
					FillEdit(flagsLeft, buff);
				}
				if (flagsPlaced == minesWasFounded && minesWasFounded == cGP.minesCount)
				{
					PlayerWin(hWnd);
				}
				UpdateWindow(hWnd);
				return;
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool needBreak = false;
	RECT windowRect;
	switch (message)
	{
	case WM_DESTROY:
		for (int i = 1; i < cGP.rows + 1; i++)
		{
			for (int j = 0; j < cGP.cols + 1; j++)
			{
				SetWindowLongPtr(btnGameField[i][j], GWLP_WNDPROC, (LPARAM)OldButtonProc);
			}
		}
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		if (wParam == IDT_TIMER)
		{
			ConvertTime(gameTimeCounter++);
			FillEdit(gameTime, currTime);
			return 0;
		}
		if (wParam == IDT_GAME_TIMER)
		{
			if (playerWin)
			{
				playerWin = false;
				KillTimer(hWnd, IDT_TIMER);
				if ((MessageBox(NULL, "Вы Победили!", "Победа!", MB_OKCANCEL)) == IDOK)
				{
					RestartGame(hWnd);
					return 0;
				}
			}
		}
	case WM_PAINT:		

		break;
	case WM_COMMAND:
		switch LOWORD(wParam)
		{
		case BTN_RESTART:
			RestartGame(hWnd);
			break;
		case MENU_SMALL_FIELD:
			LoadGame(hWnd, SMALL_FIELD_COLS, SMALL_FIELD_ROWS, SMALL_FIELD_HEIGHT, SMALL_FIELD_WIDTH,
				SMALL_FIELD_MINES_COUNT);
			break;
		case MENU_MEDIUM_FIELD:
			LoadGame(hWnd, MEDIUM_FIELD_COLS, MEDIUM_FIELD_ROWS, MEDIUM_FIELD_HEIGHT, MEDIUM_FIELD_WIDTH,
				MEDIUM_FIELD_MINES_COUNT);
			break;
		case MENU_LARGE_FIELD:
			LoadGame(hWnd, LARGE_FIELD_COLS, LARGE_FIELD_ROWS, LARGE_FIELD_HEIGHT, LARGE_FIELD_WIDTH,
				LARGE_FIELD_MINES_COUNT);
			break;
		default:
			bool needBreak = false;
			for (int i = 0; i < cGP.rows + 1; i++)
			{
				for (int j = 0; j < cGP.cols + 1; j++)
				{
					if (LOWORD(wParam) == btnID[i][j] && gameField[i][j] < 200)
					{
						if (gameField[i][j] == 9)
						{
							GameOver(hWnd);
							return 1;
						}
						OpenCell(hWnd, i, j);
						CheckCell(hWnd, i, j);
						DrawCells(hWnd);
						return 1;
					}
				}
			}
			return 1;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void FillEdit(HWND hWnd, char *text)
{
	Edit_SetText(hWnd, text);
}

void ConvertTime(int seconds)
{
	char *tmpVal = new char[1];
	int divider = 600;
	for (int i = 0; i < 2; i++)
	{
		_itoa(seconds / divider, tmpVal, 10);
		divider = divider / 10;
		currTime[i] = tmpVal[0];
	}
	currTime[2] = ':';
	if (seconds >= 60)
	{
		_itoa((seconds % 60) / 10, tmpVal, 10);
		currTime[3] = tmpVal[0];
		_itoa((seconds % 60) % 10, tmpVal, 10);
		currTime[4] = tmpVal[0];
	}
	else
	{
		_itoa(seconds / 10, tmpVal, 10);
		currTime[3] = tmpVal[0];
		_itoa(seconds % 10, tmpVal, 10);
		currTime[4] = tmpVal[0];
	}
	currTime[5] = '\0';
}

void LoadGame(HWND hWnd, int cols, int rows, int height, int width, int mines)
{
	KillTimer(hWnd, IDT_TIMER);
	SetTimer(hWnd, IDT_TIMER, 1000, NULL);
	gameTimeCounter = 0;
	RECT windowRect;
	cGP.minesCount = mines;
	UpdateGameField(cols, rows);
	if (turn != 0)
	{
		ResetField(cGP.cols, cGP.rows);
	}
	else
	{
		DestroyWindow(smallFieldBtn);
		DestroyWindow(mediumFieldBtn);
		DestroyWindow(largeFieldBtn);
		ResetField(cols, rows);
	}
	turn++;
	MoveWindow(hWnd, 100, 100, width, height, NULL);
	CreateGameField(hWnd, CELL_SIZE, height, width, cols, rows);
	DestroyMenu(menuBar);
	DestroyWindow(restartBtn);
	DestroyWindow(flagsLeft);
	DestroyWindow(gameTime);
	CreateGameMenu(hWnd);
	cGP.fieldHeight = height;
	cGP.fieldWidth = width;
	cGP.cols = cols;
	cGP.rows = rows;
	GetClientRect(hWnd, &windowRect);
	InvalidateRect(hWnd, &windowRect, NULL);
	UpdateWindow(hWnd);
}

void UpdateGameField(int cols, int rows)
{
	InitGameField(cols, rows);
	PlaceMines(cols, rows);
}

void DrawCells(HWND hWnd)
{
	HDC hDC = GetDC(hWnd);
	POINT pt;
	pt.x = 10;
	pt.y = 40;
	for (int i = 0; i < cGP.rows + 1; i++)
	{
		MoveToEx(hDC, pt.x, pt.y, NULL);
		LineTo(hDC, pt.x, CELL_SIZE * cGP.rows + 40);
		pt.x += CELL_SIZE;
	}
	pt.x = 10;
	pt.y = 40;
	for (int i = 0; i < cGP.cols + 1; i++)
	{
		MoveToEx(hDC, pt.x, pt.y, NULL);
		LineTo(hDC, cGP.cols * CELL_SIZE + 10, pt.y);
		pt.y += CELL_SIZE;
	}
	ReleaseDC(hWnd, hDC);
}

void InitGameField(int cols, int rows)
{
	if (gameField != NULL)
	{
		for (int i = 0; i < cGP.rows + 1; i++)
		{
			delete[] gameField[i];
		}
		delete[] gameField;
	}
	gameField = new int*[rows + 2];
	for (int i = 0; i < rows + 2; i++)
	{
		gameField[i] = new int[cols + 2];
	}
	for (int i = 1; i < rows + 1; i++)
	{
		for (int j = 1; j < cols + 1; j++)
		{
			gameField[i][j] = 0;
		}
	}
	for (int i = 0; i < cols + 1; i++)
	{
		gameField[i][0] = -3;
		gameField[i][cols + 1] = -3;
	}
	for (int i = 0; i < rows + 1; i++)
	{
		gameField[0][i] = -3;
		gameField[rows + 1][i] = -3;
	}
}

void CheckCell(HWND hWnd, int row, int col)
{
	if (gameField[row][col] >= 100)
	{
		if (gameField[row][col] != 109)
		{
			if (gameField[row][col] != NULL)
			{
				DestroyWindow(btnGameField[row][col]);
			}
		}
		else
		{
			GameOver(hWnd);
		}
		if (gameField[row][col] >= 101 && gameField[row][col] <= 108)
		{
			if (gameField[row][col] != NULL)
			{
				HDC hDC;
				PAINTSTRUCT ps;
				DestroyWindow(btnGameField[row][col]);
				hDC = BeginPaint(hWnd, &ps);
				char nMines[2];
				_itoa(gameField[row][col] - 100, nMines, 10);
				nMines[1] = '\0';
				gameField[row][col] -= 100;
				TextOut(hDC, 20 + (col - 1) * 30, 47 + (row - 1) * 30, nMines, 1);
				EndPaint(hWnd, &ps);
				ReleaseDC(hWnd, hDC);
			}
		}
	}
}

void PlaceFlag(HWND hWnd, int row, int col)
{
	if (gameField[row][col] <= 9)
	{
		flagsPlaced++;

		HANDLE btnBmp = LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP,
			30, 30, LR_DEFAULTCOLOR);
		
		if (btnBmp)
		{
			SendMessage(btnGameField[row][col], BM_SETIMAGE, IMAGE_BITMAP, LPARAM(btnBmp));
		}

		if (gameField[row][col] == 9)
		{
			minesWasFounded++;
		}
		gameField[row][col] += 200;
	}
	else
		if (gameField[row][col] >= 200)
		{
			flagsPlaced--;
			gameField[row][col] -= 200;
			if (gameField[row][col] == 9)
			{
				minesWasFounded--;
			}
			SendMessage(btnGameField[row][col], BM_SETIMAGE, IMAGE_BITMAP, LPARAM(NULL));
		}
}

void OpenCell(HWND hWnd, int posI, int posJ) 
{
	if (gameField[posI][posJ] == 0)
	{
		gameField[posI][posJ] = 100;
		CheckCell(hWnd, posI, posJ);
		OpenCell(hWnd, posI, posJ - 1);
		OpenCell(hWnd, posI - 1, posJ);
		OpenCell(hWnd, posI, posJ + 1);
		OpenCell(hWnd, posI + 1, posJ);
		OpenCell(hWnd, posI - 1, posJ - 1);
		OpenCell(hWnd, posI - 1, posJ + 1);
		OpenCell(hWnd, posI + 1, posJ - 1);
		OpenCell(hWnd, posI + 1, posJ + 1);
	}
	else
		if (gameField[posI][posJ] < 100 && gameField[posI][posJ] != -3)
		{
			gameField[posI][posJ] += 100;
			CheckCell(hWnd, posI, posJ);
		}
}

void PlaceMines(int cols, int rows) 
{
	int minesPlaced = 0;
	int minesNearby = 0;
	while (minesPlaced < cGP.minesCount)
	{
		int i = rand() % cols + 1;
		int j = rand() % rows + 1;
		if (gameField[i][j] != 9)
		{
			gameField[i][j] = 9;
			minesPlaced++;
		}
	}
	for (int i = 1; i < rows + 1; i++)
	{
		for (int j = 1; j < cols + 1; j++)
		{
			if (gameField[i][j] != 9)
			{
				minesNearby = 0;
				if (gameField[i - 1][j - 1] == 9)
					minesNearby++;
				if (gameField[i - 1][j] == 9)
					minesNearby++;
				if (gameField[i - 1][j + 1] == 9)
					minesNearby++;
				if (gameField[i][j - 1] == 9)
					minesNearby++;
				if (gameField[i][j + 1] == 9)
					minesNearby++;
				if (gameField[i + 1][j - 1] == 9)
					minesNearby++;
				if (gameField[i + 1][j] == 9)
					minesNearby++;
				if (gameField[i + 1][j + 1] == 9)
					minesNearby++;
				gameField[i][j] = minesNearby;
			}
		}
	}
	flagsPlaced = 0;
	minesWasFounded = 0;
}

void ResetField(int cols, int rows)
{
	if (btnID != NULL || btnGameField != NULL)
	{
		for (int i = 1; i < rows + 1; i++)
		{
			for (int j = 1; j < cols + 1; j++)
			{
				DestroyWindow(btnGameField[i][j]);
			}
		}
		for (int i = 0; i < rows + 2; i++)
		{
			delete[] btnGameField[i];
			delete[] btnID[i];
		}
		delete[] btnID;
		delete[] btnGameField;
	}
}

void CreateGameField(HWND hWnd, int cellSize, int fieldHeight, int fieldWidth, int cols, int rows)
{
	UpdateWindow(hWnd);
	int index = 4000;
	int currentCoordX = 10, currentCoordY = 40;
	btnID = new int*[rows + 2];
	btnGameField = new HWND*[rows + 2];
	for (int i = 0; i < rows + 2; i++)
	{
		btnID[i] = new int[cols + 2];
		btnGameField[i] = new HWND[cols + 2];
	}
	for (int i = 1; i < rows + 1; i++)
	{
		for (int j = 1; j < cols + 1; j++)
		{
			btnID[i][j] = index + 2;
			index += 2;
			btnGameField[i][j] = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_BITMAP,
				currentCoordX, currentCoordY, cellSize, cellSize, hWnd, (HMENU)btnID[i][j], hInst, NULL);
			if (i == 1 && j == 1)
			{
				OldButtonProc = (WNDPROC)GetWindowLongPtr(btnGameField[i][j], GWLP_WNDPROC);
			}
			SetWindowLongPtr(btnGameField[i][j], GWLP_WNDPROC, (LPARAM)SubWndProc);
			currentCoordX += cellSize;
		}
		currentCoordX = 10;
		currentCoordY += cellSize;
	}
	for (int i = 0; i < cols + 1; i++)
	{
		btnGameField[i][0] = NULL;
		btnGameField[i][cols + 1] = NULL;
		btnID[i][0] = NULL;
		btnID[i][cols + 1] = NULL;
	}
	for (int i = 0; i < rows + 1; i++)
	{
		btnGameField[0][i] = NULL;
		btnGameField[rows + 1][i] = NULL;
		btnID[0][i] = NULL;
		btnID[rows + 1][i] = NULL;
	}
	UpdateWindow(hWnd);
}

void PlayerWin(HWND hWnd)
{
	int a;
	for (int i = 1; i < cGP.rows + 1; i++)
	{
		for (int j = 1; j < cGP.rows + 1; j++)
		{
			EnableWindow(btnGameField[i][j], FALSE);
		}
	}
	playerWin = true;
}

void GameOver(HWND hWnd)
{
	for (int i = 1; i < cGP.rows + 1; i++)
	{
		for (int j = 1; j < cGP.rows + 1; j++)
		{
			if (gameField[i][j] == 9)
			{
				HANDLE btnBmp = LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP,
					30, 30, LR_DEFAULTCOLOR);
				if (btnBmp)
				{
					SendMessage(btnGameField[i][j], BM_SETIMAGE, IMAGE_BITMAP, LPARAM(btnBmp));
				}
			}
			EnableWindow(btnGameField[i][j], FALSE);
		}
	}
	UpdateGameField(cGP.cols, cGP.rows);
	KillTimer(hWnd, IDT_TIMER);
	gameTimeCounter = 0;
}

void RestartGame(HWND hWnd)
{	
	KillTimer(hWnd, IDT_TIMER);
	SetTimer(hWnd, IDT_TIMER, 1000, NULL);
	gameTimeCounter = 0;
	RECT windowRect;
	for (int i = 1; i < cGP.rows + 1; i++)
	{
		for (int j = 1; j < cGP.cols + 1; j++)
		{
			EnableWindow(btnGameField[i][j], TRUE);
		}
	}
	minesWasFounded = 0;
	flagsPlaced = 0;
	UpdateGameField(cGP.cols, cGP.rows);
	ResetField(cGP.cols, cGP.rows);
	CreateGameField(hWnd, CELL_SIZE, cGP.fieldHeight, cGP.fieldWidth, cGP.cols, cGP.rows);
	DestroyMenu(menuBar);
	DestroyWindow(restartBtn);
	DestroyWindow(flagsLeft);
	DestroyWindow(gameTime);
	CreateGameMenu(hWnd);
	GetClientRect(hWnd, &windowRect);
	InvalidateRect(hWnd, &windowRect, NULL);
	UpdateWindow(hWnd);
}

void CreateGameMenu(HWND hWnd)
{
	HMENU menuFieldSize;
	RECT windowRect;
	menuBar = CreateMenu();
	menuFieldSize = CreateMenu();
	AppendMenu(menuFieldSize, MF_STRING, MENU_SMALL_FIELD, "Маленькое (10 x 10)");
	AppendMenu(menuFieldSize, MF_STRING, MENU_MEDIUM_FIELD, "Средне (15 x 15)");
	AppendMenu(menuFieldSize, MF_STRING, MENU_LARGE_FIELD, "Большое (20 x 20)");
	AppendMenu(menuBar, MF_POPUP, (UINT_PTR)menuFieldSize, "Поле");
	SetMenu(hWnd, menuBar);
	GetClientRect(hWnd, &windowRect);
	if (turn != 0)
	{
		restartBtn = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_BITMAP,
			windowRect.right / 2 - 15, 5, CELL_SIZE, CELL_SIZE, hWnd, (HMENU)BTN_RESTART, hInst, NULL);
		HANDLE btnBmp = LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP,
			30, 30, LR_DEFAULTCOLOR);
		if (btnBmp)
		{
			SendMessage(restartBtn, BM_SETIMAGE, IMAGE_BITMAP, LPARAM(btnBmp));
		}
		gameTime = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
			windowRect.left + 10, windowRect.top + 10, 50, 22, hWnd, NULL, hInst, NULL);
		flagsLeft = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
			windowRect.right - 50, windowRect.top + 10, 30, 22, hWnd, NULL, hInst, NULL);
		EnableWindow(gameTime, FALSE);
		EnableWindow(flagsLeft, FALSE);
	}
	else
	{
		smallFieldBtn = CreateWindow("BUTTON", "Маленькое игровое поле (10 х 10)", WS_CHILD | WS_VISIBLE | 
			BS_DEFPUSHBUTTON, windowRect.left + 10, windowRect.top + 10, windowRect.right - windowRect.left - 20,
			75, hWnd, (HMENU)MENU_SMALL_FIELD, hInst, NULL);
		mediumFieldBtn = CreateWindow("BUTTON", "Среднее игровое поле (15 х 15)", WS_CHILD | WS_VISIBLE |
			BS_DEFPUSHBUTTON, windowRect.left + 10, windowRect.top + 95, windowRect.right - windowRect.left - 20,
			75, hWnd, (HMENU)MENU_MEDIUM_FIELD, hInst, NULL);
		largeFieldBtn = CreateWindow("BUTTON", "Большое игровое поле (20 х 20)", WS_CHILD | WS_VISIBLE |
			BS_DEFPUSHBUTTON, windowRect.left + 10, windowRect.top + 180, windowRect.right - windowRect.left - 20,
			75, hWnd, (HMENU)MENU_LARGE_FIELD, hInst, NULL);
	}
	UpdateWindow(hWnd);
}