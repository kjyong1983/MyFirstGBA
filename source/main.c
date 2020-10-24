
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <pcx.h>
#include <splash_pcx.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------------
// storage space for palette data
//---------------------------------------------------------------------------------
u16 PaletteBuffer[256];

unsigned int frame;


// test functions
void testCharacters();
void testFunctions();

// practical functions
void UpdateInputs();
void VblankInterrupt();
void splashScreen();

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------

int main(void) {
	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();

	irqSet(IRQ_VBLANK, VblankInterrupt);
	
	irqEnable(IRQ_VBLANK);

	// Allow Interrupts
	REG_IME = 1;

	splashScreen();

	consoleDemoInit();

	// testFunctions();

	// scroll




	while (1) {
		VBlankIntrWait();
		scanKeys();
		UpdateInputs();
	}
}

void VblankInterrupt()
//---------------------------------------------------------------------------------
{
	frame += 1;
	scanKeys();
}

void testCharacters(){

	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 25; j+=6)
		{
			iprintf("\x1b[%02d;%02dH%02d/%02d \n", i, j, i, j);
		}
	}
}

void testFunctions(){
	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	iprintf("\x1b[10;10HHello World!\n");
	testCharacters();
}

void splashScreen(){
	SetMode( MODE_4 | BG2_ON );		// screen mode & background to display

	DecodePCX(splash_pcx, (u16*)VRAM , PaletteBuffer);

	FadeToPalette( PaletteBuffer, 60);

	// TODO: need delay function
	FadeToPalette( PaletteBuffer, 60);

	// fill screen palette buffer
	for (size_t i = 0; i < 256; i++)
	{
		PaletteBuffer[i] = 0;
	}

	// 00 black
	// 15 red

	FadeToPalette( PaletteBuffer, 100);
}

void UpdateInputs(){

	int keys_pressed, keys_released;

	keys_pressed = keysDown();
	keys_released = keysUp();

	if ( keys_pressed & KEY_A ) {
		iprintf("\x1b[0;0HA\n");
	}

	if ( keys_released & KEY_A ) {
		iprintf("\x1b[0;0H \n");
	}

	if ( keys_pressed & KEY_B ) {
		iprintf("\x1b[0;0HB\n");
	}

	if ( keys_released & KEY_B)
	{
		iprintf("\x1b[0;0H \n");
	}
	
	if ( keys_pressed & KEY_SELECT ) {
		iprintf("\x1b[0;0HSELECT\n");
	}

	if ( keys_released & KEY_SELECT ) {
		iprintf("\x1b[0;0H      \n");
	}

	if ( keys_pressed & KEY_START ) {
		iprintf("\x1b[0;0HSTART\n");
	}

	if ( keys_released & KEY_START ) {
		iprintf("\x1b[0;0H     \n");
	}

	if ( keys_pressed & KEY_RIGHT ) {
		iprintf("\x1b[0;0HRIGHT\n");
	}

	if ( keys_released & KEY_RIGHT ) {
		iprintf("\x1b[0;0H     \n");
	}

	if ( keys_pressed & KEY_LEFT ) {
		iprintf("\x1b[0;0HLEFT\n");
	}

	if ( keys_released & KEY_LEFT ) {
		iprintf("\x1b[0;0H    \n");
	}

	if ( keys_pressed & KEY_UP ) {
		iprintf("\x1b[0;0HUP\n");
	}

	if ( keys_released & KEY_UP ) {
		iprintf("\x1b[0;0H  \n");
	}

	if ( keys_pressed & KEY_DOWN ) {
		iprintf("\x1b[0;0HDOWN\n");
	}

	if ( keys_released & KEY_DOWN ) {
		iprintf("\x1b[0;0H    \n");
	}

	if ( keys_pressed & KEY_R ) {
		iprintf("\x1b[0;0HR\n");
	}

	if ( keys_released & KEY_R ) {
		iprintf("\x1b[0;0H \n");
	}

	if ( keys_pressed & KEY_L ) {
		iprintf("\x1b[0;0HL\n");
	}

	if ( keys_released & KEY_L ) {
		iprintf("\x1b[0;0H \n");
	}
}
