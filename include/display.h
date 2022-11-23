#ifndef _DISPLAY
#define _DISPLAY
#include <Arduino.h>

void displaySetup();
void displayBootScreen();
void displayDrawMenu(String menuType); // Draw many lines
void displayMenu(String menuType);
void displayOptionSelector(String menuType);
void displayScrollText(); // Draw scrolling text
void displayDrawGrid(String menuType);
void displaySetOptionBuffer(String menuType);
void displayCaseDetector(String type);
//
//
//
//
//
//
//
//

void testdrawrect(); // Draw rectangles (outlines)

void testfillrect(); // Draw rectangles (filled)

void testdrawcircle(); // Draw circles (outlines)

void testfillcircle(); // Draw circles (filled)

void testdrawroundrect(); // Draw rounded rectangles (outlines)

void testfillroundrect(); // Draw rounded rectangles (filled)

void testdrawtriangle(); // Draw triangles (outlines)

void testfilltriangle(); // Draw triangles (filled)

void testdrawchar(); // Draw characters of the default font

void testdrawstyles(); // Draw 'stylized' characters

void testdrawbitmap();

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h);
#endif // _DISPLAY