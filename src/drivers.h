#include <stdbool.h>
#include <stddef.h>

#define TEXT_BUFFER 0xB8000

#define ROWS 25
#define COLUMNS 80

#define TERMINAL_COLOR 0x0F

#define SCREEN_CTRL 0x3D4
#define SCREEN_DATA 0x3D5

// PORT I/O DRIVERS

unsigned char port_byte_in(unsigned short port) {
	unsigned char result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

void port_byte_out(unsigned short port, unsigned char data) {
	__asm__ ("out %%al, %%dx " : : "a" (data), "d" (port));
}

unsigned short port_word_in(unsigned short port) {
	unsigned short result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

void port_word_out(unsigned short port, unsigned short data) {
	__asm__ ("out %%ax, %%dx" : : "a" (data), "d" (port));
}

// STRING MANIPULATION

unsigned long strlen(char string[]) {
	int len;
	for (int i=1; string[i]!=0; i++) {
		len = i;
	}
	return len+1;
}

bool strcmp(char str1[], char str2[]) {
	long len1, len2;
	len1 = strlen(str1);
	len2 = strlen(str2);

	if (len1 != len2) {
		return false;
	}
	
	for (int i=0; i<len1; i++) {
		if (str1[i] != str2[i]) {
			return false;
		}
	}
	return true;
}

// SCREEN I/O DRIVERS

int get_offset(int x, int y) {
	return 2*((y*COLUMNS)+x);
}

int get_cursor() {
	port_byte_out(SCREEN_CTRL, 14);
	int offset = port_byte_in(SCREEN_DATA) << 8;
	port_byte_out(SCREEN_CTRL, 15);
	offset += port_byte_in(SCREEN_DATA);
	return offset*2;
}

void set_cursor(int x, int y) {
	int offset;
	if (y) {
		offset = get_offset(x, y)/2;
	}
	else {
		offset = x;
	}
	port_byte_out(SCREEN_CTRL, 15);
	port_byte_out(SCREEN_DATA, offset & 255);
	port_byte_out(SCREEN_CTRL, 14);
	port_byte_out(SCREEN_DATA, offset >> 8 & 255);
}

void puts_at(char text[], int x, int y, int color) {
	char* vidptr = 0xB8000;
	char* destptr;
	int column = x;
	int row = y;
	int i = 0;
	while (text[i] != '\0') {
		if (text[i] == '\n') {
			row += 1;
			column = 0;
		}
		else {
			destptr = vidptr+get_offset(column, row);
			*destptr = text[i];
			destptr += 1;
			*destptr = color;
		}
		if (column > 79) {
			row += 1;
			column = 1;
		}
		else {
			column += 1;
		}
		i++;
	}
	set_cursor(column-1, row);
}

void puts(char string[]) {
	int startPos = get_cursor();
	puts_at(string, startPos, NULL, TERMINAL_COLOR);
}
