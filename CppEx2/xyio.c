#include "xyio.h"

void setcursor(int x, int y) {
	//Uchwyt do konsoli
	HANDLE ekran = GetStdHandle(STD_OUTPUT_HANDLE);

	//Zmienna z pozycj¹ z parametrów
	COORD punkt = { (SHORT)x, (SHORT)y };

	//Ustawienie kursora na danym ekranie w danej pozycji
	SetConsoleCursorPosition(ekran, punkt);
}

void getcursor(int* x, int* y) {
	//Uchwyt do konsoli
	HANDLE ekran = GetStdHandle(STD_OUTPUT_HANDLE);

	//Rekord struktury jako bufor na informacje o ekranie
	CONSOLE_SCREEN_BUFFER_INFO bufor;

	//Pobranie informacji do bufora
	GetConsoleScreenBufferInfo(ekran, &bufor);

	//Wpisanie wartoœci do zmiennych
	*x = bufor.dwCursorPosition.X;
	*y = bufor.dwCursorPosition.Y;
}

void getrange(int* maxx, int* maxy) {
	//Utworzenie uchwytu i pobranie informacji do bufora
	HANDLE ekran = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bufor;
	GetConsoleScreenBufferInfo(ekran, &bufor);

	//Wpisanie wartoœci do zmiennych
	*maxx = bufor.dwMaximumWindowSize.X;
	*maxy = bufor.dwMaximumWindowSize.Y;
}

void getscreen(int* xorg, int* yorg, int* xend, int* yend) {
	//Utworzenie uchwytu i pobranie informacji do bufora
	HANDLE ekran = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bufor;
	GetConsoleScreenBufferInfo(ekran, &bufor);

	//Wpisanie wartoœci do zmiennych
	*xorg = bufor.srWindow.Left;
	*yorg = bufor.srWindow.Top;
	*xend = bufor.srWindow.Right;
	*yend = bufor.srWindow.Bottom;
}

int xyscanf(int x, int y, const char* format, ...) {
	//Uchwyt do klawiatury
	HANDLE klawiatura = GetStdHandle(STD_INPUT_HANDLE);

	//Variadyczna lista argumentów <cstdarg>
	va_list argumenty;

	//Rozpoczêcie przetwarzania listy wariadycznej
	//Drugim arugmentem jest ostatni znany argument przed list¹
	va_start(argumenty, format);

	//Ustawienie pozycji kursora
	setcursor(x, y);

	//Odczytanie z klawiatury tekstu o max d³ugoœci 150
	//Wraz z zapisem ilkoœci pobranych znaków
	DWORD znaki;
	CHAR tekst[150];
	BOOL sukces = ReadConsole(klawiatura, tekst, 150, &znaki, NULL);

	int rezultat;
	if (!sukces) rezultat = EOF;
	else {
		//Obciêcie koñcowych znaków \n i \r 
		while (znaki > 0 && (tekst[znaki - 1] == '\n' || tekst[znaki - 1] == '\r')) znaki--;

		//Doklejenie na koñcu koniec ³añcucha znaków z C
		tekst[znaki] = '\0';

		//Analiza odczytanego tekstu
		rezultat = vsscanf(tekst, format, argumenty);
	}

	//Koniec przetwarzania listy wariadycznej
	va_end(argumenty);

	return rezultat;
}

int xyprintf(int x, int y, const char* format, ...) {
	//Utworzenie uchwytu
	HANDLE ekran = GetStdHandle(STD_OUTPUT_HANDLE);

	//Variadyczna lista argumentów <cstdarg>
	va_list argumenty;
	va_start(argumenty, format);

	//Drukowanie do bufora tekstowego wg formatu z listy
	int rezultat;
	CHAR tekst[150];
	rezultat = vsprintf(tekst, format, argumenty);

	//Przy poprawnym wydrukowaniu
	if (rezultat >= 0) {
		DWORD znaki = (DWORD)rezultat;

		//Ustawienie kursora
		setcursor(x, y);

		//Wydrukowanie iloœci znaków tekstu na ekranie z bufora 
		BOOL sukces = WriteConsole(ekran, tekst, znaki, NULL, NULL);

		//Przy b³êdzie drukowania przestaw rezultat
		if (!sukces) rezultat = EOF;
	}

	//Koniec przetwarzania listy wariadycznej
	va_end(argumenty);

	return rezultat;
}

void clear(void) {
	//Utworzenie uchwytu i pobranie informacji do bufora
	HANDLE ekran = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bufor;
	GetConsoleScreenBufferInfo(ekran, &bufor);

	//Definiuje licznik znaków
	DWORD licznik;

	//Obliczam iloœæ znaków potrzebnych do wpisania w celu wyczyszczenia konsoli
	DWORD dlugosc = bufor.dwSize.X * bufor.dwSize.Y;

	//Wpisanie znaków w konsoli i ustawienie pozycji na {0,0}
	COORD pozycja = { 0,0 };
	FillConsoleOutputCharacter(ekran, ' ', dlugosc, pozycja, &licznik);
	SetConsoleCursorPosition(ekran, pozycja);
}

#if defined(_MSC_VER) && (_MSC_VER < 1800)
static int vsscanf(const char* buffer, const char* format, va_list argPtr)
{
	void* a[10];
	int i;
	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++)
		a[i] = va_arg(argPtr, void*);
	return sscanf(buffer, format, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
}
#endif