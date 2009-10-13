/*
 *   menu.h
 *      
 *   Copyright 2009 Andrew Anderson <billamonster.com>
 *      
 *   This file is part of Blockman.
 *
 *   Blockman is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Blockman is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.*
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Blockman.  If not, see <http://www.gnu.org/licenses/>.
 */


/*** Function Prototypes ***/
int MainMenu();
int ReplayPauseMenu();
int EndOfLevelMenu();
int OptionsMenu(bool inGame);
int PauseMenu();
int SelectLevelMenu();





/*** menuItem class ***/
class menuItem {
	public:
		// Constructor
		menuItem():text(NULL) {}
	
		// Destructor Prototype
		~menuItem();
	
		/*** Get ***/
		int GetX() const { return x; };
		int GetY() const { return y; };
		int GetW(int letterSpacing) const { return GetTextW(text, letterSpacing); }; // GetTextW is in font.cpp
		char* GetText() const { return text; };
	
		/*** Set ***/
		void SetX(int xPos) { x = xPos; };
		void SetY(int yPos) { y = yPos; };
		
		void DelText() { delete [] text; text = NULL; }; // This must be called before SetText
		void SetText(char *txt);
	
	private:
		int x;
		int y;
		char *text;
};

// Destructor
menuItem::~menuItem() {
	delete [] text;
	text = NULL;
}


void menuItem::SetText(char *txt) {
	DelText();
	
	text = new char[strlen(txt) + 1];
	strcpy(text, txt);
}





/*** menu class ***/
class menu {
	public:
		// Constructor protoype
		menu(uint numberOfItems);
		
		// Destructor prototype
		~menu();
		
		/*** Get ***/
		int GetItemX(uint item) { return items[item].GetX(); };
		int GetItemY(uint item) { return items[item].GetY(); };
		int GetItemW(uint item, int spacing) { return items[item].GetW(spacing); };
		int GetSel() const { return sel; };
		
		/*** Set ***/
		void SetSel(int s) { sel = s; };
		void Move(int xPos, int yPos) { x = xPos; y = yPos; };
		void MoveItem(uint item, int xPos, int yPos);
		void NameItem(uint item, const char *name);
		void NameItem(uint item, char *name);
		
		void SetTitle(const char *aTitle);
		void SetTitle(char *aTitle);
		
		/*** Other ***/
		void AutoArrange(char type);
		void Display();
		int Input();
		void SpaceItems(uint startItem);
	private:
		char *title;		// Title of the menu.
		int titleLetterSpacing;	// Letter spacing increase for title text.
		uint numItems;		// How many menuItems there are
		menuItem *items;	// Pointer to hold array of menuItems
		int sel;		// Currently selected item
		int x, y;		// Position of the entire menu
		int titleX, titleY;	// Position of the title (influenced by AutoArrange's type)
};

// Constructor
menu::menu(uint numberOfItems):
	title(NULL),
	titleLetterSpacing(4),
	sel(0)
{
	numItems = numberOfItems;
	items = new menuItem[numItems];
}

// Destructor
menu::~menu() {
	delete [] title;
	title = NULL;
	
	delete [] items;
	items = NULL;
}



// Overloaded for both <const char *> and <char *>
void menu::SetTitle(const char *aTitle) {
	// Clean up old text data
	delete [] title;
	title = NULL;
	
	// Convert <const char *> to <char *>
	char tempTitle[strlen(aTitle) + 1];
	strcpy(tempTitle, aTitle);

	title = new char[strlen(tempTitle) + 1];
	strcpy(title, tempTitle);
}
void menu::SetTitle(char *aTitle) {
	// Clean up old text data
	delete [] title;
	title = NULL;
	
	title = new char[strlen(aTitle) + 1];
	strcpy(title, aTitle);
}

// Overloaded for both "const char *" and "char *"
void menu::NameItem(uint item, const char *name) {
	// Clean up old text data
	items[item].DelText();
	
	char tempName[strlen(name) + 1];
	strcpy(tempName, name);
	items[item].SetText(tempName);
}
void menu::NameItem(uint item, char *name) {
	items[item].SetText(name);
}


// Arranges menu items according to type:
//	type = 0
//	Position first item at the menu's (x, y) and all other items
//	below it with a bit of space between each of them.
//
//	type = 1
//	Same as type 0, but items are centered on the menu's x-coordinate
void menu::AutoArrange(char type) {
	titleX = x;
	titleY = y;
	// Position first item right below title, and to the right a little
	items[0].SetX(x + FONT_W);
	items[0].SetY(y + FONT_H + (FONT_H / 4));
	
	// Type 0 (left-aligned)
	for (uint i = 1; i < numItems; i++) {
		items[i].SetX(items[i - 1].GetX());
		items[i].SetY(items[i - 1].GetY() + FONT_H + (FONT_H / 4));
	}

	// Type 1 uses type 0's Y-positionings, but changes the Xs to be centered
	if (type == 1) {
		titleX = x - (GetTextW(title, titleLetterSpacing) / 2);

		for (uint i = 0; i < numItems; i++) {
			items[i].SetX(x - (items[i].GetW(0) / 2));
		}
	}
}



void menu::MoveItem(uint item, int xPos, int yPos) {
	items[item].SetX(xPos);
	items[item].SetY(yPos);
}




void menu::Display() {
	uint r, g, b;
	
	if (title != NULL) {
		DrawText(titleX, titleY, title, titleLetterSpacing, 255, 255, 255);
	}
	for (uint i = 0; i < numItems; i++) {
		if (i == static_cast<uint>(sel)) {
			r = TEXT_HIGHLIGHT_R;
			g = TEXT_HIGHLIGHT_G;
			b = TEXT_HIGHLIGHT_B;
		}
		else {
			
			r = TEXT_NORMAL_R;
			g = TEXT_NORMAL_G;
			b = TEXT_NORMAL_B;
		}
		DrawText(items[i].GetX(), items[i].GetY(), items[i].GetText(), 0, r, g, b);
	}
}


int menu::Input() {
	int key = MenuInput(); // In input.cpp
	int oldSel = sel;
	
	switch (key) {
		case 1: // Up
			
			sel--;
			break;
		case 2: // Down
			sel++;
			break;
		case 3: // Left
			PlaySound(5);
			return 3;
			break;
		case 4: // Right
			PlaySound(5);
			return 4;
			break;
		case 5: // Enter
			PlaySound(6);
			return 1;
			break;
		case 6: // Home
			sel = 0;
			break;
		case 7: // End
			sel = numItems - 1;
			break;
		case 10: // Page Up
			sel -= 7;
			break;
		case 11: // Page Down
			sel += 7;
			break;
		case 8: // Esc
			PlaySound(7);
			return -1;
			break;
		case 9: // Close window
			return -2;
			break;
	}
	
	if (sel < 0) sel = 0;
	if (sel > static_cast<int>(numItems - 1)) sel = static_cast<int>(numItems - 1);
	
	if (sel != oldSel) PlaySound(5);
	
	return 0;
}


void menu::SpaceItems(uint startItem) {
	for (uint i = startItem; i < numItems; i++) {
		items[i].SetY(items[i].GetY() + FONT_H);
	}
}
