#include "fxlib.h"
#include "revolution.h"
#include <stdio.h>
#include "src/game_lib.h"	// Will include the rest

#define STATE_MENU	0
#define STATE_GAME	1

unsigned char state = 0;
unsigned char *vram = GetVRAMAddress();

void menu_banner()
{
	unsigned char i = 0;
	unsigned char s = 0;
	for(i = 0; i < 6; i++)
	{
		s+=i;
		DrawLine(s, i, 127-s, i, vram, 1);
	}


}

void menu_drawname(unsigned char y, FONTCHARACTER *name)
{
	unsigned char i = 0, r = 1;
	char *shortName = FONTCHARACTER[17];

	while(i < 23 && r == 1)
	{
		shortName[i] = name[i+7];
		if(name[i+8] == '.' && name[i+9] == 'z')
		{
			r = 0;		// stop iterating
		}
		i++;
	}

	CoolText((128-(i*3+(i-1)*1)), 11 + 11 * y, FONT_FIVE, shortName, vram);
}

void menu()
{
	// Draw banner
	menu_banner();

	// Find some files named *.zoldo in storage
	FONTCHARACTER *pathname = {'\\','\\','f','l','s','0','\\','*','.','z','o','l','d','o', 0};
	int findHandle;
	FONTCHARACTER *foundfile = FONTCHARACTER[30];	// 17 more than pathname
	FILE_INFO fileinfo;

	int err = 0;

	err = Bfile_FindFirst(pathname, &findHandle, foundfile, &fileinfo);

	if(err == 0 && fileinfo.type == DT_FILE)	// Is it usable?
	{

	}
}

// The program starts here
int AddIn_main(int isAppli, unsigned short OptionNum)
{
	if(state == 0)
	{
		menu();
	}
}

//****************************************************************************
//**************                                              ****************
//**************                 Notice!                      ****************
//**************                                              ****************
//**************  Please do not change the following source.  ****************
//**************                                              ****************
//****************************************************************************


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section


#pragma section _TOP

//****************************************************************************
//  InitializeSystem
//
//  param   :   isAppli   : 1 = Application / 0 = eActivity
//              OptionNum : Option Number (only eActivity)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************
int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section
