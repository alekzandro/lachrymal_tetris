/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "header_file.h"  /* Declatations for these labs */


/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

int getbtns(void){

int returnValue = PORTD & 0xe0;
/*
  volatile int* ptr = (volatile int*) PORTD;
  returnValue = *ptr & 0xe0; 
  */
	returnValue = returnValue >> 5;

	return returnValue;
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08)); // while no underrun condition
	SPI2BUF = data;
	while(!(SPI2STAT & 1));    // while buffer is not full
	return SPI2BUF;
}

void display_update(void) {
  int i, j, k;
  int c;
  for(i = 0; i < 4; i++) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x22);
    spi_send_recv(i);
    
    spi_send_recv(0x0);
    spi_send_recv(0x10);
    
    DISPLAY_CHANGE_TO_DATA_MODE;
    
    for(j = 0; j < 16; j++) {
      c = textbuffer[i][j];
      if(c & 0x80)
        continue;
      
      //for(k = 0; k < 8; k++)
      // spi_send_recv(font[c*8 + k]);
    }
  }
}


void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_frame()
{
  int x = 66; 
  int y = 0;

while( y < 31 ){
  pixel_set(x, y);
  y++;
}

  while( x < 127 ){
    pixel_set(x, y);
    x++;
  }

while( y > 0 ){
  pixel_set(x, y);
  y--;
}

while( x > 65 ){
  pixel_set(x, y);
  x--;
}
}

void display_clear()
{

  int i, j;
  for(i = 0; i < 31; i++)
    for(j = 0; j < 127; j++)
      pixel_clr(j, i);


}



void display_print() {
    int i, j;
    int t = 0;

    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0 & 0xF);
        spi_send_recv(0x10 | ((0 >> 4) & 0xF));
        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = t; j <((i+1) * 128); j++){
            spi_send_recv(*(dis_ptr + j));
        }
        t +=128;
    }
}


void pixel_set(unsigned int x, unsigned int y)
{

if(y < 8)
  x = x;

else if(y > 7 && y < 16){
  x = x + 128;
  y = y - 8;
}

else if(y > 15 && y < 24 ){
  x =  x + 256;
  y = y - 16;
}

else if(y > 23){
  x =  x + 384;
  y = y - 24;
}

switch(y)
{
    case 0:
    *(dis_ptr + x) |= 0x01;
    break;

    case 1:
    *(dis_ptr + x) |= 0x02;
    break;

    case 2:
    *(dis_ptr + x) |= 0x04;
    break;

    case 3:
    *(dis_ptr + x) |= 0x08;
    break;

    case 4:
    *(dis_ptr + x) |= 0x10;
    break;

    case 5:
    *(dis_ptr + x) |= 0x20;
    break;

    case 6:
    *(dis_ptr + x) |= 0x40;
    break;

    case 7:
    *(dis_ptr + x) |= 0x80;
    break;
}

}

void pixel_clr(unsigned int x,unsigned int y)
{

if(y < 8)
  x = x;

else if(y > 7 && y < 16){
  x = x + 128;
  y = y - 8;
}

else if(y > 15 && y < 24 ){
  x =  x + 256;
  y = y - 16;
}

else if(y > 23){
  x =  x + 384;
  y = y - 24;
}

switch(y)
{
    case 0:
    *(dis_ptr + x) &= ~(0x01);
    break;

    case 1:
    *(dis_ptr + x) &= ~(0x02);
    break;

    case 2:
    *(dis_ptr + x) &= ~(0x04);
    break;

    case 3:
    *(dis_ptr + x) &= ~(0x08);
    break;

    case 4:
    *(dis_ptr + x) &= ~(0x10);
    break;

    case 5:
    *(dis_ptr + x) &= ~(0x20);
    break;

    case 6:
    *(dis_ptr + x) &= ~(0x40);
    break;

    case 7:
    *(dis_ptr + x) &= ~(0x80);
    break;
}

}

void display_number(uint8_t number, uint8_t row, uint8_t col){
  uint8_t offset_x = (row * 6);
  uint8_t offset_y = 24 - (col * 6);


  switch(number){
    case 0:
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);
    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);
    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 1:
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);
    break;
    case 2:
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 3, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);

    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);

    break;
    case 3:

    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 3, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);

    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 4:
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 3, offset_y +2);
    
    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    break;
    case 5:
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 3, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);

    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 6:
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 3, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);

    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 7:
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 1, offset_y +2);

    pixel_set(offset_x + 1, offset_y +3);
    break;
    case 8:
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);
    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 3, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);
    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 9:
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);
    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 3, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);
    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
  }





}

int getSize(char* c){
  int size =0;
  char* i;
  for(i = c; *i != '\0'; i++){
    size++;
  }
return size;
}

void display_char( char* chr, uint8_t row, uint8_t col){
  uint8_t offset_x = (row * 6);
  uint8_t offset_y = 26 - (col * 6);
  char temp;
  int i;
  int size = getSize(chr);
 

  for(i = 0; i < size ; i++){
    temp = *chr;
    chr++;
    if((i != 0) && (i % 5 == 0)){
      offset_x += 6;
      offset_y = 26;
    }
    
  switch(temp){
    case 'A':
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 3, offset_y +2);

    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 'C':
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);
    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);
    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 'E':
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 3, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);

    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);

    break;
    case 'G':

    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);

    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 'L':
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 5, offset_y +2);
    
    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 'M':
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 3, offset_y +2);

    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 'O':
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 2, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);

    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 'R':
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 2, offset_y +2);
    pixel_set(offset_x + 4, offset_y +2);

    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
     pixel_set(offset_x + 4, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 'S':
    pixel_set(offset_x + 1, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    pixel_set(offset_x + 5, offset_y +1);

    pixel_set(offset_x + 1, offset_y +2);
    pixel_set(offset_x + 3, offset_y +2);
    pixel_set(offset_x + 5, offset_y +2);

    pixel_set(offset_x + 1, offset_y +3);
    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 5, offset_y +3);
    break;
    case 'V':
    pixel_set(offset_x + 2, offset_y +1);
    pixel_set(offset_x + 3, offset_y +1);
    pixel_set(offset_x + 4, offset_y +1);
    
    pixel_set(offset_x + 5, offset_y +2);

    pixel_set(offset_x + 2, offset_y +3);
    pixel_set(offset_x + 3, offset_y +3);
    pixel_set(offset_x + 4, offset_y +3);
    break;
    case ':':
    pixel_set(offset_x + 2, offset_y +2);
    pixel_set(offset_x + 2, offset_y +4);
    break;
  }
  if(temp == ' '){
    offset_y -= 3;
  } else {
    offset_y -= 6;
  }
  }
 
}


/*
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

/*
void display_string(int line, char *s) {
  int i;
  if(line < 0 || line >= 4)
    return;
  if(!s)
    return;
  
  for(i = 0; i < 16; i++)
    if(*s) {
      textbuffer[line][i] = *s;
      s++;
    } else
      textbuffer[line][i] = ' ';
}

static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}
*/

