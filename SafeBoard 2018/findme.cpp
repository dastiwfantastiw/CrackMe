
#include "stdafx.h"
#include <Windows.h>

VOID DecodeMessage(BYTE* buffer, BYTE* Msg, INT Size_m, BYTE* Key, INT Size_k)
{
	if (Size_m != Size_k)
		ExitProcess(0);

	for (INT i = 0; i < Size_m; i++)
	{
		Msg[i] ^= Key[i];
		buffer[i] = Msg[i];
	}

	buffer[Size_m] = '\n';
	buffer[Size_m + 1] = '\0';

}

int main()
{
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO info;
	if (!GetConsoleScreenBufferInfo(hStdout, &info))

	SetConsoleTitleA("Kaspersky Lab");
	SetConsoleTextAttribute(hStdout, 10);

	BYTE *KEY    = new BYTE[15] { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	BYTE* Hello  = new BYTE[15] { 0x54, 0x4c, 0x47, 0x21, 0x27, 0x46, 0x0e, 0xe7, 0xec, 0x8a, 0xd8, 0xa3, 0xb9, 0x8b, 0xc5 };
	BYTE buffer[20];
	DWORD dwCount = 0;

	ZeroMemory(&buffer, 20);

    DecodeMessage(buffer, Hello, 15, KEY, 15);
    
	WriteConsoleA(hStdout, &buffer, 16, 0, 0);
	ZeroMemory(&buffer, sizeof(buffer));

	SetConsoleTextAttribute(hStdout, 11);
	ReadConsoleA(hStdin, &buffer, sizeof(buffer), &dwCount, 0);

	VOID * MemFunc = VirtualAlloc(0, 63, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (MemFunc)
	{
		BYTE *shell = new BYTE[63]
		{ 
			0x8B, 0x44, 0x24, 0x04, 0x66, 0x83,
			0xF8, 0x0A, 0x75, 0x2C, 0x8B, 0x4C,
			0x24, 0x08, 0x81, 0x31, 0xDE, 0xC0,
			0xAD, 0xDE, 0x81, 0x39, 0xEF, 0x8A,
			0xE0, 0x8F, 0x75, 0x1A, 0x81, 0x41,
			0x04, 0x0D, 0xF0, 0xAD, 0xBA, 0x81,
			0x71, 0x04, 0xCE, 0xFA, 0xED, 0xFE,
			0xF7, 0x59, 0x04, 0x81, 0x79, 0x04,
			0x67, 0x38, 0xF0, 0x09, 0x74, 0x03,
			0x31, 0xC0, 0xC3, 0xB8, 0x01, 0x00,
			0x00, 0x00, 0xC3
		};
		DWORD result = 0;

		memcpy_s(MemFunc, 63, shell, 63);
		_asm
		{
			lea esi, [buffer]
			push esi
			push dwCount
			call MemFunc
			mov result, eax
		}
		VirtualFree(MemFunc, 63, MEM_RELEASE);
		if (result != 0)
		{
			SetConsoleTextAttribute(hStdout, 0xE);
			BYTE * Correct = new BYTE[15]{ 0x47, 0x43, 0x5f, 0x2d, 0x31, 0x46, 0x14, 0xe7, 0xfd, 0xcf, 0x9a, 0xcc, 0xdd, 0xee, 0xff };
			DecodeMessage(buffer, Correct, 15, KEY, 15);
			WriteConsoleA(hStdout, &buffer, 11, 0, 0);
			SetConsoleTextAttribute(hStdout, info.wAttributes);
		}
		else
		{
			SetConsoleTextAttribute(hStdout, 0xC);
			BYTE * Incorrect = new BYTE[15]{ 0x58, 0x4c ,0x45 ,0x25, 0x39, 0x0f, 0x13, 0xa8, 0xfa, 0xc5, 0xdf, 0xa9, 0xf3, 0xee, 0xff };
			DecodeMessage(buffer, Incorrect, 15, KEY, 15);
			WriteConsoleA(hStdout, &buffer, 13, 0, 0);
			SetConsoleTextAttribute(hStdout, info.wAttributes);
		}
		_gettch();
	}
	else
	{
		SetConsoleTextAttribute(hStdout, info.wAttributes);
		ExitProcess(0);
	}
	SetConsoleTextAttribute(hStdout, info.wAttributes);
	ExitProcess(0);
}
