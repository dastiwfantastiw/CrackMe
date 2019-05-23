#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include "md5.h"

BYTE email[40];
BYTE pass[40];

DWORD email_len = 0;
DWORD pass_len = 0;

INT byte_res = 1337;

CONST CHAR key[14] = "\x6b\x61\x24\x70\x65\x72\x24\x6b\x79\x5f\x31\x61\x38";

using namespace std;


int main()
{
	do
	{
		HANDLE hNamedPipe = CreateFileA("\\\\.\\pipe\\MyPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hNamedPipe != INVALID_HANDLE_VALUE)
		{

			DWORD Written = 0;

			ZeroMemory(&email, 40);
			ZeroMemory(&pass, 40);

			do
			{ 
			ReadFile(hNamedPipe, email, 40, &Written, 0);
			email_len = Written;
			}
			while (email_len == 0);


			do{
			ReadFile(hNamedPipe, pass, 40, &Written, 0);
			pass_len = Written;
			}
			while (pass_len == 0);

			string hash_key = md5(key);
			string email_str = (CHAR*)email;
			string pass_str = (CHAR*)pass;

			for (int i = 0; i < email_str.length(); i++)
			{
				if (email_str[i] >= 'A' && email_str[i] <= 'Z')
				{
					WriteFile(hNamedPipe, "\xAB", 1, &Written, 0);
					return -1;
				}
				email_str[i] = (CHAR)((0xFF - (INT)email_str[i] + 3) >> 2);
			}
			string hash_email = md5(email_str);
			string hash_pass = md5(pass_str);

			string hash_result = md5(hash_email + hash_key);

			for (int i = 0; i < pass_len; i++)
			{
				if ((UINT)pass_str[i] == (UINT)hash_result[i])
					byte_res ^= 0xFF - (BYTE)i * 3;
				else
				{
					byte_res = 0xFF;
					break;
				}
			}
			WriteFile(hNamedPipe, &byte_res, 1, &Written, 0);

			CloseHandle(hNamedPipe);
		}
	} while (1);
    return 0;
}
