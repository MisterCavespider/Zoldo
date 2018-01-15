#include "fxlib.h"
#include "revolution.h"
#include <stdio.h>
#include "src/game_lib.h"	// Will include the rest

#define STATE_MENU	0
#define STATE_GAME	1

#define zoldobanner_width 32
#define zoldobanner_height 8
static unsigned char zoldobanner_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xc3, 0x9d, 0xf1, 0xe7, 0xfb, 0x6d, 0xf6, 0xdb,
   0xe7, 0x6d, 0xf6, 0xdb, 0xdf, 0x6d, 0xf6, 0xdb, 0xc3, 0x9c, 0x31, 0xe7,
   0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 };

unsigned char state = 0;
unsigned char *vram;

void menu_banner()
{
	unsigned char i = 0;
	unsigned char s = 0;
	for(i = 0; i < 7; i++)
	{
		s+=i;
		DrawLine(s, i, 127-s, i, vram, BLACK);
	}

	DrawSprite(50, 0, 32, 8, 4, zoldobanner_bits, vram, NO_TRANS);


	// DrawLine(50, 1, 53, 1, vram, WHITE);
	// DrawLine(53, 2, 50, 4, vram, WHITE);
	// DrawLine(50, 5, 53, 5, vram, WHITE);
    //
	// DrawLine(57, 1, 58, 1, vram, WHITE);
	// DrawLine(59, 2, 59, 4, vram, WHITE);
	// DrawLine(58, 5, 57, 5, vram, WHITE);
	// DrawLine(56, 4, 56, 2, vram, WHITE);

}

void menu_drawname(unsigned char y, FONTCHARACTER *name)
{
	unsigned char i = 0;
	unsigned char r = 1;
	char sname[17];

	while(i < 23 && r == 1)
	{
		sname[i] = name[i];
		if(name[i+1] == '.' && name[i+2] == 'z')
		{
			r = 0;		// stop iterating
		}
		i++;
	}

	PrintMini((128-(i*3+(i-1)*1))/2, 11 + 11 * y, sname, MINI_OVER);
}

void menu_list()
{
	// Find some files named *.zoldo in storage
	FONTCHARACTER pathname[] = {'\\','\\','f','l','s','0','\\','*','.','z','o','l','d','o', 0};
	int findHandle;
	FONTCHARACTER foundfile[30];	// 17 more than pathname
	FILE_INFO fileinfo;

	int err = 0;

	err = Bfile_FindFirst(pathname, &findHandle, foundfile, &fileinfo);

	if(err == 0 && fileinfo.type == DT_FILE)	// Is it usable?
	{
		menu_drawname(0, foundfile);
	}
}

void menu()
{
	vram = (unsigned char *) GetVRAMAddress();

	menu_banner();
	menu_list();
}

// The program starts here
int AddIn_main(int isAppli, unsigned short OptionNum)
{
	unsigned int kc = 0;

	if(state == 0)
	{
		menu();
	}


	GetKey(&kc);
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
