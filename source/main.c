
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_base.h>

#include <pcx.h>
#include <splash_pcx.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


///////////////////////// simplebg scroll

// --------------------------------------------------------------------

#define MAPADDRESS		MAP_BASE_ADR(31)	// our base map address
#define DELAY			2			// slow things down
#define TILEWIDTH		8			// how much to scroll
#define ROW				10			// what row to place text at

// --------------------------------------------------------------------

const u16 sprite[] = {
	// RGB8(0x40,0x80,0xc0),
	// RGB8(0xFF,0xFF,0xFF),
	// RGB8(0xF5,0xFF,0xFF),
	// RGB8(0xDF,0xFF,0xF2),
	// RGB8(0xCA,0xFF,0xE2),
	// RGB8(0xB7,0xFD,0xD8),
	// RGB8(0x2C,0x4F,0x8B)

	RGB8(0x2C,0x4F,0xD8),
	RGB8(0x2C,0x4F,0xD8),
	RGB8(0x2C,0x4F,0xD8),
	RGB8(0xFF,0xFF,0xFF),
	RGB8(0x2C,0x4F,0xD8),
	RGB8(0x2C,0x4F,0xD8),
	RGB8(0x2C,0x4F,0xD8)
};


const u8 message[] = {
	"                                " \
	"Hello, this is an example of an oldschool simple tile scroller " \
	"not unlike how it was done in days of yore.  The '@' symbol " \
	"at the top of your screen is intentional, to dispel the illusion " \
	"of this scroller, to demonstrate the simple concept behind it. " \
	"Check out the source to learn how it works.  It is very simple! " \
	"This exercise brought to you by r6502...          " \
	"Text is about to restart... "
};



void updatescrolltext(u32 idx)
{
	u16 *temppointer;

	temppointer = (u16 *)MAPADDRESS + (ROW * 32);

	// write out a whole row of text to the map
	for(u32 i=0; i<32; i++)
	{
		// check for end of message so we can wrap around properly
		if(message[idx] == 0) idx = 0;

		// write a character - we subtract 32, because the font graphics
		// start at tile 0, but our text is in ascii (starting at 32 and up)
		// in other words, tile 0 is a space in our font, but in ascii a
		// space is 32 so we must account for that difference between the two.
		*temppointer++ = message[idx++] - 32;
	}
}


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

// initialize our variables
u32 scrollx = 0;
u32 textindex = 0;
u32 scrolldelay = 0;

void updateScroll(){
	// check if we reached our delay
	if(scrolldelay == DELAY) {
		// yes, the delay is complete, so let's reset it
		scrolldelay = 0;

		// check if we reached our scrollcount
		if(scrollx == (TILEWIDTH-1)) {
			// yes, we've scrolled enough, so let's reset the count
			scrollx = 0;

			// check if we reached the end of our scrolltext
			// and if so we need to restart our index
			if(message[textindex] == 0) textindex = 0;
			else textindex++;

			// finally, let's update the scrolltext with the current text index
			updatescrolltext(textindex);
		}
		else scrollx++;
	}
	else scrolldelay++;

	// update the hardware horizontal scroll register
	BG_OFFSET[0].x = scrollx;
}


u16* videoBuffer;
 
//

// const int SCREEN_WIDTH = 240
// const int SCREEN_HEIGHT = 160

int playerX = SCREEN_WIDTH / 2;
int playerY = SCREEN_HEIGHT / 2;

int playerSprWidth = 8;
int playerSprHeight = 8;

void PlotPixel(int x, int y, unsigned short int c)	
{ 
	videoBuffer[(y) *120 + (x)] = (c); 
}


void drawSprite(int x, int y){
	for (int y = 0; y < playerSprHeight; y++)
	{
		for (int x = 0; x < playerSprWidth; x++)
		{
			PlotPixel(x, y, 3);
		}
		
	}
	
}


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



	iprintf("\x1b[10;6H-Yingyeothon 21st-\n");
	iprintf("\x1b[11;8H-I am Yingyeo-\n");


	// // // scroll

	// // u32 i, scrollx, scrolldelay, textindex;
	// u16 *temppointer;

	// // // load the palette for the background, 7 colors
	// temppointer = BG_COLORS;
	// for(int i=0; i<7; i++) {
	// 	*temppointer++ = sprite[i];
	// }

	// // load the font into gba video mem (48 characters, 4bit tiles)
	// // CpuFastSet(r6502_portfont_bin, (u16*)VRAM,(r6502_portfont_bin_size/4) | COPY32);
	// // CpuFastSet( MAP_BASE_ADR(31), (u16*)VRAM, COPY32);

	// // clear screen map with tile 0 ('space' tile) (256x256 halfwords)

	// // *((u32 *)MAP_BASE_ADR(31)) =0;
	// // CpuFastSet( MAP_BASE_ADR(31), MAP_BASE_ADR(31), FILL | COPY32 | (0x800/4));

	// // set screen H and V scroll positions
	// BG_OFFSET[0].x = 0; BG_OFFSET[0].y = 0;



	// // put the '@' symbol on the top of the screen to show how
	// // the screen is only scrolling 7 pixels - to reveal the
	// // illusion of how the scroller works
	// *((u16 *)MAPADDRESS + 1) = 0x20;	// 0x20 == '@'

	// // set the screen base to 31 (0x600F800) and char base to 0 (0x6000000)
	// BGCTRL[0] = SCREEN_BASE(31);

	// // screen mode & background to display
	// SetMode( MODE_0 | BG0_ON );


	// SetMode( MODE_4 | BG2_ENABLE );


	// SetMode(MODE_3);

	while (1) {
		VBlankIntrWait();
		
		// updateScroll();
		
		drawSprite(playerX, playerY);

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
		playerX++;
	}

	if ( keys_released & KEY_RIGHT ) {
		iprintf("\x1b[0;0H     \n");
	}

	if ( keys_pressed & KEY_LEFT ) {
		iprintf("\x1b[0;0HLEFT\n");
		playerX--;
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
