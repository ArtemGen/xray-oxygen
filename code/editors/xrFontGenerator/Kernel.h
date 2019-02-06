/*
  ******************OXYGEN DEV TEAM (C) 2019**************************
  ******************    Author: Phantom1020 (samber13 - ��� ���������� ������ �� ����� � �� �������, ����� �����������)    **************************
*/

#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
 
// @ �� �� �����������, ������� ����� ��� ����������� � ��������, � ����� ��� ��������� �������� (�����, ������)
constexpr unsigned int TOTAL_ANSCII = 96 + 63 + 1;

namespace XRay
{
	// @ Contains information about glyph or symbol in texture
	struct data_symbol
	{
		size_t x[1], y[1];
		size_t x_off, y_off;
		int advance;
	};
	
	struct PathList
	{
		u32 FontSize;

		xr_string PathName;
		xr_string FileName;

		xr_string PathOutName;
		xr_string FileOutName;
	};

	struct ConvInfo
	{
		FT_Library lib;
		FT_Face    face;

		unsigned int   TexWid = 1; // @ �� ������� ��� � ����, ��� ������ �������� �� ���
		unsigned int   TexHeig = 0;
		unsigned int   FontHeig = 0;
					   
		unsigned int   PenX = 0;
		unsigned int   PenY = 0;
		unsigned char* Pixels;

		bool bHaveTexconv; // @ ��� ��������, ����� �� ���� � ����� texconv ��� ���;
	};
	
	extern HANDLE hConsole;

	class CFontGen
	{
		data_symbol info[TOTAL_ANSCII];
		data_symbol info_copy = { 0 }; 
		ConvInfo ConverterInfo;
	public:
		static bool bSucFile;
		static bool bSucDir;
		PathList PathSystem;

	public:
		CFontGen();
		~CFontGen() = default;

		void ParseFont(int index, int max_value);
		void ManageCreationFile(void);
		void CreateFolder(void);

		// @ Creates already with _size_800, _size_1024, _size_1600
		void InitFont();
		void CreateGSCFonts();
		void CheckTexConv();
		void InitFreeType();
		void ReleaseFreeType();
	};
}