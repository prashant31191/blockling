/*
 *   Copyright 2010 Andrew Anderson <www.billamonster.com>
 *      
 *   This file is part of Blockling.
 *
 *   Blockling is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Blockling is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.*
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Blockling.  If not, see <http://www.gnu.org/licenses/>.
 */


int ControlSetupMenu(bool inGame) {
	int numItems = NUM_PLAYER_KEYS + 4 + 1 + 2;
	menu csMenu(numItems); // Create the menu object
	char text[256]; // For temporarily holding menu items' text as it is formed
	char tempString[32];
	
	int i;
	int action;
	bool gettingKey = false;
	SDLKey keySym;
	
	/** Set static menu items **/
	csMenu.Move(inGame ? SCREEN_W / 2 : 75, FONT_H * 4);
	csMenu.SetTitle("CONTROL SETUP");
	csMenu.NameItem(numItems - 2, "Reset To Defaults");
	csMenu.NameItem(numItems - 1, "Done");

	while (true) {
		/** Set dynamic menu items' text *****************/
		for (i = 0; i < numItems - 2; i++) {
			// Determine text
			switch (i) {
				case 0:
					sprintf(tempString, "Move Left:\t");
					break;
				case 1:
					sprintf(tempString, "Move Right:\t");
					break;
				case 2:
					sprintf(tempString, "Pick Up Block:");
					break;
				case 3:
					sprintf(tempString, "Drop Block:\t");
					break;
				case 4:
					sprintf(tempString, "Push Block:\t");
					break;
				case 5:
					sprintf(tempString, "Move Camera Left:");
					break;
				case 6:
					sprintf(tempString, "Move Camera Right:");
					break;
				case 7:
					sprintf(tempString, "Move Camera Up:\t");
					break;
				case 8:
					sprintf(tempString, "Move Camera Down:");
					break;
				case 9:
					sprintf(tempString, "Undo:");
					break;
			}
			
			
			if (i < static_cast<int>(NUM_PLAYER_KEYS)) {
				keySym = option_playerKeys[i].sym;
			}
			else if (i < static_cast<int>(NUM_PLAYER_KEYS) + 5) {
				keySym = gameKeys[i - 5].sym;
			}
			
			// Store this menu item's string in the text variable
			sprintf(
				text,
				"%s\t%s",
				tempString,
				(gettingKey && csMenu.GetSel() == static_cast<int>(i)) ?
					"Press a key..." :
					KeyName(keySym)
			);

			// Set it
			csMenu.NameItem(i, text);
		}


		/** Render *****************/
		do {
			if (inGame) {
				Render(0b00001000);
			}
			else {
				DrawBackground();
			}
			csMenu.AutoArrange(static_cast<char>(inGame ? 1 : 0));
			
			// Cheatingly position the items at a fixed x, if using the inGame text centering
			if (inGame) {
				for (i = 0; i < numItems - 2; i++) {
					csMenu.MoveItem(i, (SCREEN_W / 2) - (FONT_W * 10), csMenu.GetItemY(i));
				}
			}
			csMenu.SpaceItems(numItems - 2);
			csMenu.Display();
			UpdateScreen();
			
			if (gettingKey) {
				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_KEYDOWN) {
						if (event.key.keysym.sym != SDLK_ESCAPE) {
							if (csMenu.GetSel() < static_cast<int>(NUM_PLAYER_KEYS)) {
								option_playerKeys[csMenu.GetSel()].sym = event.key.keysym.sym;
							}
							else {
								gameKeys[csMenu.GetSel() - 5].sym = event.key.keysym.sym;
							}
						}
						gettingKey = false;
						break;
					}
				}
			}
		} while (gettingKey);

		
		
		/** Input ******************/
		action = csMenu.Input();
		
		switch (action) {
			case 1: // Enter
				// Reset to defaults
				if (csMenu.GetSel() == numItems - 2) {
					ResetDefaultKeys();
					break;
				}
				
				// Done
				if (csMenu.GetSel() == numItems - 1) return -1;

				// Change a key
				gettingKey = true;
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
	}
}




int Credits() {
	int y = SCREEN_H; // Position of the scrolling credits
	//char text[1024]; // For holding the credits
	uint t = SDL_GetTicks();
	
	//sprintf(text, "BLOCKMAN\nversion 0.1\n\nCopyright 2010 Andrew Anderson <http://www.billamonster.com>\n\nThis is free software, and you are welcome to change and redistribute it under the conditions of the GNU General Public License (GPL) version 3 or later <http://gnu.org/licenses/gpl.html>.  There is NO WARRANTY, to the extent permitted by law.\n\n\nPROGRAMMING\nAndrew Anderson\n\n\nSOUND\nAndrew Anderson\n\n\nGRAPHICS\nAndrew Anderson\n\n\nLEVEL DESIGN\nAndrew Anderson\n\n\nTESTING\nTyler Christensen\nAubree Dinsfriend\nMatthew Galla\nJeanette Ortiz\nJeTSpice <http://jetspicegames.com/>\n\n\nBASED ON AN ORIGINAL CONCEPT\nfound in\nBLOCK-MAN 1\nCopyright 1993 Soleau Software <http://www.soleau.com>\n\n\nSPECIAL THANKS TO\nthe teams behind the free/open-source projects which made this game possible:\n\nAudacity <http://audacity.sourceforge.net>\nDebian <http://www.debian.org>\nGeany <http://www.geany.org>\nGIMP <http://www.gimp.org>\nGNU <http://www.gnu.org>\nThe Linux Kernel <http://www.kernel.org>\nSDL <http://www.libsdl.org>\nUbuntu <http://www.ubuntu.com>\n\n\nETERNAL THANKS TO\nYeshua the Resurrected Messiah");
	
	while (y > -(FONT_H * 70)) {
		
		if (y > SCREEN_H) y = SCREEN_H;
		
		DrawBackground();
		DrawText(SCREEN_W / 2, y, "BLOCKLING\n\nCopyright 2010 Andrew Anderson <http://www.billamonster.com>\n\nThis is free software, and you are welcome to change and redistribute it under the conditions of the GNU General Public License (GPL) version 3 or later <http://gnu.org/licenses/gpl.html>.  There is NO WARRANTY, to the extent permitted by law.\n\n\nPROGRAMMING\nAndrew Anderson\n\n\nSOUND\nAndrew Anderson\n\n\nGRAPHICS\nAndrew Anderson\n\n\nLEVEL DESIGN\nAndrew Anderson\n\n\nTESTING\nTyler Christensen\nAubree Dinsfriend\nMatthew Galla\nJeanette Ortiz\nJeTSpice <http://jetspicegames.com/>\n\n\nBASED ON AN ORIGINAL CONCEPT\nfound in\nBLOCK-MAN 1\nCopyright 1993 Soleau Software <http://www.soleau.com>\n\n\nSPECIAL THANKS TO\nthe teams behind the free/open-source projects which made this game possible:\n\nAudacity <http://audacity.sourceforge.net>\nDebian <http://www.debian.org>\nGeany <http://www.geany.org>\nGIMP <http://www.gimp.org>\nGNU <http://www.gnu.org>\nThe Linux Kernel <http://www.kernel.org>\nSDL <http://www.libsdl.org>\nUbuntu <http://www.ubuntu.com>\n\n\nETERNAL THANKS TO\nYeshua the Resurrected Messiah", true, (SCREEN_W * .9), 0, 1);
		UpdateScreen();

		switch (MenuInput()) {
			case 1: // Up
				y += FONT_H;
				t = SDL_GetTicks();
				break;
			case 2: // Down
				y -= FONT_H;
				t = SDL_GetTicks();
				break;
			case 10: // Page Up
				y += SCREEN_H;
				t = SDL_GetTicks();
				break;
			case 11: // Page Down
				y -= SCREEN_H;
				t = SDL_GetTicks();
				break;
			case 8: // Esc
				return -1;
				break;
			case 9: // Close window
				return -2;
				break;
		}

		if (SDL_GetTicks() >= t + 250) {
			t = SDL_GetTicks();
			y -= (FONT_H / 2);
		}
	}
	
	return 0;
}




int HelpMenu(bool inGame) {
	int numItems = 1;
	menu helpMenu(numItems); // Create the menu object
	char text[256]; // Hold the help text
	
	int x1 = (SCREEN_W / 2) - (FONT_W * 18); // Position of titles
	int x2 = (SCREEN_W / 2) - (FONT_W * 17); // Position of body text
	
	int action;
	
	/** Set static menu items **/
	helpMenu.Move(inGame ? SCREEN_W / 2 : 75, (FONT_H + 2) * 1);
	helpMenu.SetTitle("HELP");
	helpMenu.NameItem(0, "Done");

	while (true) {
		/** Render *****************/
		if (inGame) {
			Render(0b00001000);
		}
		else {
			DrawBackground();
		}
		helpMenu.AutoArrange(static_cast<char>(inGame ? 1 : 0));
		helpMenu.MoveItem(0, helpMenu.GetItemX(0), (FONT_H + 2) * 20); // Postion the "Done" button
		helpMenu.Display();

		/** Show the help text ****/
		sprintf(text, "How to Play:");
		DrawText(x1, (FONT_H + 2) * 2, text, 3);
		sprintf(text, "Pick up blocks and stack them to get to the exit!  BLOCKMAN can only climb up steps one block high.");
		DrawText(x2, (FONT_H + 2) * 3, text, false, static_cast<int>(SCREEN_W * .9) - x2, 0, 1);

		sprintf(text, "In-Game Controls:");
		DrawText(x1, (FONT_H + 2) * 7, text, 3);
		
		sprintf(text, "Esc\t\t\tShow the pause menu\nF1\t\t\tShow help\nF5\t\t\tRestart the level\nPAGE UP/DOWN\tChange the tileset\n(See CONTROL SETUP in the OPTIONS menu to set the movement controls)");
		DrawText(x2, (FONT_H + 2) * 8, text, false, static_cast<int>(SCREEN_W * .9) - x2, 0, 1);

		sprintf(text, "Global Keyboard Shortcuts:");
		DrawText(x1, (FONT_H + 2) * 15, text, 3);
		sprintf(text, "F2\t\t\tToggle music ON/OFF\nF3\t\t\tToggle sound ON/OFF\nAlt + Enter\tToggle fullscreen mode");
		DrawText(x2, (FONT_H + 2) * 16, text, 1);

		UpdateScreen();
		
		
		/** Input ******************/
		action = helpMenu.Input();
		
		switch (action) {
			case 1: // Enter
				switch (helpMenu.GetSel()) {
					case 0:
						return -1;
						break;
				}
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
	}
}





int MainMenu() {
	int numItems = 4;
	menu mainMenu(numItems);
	
	mainMenu.SetTitle(GAME_TITLE);
	mainMenu.NameItem(0, "Play");
	mainMenu.NameItem(1, "Options");
	mainMenu.NameItem(2, "Credits");
	mainMenu.NameItem(3, "Quit");
	
	mainMenu.Move(75, FONT_H * 7);
	mainMenu.AutoArrange(0);
	mainMenu.SpaceItems(3);
	
	int action;
	
	while (true) {
		action = mainMenu.Input();
		
		switch (action) {
			case 1: // Enter
				return mainMenu.GetSel();
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
		
		
		DrawBackground();
		mainMenu.Display();
		UpdateScreen();
	}
}



int OptionsMenu(bool inGame) {
	int numItems = 10;
	menu optMenu(numItems); // Create the menu object
	char text[72]; // For temporarily holding menu items' text as it is formed
	
	uint maxUndoSize = 500;
	char change_undoSize;
	
	char maxBackground = 2;
	
	char maxCameraMode = 1;
	
	int action;
	
	/** Set static menu items **/
	optMenu.Move(inGame ? SCREEN_W / 2 : 75, FONT_H * 4);
	optMenu.SetTitle("OPTIONS");
	optMenu.NameItem(8, "Control Setup");
	optMenu.NameItem(9, "Done");

	while (true) {
		/** Set dynamic menu items' text *****************/

		// Determine Sound text
		sprintf(text, "Sound: ");
		strcat(text, (option_soundOn ? "ON" : "OFF"));
		optMenu.NameItem(0, text);

		optMenu.SetLeftArrow(0,	(option_soundOn > 0));
		optMenu.SetRightArrow(0, (option_soundOn < 1));

		
		// Determine Music text
		sprintf(text, "Music: ");
		strcat(text, (option_musicOn ? "ON" : "OFF"));
		optMenu.NameItem(1, text);

		optMenu.SetLeftArrow(1,	(option_musicOn > 0));
		optMenu.SetRightArrow(1, (option_musicOn < 1));

		// Determine Timer on/off text
		sprintf(text, "Game Timer: ");
		strcat(text, (option_timerOn ? "ON" : "OFF"));
		optMenu.NameItem(2, text);

		optMenu.SetLeftArrow(2,	(option_timerOn > 0));
		optMenu.SetRightArrow(2, (option_timerOn < 1));

		// Determine Timer on/off text
		sprintf(text, "Replays: ");
		strcat(text, (option_replayOn ? "ON" : "OFF"));
		optMenu.NameItem(3, text);

		optMenu.SetLeftArrow(3,	(option_replayOn > 0));
		optMenu.SetRightArrow(3, (option_replayOn < 1));


		// Determine Fullscreen on/off text
		sprintf(text, "Fullscreen: ");
		strcat(text, (option_fullscreen ? "ON" : "OFF"));
		optMenu.NameItem(4, text);

		optMenu.SetLeftArrow(4,	(option_fullscreen > 0));
		optMenu.SetRightArrow(4, (option_fullscreen < 1));


		// Determine Camera Setting text
		sprintf(text, "Camera: ");
		switch (option_cameraMode) {
			case 0:
				strcat(text, "AUTO");
				break;
			case 1:
				strcat(text, "MANUAL");
				break;
		}
		optMenu.NameItem(5, text);

		optMenu.SetLeftArrow(5, (option_cameraMode > 0));
		optMenu.SetRightArrow(5, (option_cameraMode < maxCameraMode));


		// Determine Background text
		sprintf(text, "Background: ");
		switch (option_background) {
			case 0:
				strcat(text, "OFF");
				break;
			case 1:
				strcat(text, "STATIC");
				break;
			case 2:
				strcat(text, "SCROLLING");
				break;
		}
		optMenu.NameItem(6, text);

		optMenu.SetLeftArrow(6, (option_background > 0));
		optMenu.SetRightArrow(6, (option_background < maxBackground));


		// Determine Undo text
		sprintf(text, "Undo Memory: %d move", option_undoSize);
		if (option_undoSize != 1) strcat(text, "s");
		optMenu.NameItem(7, text);
		
		optMenu.SetLeftArrow(7,	(option_undoSize > 0));
		optMenu.SetRightArrow(7, (option_undoSize < maxUndoSize));
		

		
		/** Render *****************/
		if (inGame) {
			Render(0b00001000);
		}
		else {
			DrawBackground();
		}
		optMenu.AutoArrange(static_cast<char>(inGame ? 1 : 0));
		optMenu.SpaceItems(8);
		optMenu.SpaceItems(9);
		optMenu.Display();
		// If the "replays" or "undo" options are selected
		if (inGame && (optMenu.GetSel() == 3 || optMenu.GetSel() == 7)) {
			DrawText(SCREEN_W / 2, FONT_H * 20, "(Note: This setting will not take effect\nuntil a new level is loaded.)", true, SCREEN_W * .9, 0, 1);
		}
		UpdateScreen();
		
		
		/** Input ******************/
		action = optMenu.Input();
		change_undoSize = 0;
		
		switch (action) {
			case 1: // Enter
				switch (optMenu.GetSel()) {
					case 0:
						ToggleSound();
						break;
					case 1:
						option_musicOn = (option_musicOn ? false : true);
						break;
					case 2:
						option_timerOn = (option_timerOn ? false : true);
						break;
					case 3:
						option_replayOn = (option_replayOn ? false : true);
						break;
					case 4:
						ToggleFullscreen();
						break;
					case 5:
						if (option_cameraMode < maxCameraMode) {
							option_cameraMode++;
						}
						else {
							option_cameraMode = 0;
						}
						break;
					case 6:
						if (option_background < maxBackground) {
							option_background++;
						}
						else {
							option_background = 0;
						}
						break;
					case 7:
						change_undoSize = 1;
						if (option_undoSize == maxUndoSize) option_undoSize = 0;
						break;
					case 8:
						if (ControlSetupMenu(inGame) == -2) return -2;
						break;
					case 9:
						return -1;
						break;
				}
				break;
			case 3: // Left
				switch (optMenu.GetSel()) {
					case 0:
						if (option_soundOn) ToggleSound();
						break;
					case 1:
						option_musicOn = false;
						break;
					case 2:
						option_timerOn = false;
						break;
					case 3:
						option_replayOn = false;
						break;
					case 4:
						if (option_fullscreen) ToggleFullscreen();
						break;
					case 5:
						if (option_cameraMode > 0) option_cameraMode--;
						break;
					case 6:
						if (option_background > 0) option_background--;
						break;
					case 7:
						change_undoSize = -1;						
						break;
				}
				break;
			case 4: // Right
				switch (optMenu.GetSel()) {
					case 0:
						if (!option_soundOn) {
							ToggleSound();
							PlaySound(5); // Play sound again here, since the sound in the menu selection was muted.
						}
						break;
					case 1:
						option_musicOn = true;
						break;
					case 2:
						option_timerOn = true;
						break;
					case 3:
						option_replayOn = true;
						break;
					case 4:
						if (!option_fullscreen) ToggleFullscreen();
						break;
					case 5:
						if (option_cameraMode < maxCameraMode) option_cameraMode++;
						break;
					case 6:
						if (option_background < maxBackground) option_background++;
						break;
					case 7:
						change_undoSize = 1;
						break;
				}
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}

		// Adjust undoSize according to which key was pushed,
		// handling irregular sequence of 0, 1, 50, 100, etc.
		if (change_undoSize == 1) { // Increase
			if (option_undoSize == 0) {
				option_undoSize = 1;
			}
			else if (option_undoSize == 1) {
				option_undoSize = 50;
			}
			else {
				option_undoSize += 50;
			}
		}
		if (change_undoSize == -1) { // Decrease
			if (option_undoSize == 50) {
				option_undoSize = 1;
			}
			else if (option_undoSize == 1) {
				option_undoSize = 0;
			}
			else if (option_undoSize > 50) {
				option_undoSize -= 50;
			}
		}
		
		// Limit undoSize
		if (option_undoSize > maxUndoSize) option_undoSize = maxUndoSize;
	}
}





int PauseMenu() {
	int numItems = 6;
	menu pauseMenu(numItems);
	
	pauseMenu.SetTitle("PAUSED");
	pauseMenu.NameItem(0, "Resume");
	pauseMenu.NameItem(1, "Options");
	pauseMenu.NameItem(2, "Help");
	
	pauseMenu.NameItem(3, "Resart Level");
	pauseMenu.NameItem(4, "Change Level");
	pauseMenu.NameItem(5, "Quit Game");
	
	pauseMenu.Move(SCREEN_W / 2, 100);
	pauseMenu.AutoArrange(1);
	pauseMenu.SpaceItems(3);

	int action;
	
	while (true) {
		action = pauseMenu.Input();
		
		switch (action) {
			case 1: // Enter
				switch (pauseMenu.GetSel()) {
					case 1: // Options
						if (OptionsMenu(true) == -2) {
							// If closing the window was requested
							return -2;
						}
						else {
							// Sync the in-game player keymap with the preferences,
							// in case they changed.
							RefreshPlayerKeys();
						}
						break;
					case 2:
						if (HelpMenu(true) == -2) {
							// If closing the window was requested
							return -2;
						}
						break;
					default:
						return pauseMenu.GetSel();
						break;
				}
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
		
		Render(0b00001000);
		pauseMenu.Display();
		UpdateScreen();
	}
}




int EndOfLevelMenu() {
	int numItems = 4;
	
	menu theMenu(numItems);
	
	theMenu.SetTitle("NOW WHAT?");
	theMenu.NameItem(0, "Next Level");
	theMenu.NameItem(1, "View Replay");
	theMenu.NameItem(2, "Change Level");
	theMenu.NameItem(3, "Quit Game");
	
	theMenu.Move(SCREEN_W / 2, 100);
	theMenu.AutoArrange(1);
	theMenu.SpaceItems(2);
	
	int action;
	
	while (true) {
		action = theMenu.Input();
		
		switch (action) {
			case 1: // Enter
				return theMenu.GetSel();
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
		
		Render(0b00001110);
		theMenu.Display();
		UpdateScreen();
	}
}




int ReplayPauseMenu() {
	int numItems = 6;
	
	menu pauseMenu(numItems);
	
	pauseMenu.SetTitle("REPLAY PAUSED");
	pauseMenu.NameItem(0, "Resume");
	pauseMenu.NameItem(1, "Take Control From Here");
	pauseMenu.NameItem(2, "Restart");
	pauseMenu.NameItem(3, "Options");
	pauseMenu.NameItem(4, "Next Level");
	pauseMenu.NameItem(5, "Quit Game");
	
	pauseMenu.Move(SCREEN_W / 2, 100);
	pauseMenu.AutoArrange(1);
	pauseMenu.SpaceItems(3);
	pauseMenu.SpaceItems(4);
	
	int action;
	
	while (true) {
		action = pauseMenu.Input();
		
		switch (action) {
			case 1: // Enter
				switch (pauseMenu.GetSel()) {
					case 3: // Options
						if (OptionsMenu(true) == -2) {
							// If closing the window was requested
							return -2;
						}
						else {
							// Sync the in-game player keymap with the preferences,
							// in case they changed.
							RefreshPlayerKeys();
						}
						break;
					default:
						return pauseMenu.GetSel();
						break;
				}
				
				break;
			case -1: // Esc
				return -1;
				break;
			case -2: // Close window
				return -2;
				break;
		}
		
		Render(0b00001000);
		pauseMenu.Display();
		UpdateScreen();
	}

}





int SelectLevelMenu() {
	uint i;
	int numItems = 2;
	menu lvlMenu(numItems); // Create the menu object
	char text[512]; // For holding complete level error message
	int bottomY = SCREEN_H - FONT_H - 4;
	
	FILE *testFile;
	bool refreshLevel = true;
	uint numLevels = 999;
	char *levelError = NULL; // For pointing to the (actual) error message returned by LoadLevel
	
	int action;
	
	lvlMenu.Move(SCREEN_W / 2, 4);
	lvlMenu.SetTitle("");
	lvlMenu.SetSel(1); // Select the level number menu item by default

	while (true) {
		/** Load Level *****************/
		if (refreshLevel) {
			while (true) {
				// Load Level
				delete [] levelError;
				levelError = LoadLevel(currentLevel);
				
				// If the level could not be loaded
				sprintf(text, "!");
				if (levelError != NULL && strcmp(levelError, text) == 0) {
					numLevels = currentLevel;
					
					if (currentLevel <= 0) break;
					currentLevel--;
					continue;
				}
				
				
				testFile = OpenLevel(currentLevel + 1);
				if (testFile != NULL) {
					fclose(testFile);
				}
				else {
					numLevels = currentLevel + 1;
				}
				
				/*** Turn off all the status lights on the telepads ***/
				if (levelError == NULL) {
					for (i = 0; i < numTelepads; i++) {
						telepads[i].NeedsToTeleport();
					}
				}
				
				break;
			}
			
			refreshLevel = false;
		}


		/** Set dynamic menu items' text *****************/
		// Determine Leveset text
		sprintf(text, "Set: ");
		switch (option_levelSet) {
			case 0:
				strcat(text, GAME_TITLE);
				break;
			case 1:
				strcat(text, "BLOCK-MAN 1");
				break;
			case 2:
				strcat(text, "CUSTOM");
				break;
		}
		lvlMenu.NameItem(0, text);
		lvlMenu.SetLeftArrow(0,	(option_levelSet > 0));
		lvlMenu.SetRightArrow(0, (option_levelSet < NUM_LEVEL_SETS - 1));
		
		// Determine Level text
		sprintf(text, "Level %d", currentLevel);
		lvlMenu.NameItem(1, text);
		lvlMenu.SetLeftArrow(1,	(currentLevel > 0));
		lvlMenu.SetRightArrow(1, (currentLevel < numLevels - 1));
		
		
		/** Render *****************/
		if (levelError == NULL) {
			Render(0b00001010);
		}
		else {
			DrawBackground();
			sprintf(text, "Syntax errors exist in the level file:\n\n%s\nPlease fix the errors and try selecting the level again.", levelError);
			DrawText(FONT_W * 3, FONT_H * 3, text, false, (SCREEN_W * .9) - (FONT_W * 3), 0, 1);
		}
		lvlMenu.AutoArrange(static_cast<char>(1));
		
		// Position the menu items to be stacked at the bottom of the screen
		lvlMenu.MoveItem(0, lvlMenu.GetItemX(0), bottomY - FONT_H - 2);
		lvlMenu.MoveItem(1, lvlMenu.GetItemX(1), bottomY);
		
		// Display the menu
		lvlMenu.Display();
		
		// Update the screen
		UpdateScreen();
		
		
		
		/** Input ******************/
		action = lvlMenu.Input();
		
		switch (action) {
			case 1: // Enter
				if (levelError == NULL) {
					switch (lvlMenu.GetSel()) {
						case 0:
						case 1:
							delete [] levelError;
							return lvlMenu.GetSel();
							break;
					}
				}
				break;
			case 3: // Left
				switch (lvlMenu.GetSel()) {
					case 0:
						if (option_levelSet > 0) option_levelSet--;
						numLevels = 999;
						refreshLevel = true;
						break;
					case 1:
						if (currentLevel > 0) currentLevel--;
						refreshLevel = true;
						break;
				}
				break;
			case 4: // Right
				switch (lvlMenu.GetSel()) {
					case 0:
						if (option_levelSet < NUM_LEVEL_SETS - 1) option_levelSet++;
						numLevels = 999;
						refreshLevel = true;
						break;
					case 1:
						currentLevel++;
						refreshLevel = true;
						break;
				}
				break;
			case -1: // Esc
				CollectLevelGarbage();
				delete [] levelError;
				return -1;
				break;
			case -2: // Close window
				CollectLevelGarbage();
				delete [] levelError;
				return -2;
				break;
		}

		// Limit currentLevel
		if (currentLevel > numLevels) currentLevel = numLevels;
	}
}
