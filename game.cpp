/*
 *   game.cpp
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

bool LoadLevel(uint level);


void CollectLevelGarbage() {
	delete [] bricks; bricks = NULL;
	delete [] blocks; blocks = NULL;
	delete [] telepads; telepads = NULL;
	delete [] torches; torches = NULL;
	delete [] playerKeys; playerKeys = NULL;

	if (undoBlocks != NULL) {
		for (uint i = 0; i < maxUndo; i++) {
			delete [] undoBlocks[i];
			undoBlocks[i] = NULL;
		}
		delete [] undoBlocks;
		undoBlocks = NULL;
	}
}





void Game() {

	uint i;
	int x, b;
	std::string tempPath;
	char s[11];
	bool pushedKey;
	bool quitGame = false;
	char wonLevel; // 0 no, 1 yes
	
	currentLevel = 0;

	/*** Loop to advance to next level ***/
	while(quitGame == false) {
		// Load Level
		if (LoadLevel(currentLevel) == false) {
			fprintf(stderr, "Error: Loading level %d failed.", currentLevel);

			currentLevel = 0;
			CollectLevelGarbage();
			continue;
			//escKey = 1;
			//break;
		}
		
		
		
		int playerBlock[numPlayers]; 	// Stores which block the player is currently picking
						// up and waiting on until he can move again.
		
		// Initialize playerBlock
		for (i = 0; i < numPlayers; i++) {
			playerBlock[i] = -1;
		}
		
		wonLevel = 0;
		

		/******* GAME LOOP *******/
		while (quitGame == false) {
			#ifdef DEBUG
			i = currentLevel;
			#endif
			
			justUndid = false;
			Input();
						
			#ifdef DEBUG
			if (currentLevel != i) break;
			#endif

			if (gameKeys[0].on > 0) {
				quitGame = true;
			}
			
			if (gameKeys[1].on > 0) {
				Undo(1);
			}
			
			/** Manual Camera Movement **/
			
			i = 20; // camera speed
			
			// Move camera left
			if (gameKeys[2].on > 0) {
				cameraX -= i;
				printf("Moving camera left");
			}
			
			// Move camera right
			if (gameKeys[3].on > 0) {
				cameraX += i;
			}
			
			// Move camera up
			if (gameKeys[4].on > 0) {
				cameraY -= i;
			}

			// Move camera down
			if (gameKeys[5].on > 0) {
				cameraY += i;
			}
			
			
			/*** Handle Player Movement ***/
			for (i = 0; i < numPlayers; i++) {
				pushedKey = false; // This will forbid certain actions from both happening in the same frame
				

				// If the player is not already moving
				if (blocks[i].GetXMoving() == 0 && blocks[i].GetYMoving() == 0 
					// and is on solid ground
					&& blocks[i].OnSolidGround())
				{
					
					// Enter (push a block)
					if (pushedKey == false && playerBlock[i] == -1 && playerKeys[(i * NUM_PLAYER_KEYS) + 4].on > 0) {
						// Determine which tile the player is looking at.
						if (blocks[i].GetDir() == 0) {	// Facing left
							x = blocks[i].GetX() - TILE_W;
						}
						else { 				// Facing right
							x = blocks[i].GetX() + blocks[i].GetW();
						}
						b = BlockNumber(x, blocks[i].GetY() + (blocks[i].GetH() - 1), TILE_W, 1); // First try to push from bottom of player
						if (b < 0) b = BlockNumber(x, blocks[i].GetY(), TILE_W, 1); // If that didn't work, push from top
						
						if (b >= 0 && blocks[b].OnSolidGround()) {
							Undo(0); // Save Undo state
							
							if (blocks[i].GetDir() == 0) {
								blocks[i].SetXMoving(-TILE_W);
								blocks[b].SetXMoving(-TILE_W);
							}
							else {
								blocks[i].SetXMoving(TILE_W);
								blocks[b].SetXMoving(TILE_W);
							}
							if (blocks[i].GetStrong() == 1) blocks[b].SetStrong(1); // If player is strong, make this block strong for now
							playerBlock[i] = b; // Player can't move until this block stops moving
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
					
					// Left
					if (playerKeys[(i * NUM_PLAYER_KEYS) + 0].on > 0) {
						if (blocks[i].GetDir() == 0) {
							// Player musn't be waiting for a block he's acted upon
							if (playerBlock[i] == -1) {
								Undo(0); // Save Undo state
								
								blocks[i].Climb(0);
								pushedKey = true; // Now player can't push other buttons this frame
							}
						}
						else {
							blocks[i].SetDir(0);
						}
					}

					// Right
					if (pushedKey == false && playerKeys[(i * NUM_PLAYER_KEYS) + 1].on > 0) {
						if (blocks[i].GetDir() == 1) {
							// Player musn't be waiting for a block he's acted upon
							if (playerBlock[i] == -1) {
								Undo(0); // Save Undo state
								
								blocks[i].Climb(1);
								pushedKey = true; // Now player can't push other buttons this frame
							}
						}
						else {
							blocks[i].SetDir(1);
						}
					}
					
					// Up (pick up block)
					if (pushedKey == false && playerBlock[i] == -1 && playerKeys[(i * NUM_PLAYER_KEYS) + 2].on > 0) {
						// Determine which tile the player is looking at.
						if (blocks[i].GetDir() == 0) {	// Facing left
							x = blocks[i].GetX() - TILE_W;
						}
						else { 				// Facing right
							x = blocks[i].GetX() + blocks[i].GetW();
						}
						b = BlockNumber(x, blocks[i].GetY(), TILE_W, 1);
						
						// If it's a block, make it climb up onto the player =)
						if (b >= 0) {
							Undo(0); // Save Undo state
							
							// If player is strong, make this block strong for now
							if (blocks[i].GetStrong() == 1) blocks[b].SetStrong(1);
							
							// Climb
							blocks[b].Climb((blocks[i].GetDir() == 0) ? 1 : 0);
							
							playerBlock[i] = b; // Player can't move until this block stops moving
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
					
					
					// Down (set down block)
					if (pushedKey == false && playerBlock[i] == -1 && playerKeys[(i * NUM_PLAYER_KEYS) + 3].on > 0) {
						// What's on top of player's head?
						b = BlockNumber(blocks[i].GetX(), blocks[i].GetY() - 1, TILE_W, 1);

						// If it's a block, set its path to be set down.
						if (b >= 0 && blocks[b].GetXMoving() == 0 && blocks[b].GetYMoving() == 0) {
							Undo(0); // Save Undo state
							
							tempPath = "";
							if (blocks[i].GetDir() == 0) { // player facing left
								sprintf(s, "-%dy-%dx", TILE_H - blocks[i].GetH(), TILE_W);
								tempPath = s;
							}
							if (blocks[i].GetDir() == 1) { // player facing right
								sprintf(s, "-%dy%dx", TILE_H - blocks[i].GetH(), TILE_W);
								tempPath = s;
							}
							blocks[b].SetPath(tempPath);
							
							if (blocks[i].GetStrong() == 1) blocks[b].SetStrong(1); // If player is strong, make this block strong for now
							playerBlock[i] = b; // Player can't move until this block stops moving
							pushedKey = true; // Now player can't push other buttons this frame
						}
					}
				}

				// Check if player is waiting for a block he's acted upon to finish moving
				if (playerBlock[i] != -1) {
					// If it's not moving (along a path) anymore
					if (blocks[playerBlock[i]].GetPath().length() == 0 && blocks[playerBlock[i]].GetXMoving() == 0 && blocks[playerBlock[i]].GetYMoving() == 0) {
						// Not strong anymore
						blocks[playerBlock[i]].SetStrong(0);
						
						// No longer wait for it.
						playerBlock[i] = -1;
					}
				}
			}
			
			
			/*** Do Block Physics ***/
			// Reset all DidPhysics flags
			for (i = 0; i < numBlocks; i++) {
				blocks[i].SetDidPhysics(false);
				blocks[i].SetMoved(false);
			}
			// Do Physics
			for (i = 0; i < numBlocks; i++) {
				if (blocks[i].GetDidPhysics() == false) {
					//printf("===Block %d Physics===\n", i);
					blocks[i].Physics();
				}
			}
			// Do Post-Physics (decrement xMoving and yMoving, revoke temporary strength privileges)
			for (i = 0; i < numBlocks; i++) {
				blocks[i].PostPhysics();
			}
			
			
			// For each player:
			// If player is not floating and there is no block on his head, make him rise
			for (i = 0; i < numPlayers; i++) {
				if (blocks[i].GetType() == 1 && blocks[i].GetH() < TILE_H && BlockNumber(blocks[i].GetX(), blocks[i].GetY() - 1, blocks[i].GetW(), 1) < 0) {
					b = blocks[i].GetY(); 			// Save old y position
					blocks[i].SetYMoving(-blockYSpeed);	// Set the block to move up
					if (blocks[i].GetYMoving() < -2) blocks[i].SetYMoving(-2);
					blocks[i].Physics(); 			// Try to move the block up
					if (blocks[i].GetY() < b) 		// If it moved up, increase the height by the amount moved
						blocks[i].SetH(blocks[i].GetH() + (b - blocks[i].GetY()));
				}
			}

			// Check if player is on/in certain tiles
			for (i = 0; i < numPlayers; i++) {
				// Is player on a spike?
				if (BoxContents(blocks[i].GetX(), blocks[i].GetY() + blocks[i].GetH(), blocks[i].GetW(),1) == -3) {
					blocks[i].SetFace(4); // scared mouth
					blocks[i].SetDir(2); // Facing the camer
				}
				
				// Is player at exit?
				if (blocks[i].GetDir() != 3 && blocks[i].GetXMoving() == 0) {
					if (BoxOverlap(exitX, exitY, TILE_W, TILE_H, blocks[i].GetX(), blocks[i].GetY(), blocks[i].GetW(), blocks[i].GetH())) {
						blocks[i].SetFace(3); // happy mouth
						blocks[i].SetDir(2); // Face the camera
						wonLevel = 1;
					}
				}
			}
			
		
			/*** Do Telepads ***/
			for (i = 0; i < numTelepads; i++) {
				if (telepads[i].NeedsToTeleport()) {
					telepads[i].Teleport();
				}
			}


			// Center camera on player
			//cameraX = (blocks[0].GetX() + (blocks[0].GetW() / 2)) - (SCREEN_W / 2);
			//cameraY = (blocks[0].GetY() + (blocks[0].GetH() / 2)) - (SCREEN_H / 2);
			cameraTargetX = blocks[0].GetX() + (blocks[0].GetW() / 2);
			cameraTargetY = blocks[0].GetY() + (blocks[0].GetH() / 2);

			/** Render **/
			Render(1);
			
			if (wonLevel == 1) {
				SDL_Delay(1000);
				currentLevel ++;
				break;
			}
			
		} // End of game loop
		
		
		/** Collect garbage **/
		CollectLevelGarbage();


	} // Back to top of while loop to load next level
}






bool LoadLevel(uint level) {
	bool syntaxError = false;
	char filename[32];
	FILE * f;

	std::cout << "Loading level " << level << "...\n";
	sprintf(filename, "data/levels/%03d.txt", level);
	f = fopen(filename, "rb");
	if (f == NULL) {
		return false;
	}

	// Get info about level (width, height, numBlocks, numBricks, etc.)
	int c;
	int x = 0, y = 0, width = 0, height = 0;
	
	char telepadLetter[26];
	int telepadMates[26];
	for (uint i = 0; i < 26; i++) {
		telepadLetter[i] = 0;
		telepadMates[i] = 0;
	}
	bool addNewTelepad;
	
	numBlocks = 0;
	numPlayers = 0;
	numBricks = 0;
	numTelepads = 0;
	numTorches = 0;
	numSpikes = 0;
	while (!feof(f)) {
		c = fgetc(f);

		if (c > 32) {
			switch (c) {
				case '@': // player
					numPlayers ++;
					numBlocks ++;
					break;
				case '*': // exit
					break;
				case '^':
					numSpikes ++;
					break;
				case 'T':
					numTorches ++;
				case '0':
					numBricks ++;
					break;
				case 'x':
				case 'X':
					numBlocks ++;
					break;
				default:
					break;
			}
			// Check for lowercase a - z (telepad pairs)
			if (c >= 97 && c <= 122) {
				addNewTelepad = true;
				for (uint i = 0; i < numTelepads; i++) {
					if (telepadLetter[i] == static_cast<char>(c)) {
						telepadMates[i] ++;
						addNewTelepad = false;
						break;
					}
				}
				if (addNewTelepad) {
					telepadLetter[numTelepads] = static_cast<char>(c);
					numTelepads ++;
				}
			}
			
			x += TILE_W;
			if (x > width) width = x;
		}

		// New Line (LF)
		if (c == 10) {
			x = 0;
			height += TILE_H;			
		}
	}
	
	
	// Check for telepad errors
	for (uint i = 0; i < numTelepads; i++) {
		if (telepadMates[i] != 1) {
			fprintf(stderr, "Level data syntax error: Telepad '%c' should have 1 mate, but it has %d mates.\n", telepadLetter[i], telepadMates[i]);
			syntaxError = true;
		}
	}
	
	
	
	// Don't load the level if there are syntax errors
	if (syntaxError == true) {
		fprintf(stderr, "Syntax errors exist in the level data, so it can't be loaded.  Sorry =)\n");
		
		fclose(f);
		return false;
	}
	
	

	// Position level according to size, and initialize variables
	// according to number of objects

	x = -(width / 2);
	x += (abs(x) % TILE_W); // Align to grid
	
	y = -(height / 2);
	y += (abs(y) % TILE_H); // Align to grid

	cameraX = -(SCREEN_W / 2);
	cameraY = -(SCREEN_H / 2);

	// These global variables are used by the camera
	levelX = x;
	levelY = y;
	levelW = width;
	levelH = height;

	playerKeys = new keyBinding[NUM_PLAYER_KEYS * numPlayers];
	// Default Player keyboard layout
	playerKeys[0].sym = SDLK_LEFT;		// Move player left
	playerKeys[1].sym = SDLK_RIGHT;		// Move player right
	playerKeys[2].sym = SDLK_UP;		// Pick up block
	playerKeys[3].sym = SDLK_DOWN;		// Set down block
	playerKeys[4].sym = SDLK_RETURN;	// Push block

	bricks = new brick[numBricks];
	blocks = new block[numBlocks];
	telepads = new telepad[numTelepads];
	torches = new torch[numTorches];
	spikes = new spike[numSpikes];

	undoBlocks = new block*[maxUndo];
	for (uint i = 0; i < maxUndo; i++) {
 		undoBlocks[i] = new block[numBlocks];
	}
	Undo(-1); // Reset undo

	
	// Default physics settings
	blockXSpeed = 8;
	blockYSpeed = 8;
	blockXGravity = 0;
	blockYGravity = blockYSpeed;



	// Now read the level again, getting object coordinates
	numBlocks = numPlayers; // We'll re-use these variables for array indexes
	numPlayers = 0;
	numBricks = 0;
	numTorches = 0;
	numSpikes = 0;

	// Reset telepad pairing arrays
	for (uint i = 0; i < 26; i++) {
		telepadMates[i] = -1;
	}

	rewind(f); // Go back to the beginning of the file
	while (!feof(f)) {
		c = fgetc(f);
		
		if (c > 32) {
			switch (c) {
				case '@': // player
					blocks[numPlayers].SetX(x);
					blocks[numPlayers].SetY(y);
					blocks[numPlayers].SetType(1);
					numPlayers ++;
					break;
				case '*': // exit
					exitX = x;
					exitY = y;
					break;
				case '^':
					spikes[numSpikes].SetX(x);
					spikes[numSpikes].SetY(y);
					numSpikes ++;
					break;
				case 'T': // torch
					torches[numTorches].SetX(x);
					torches[numTorches].SetY(y);
					numTorches ++;
				case '0': // brick
					bricks[numBricks].SetX(x);
					bricks[numBricks].SetY(y);
					numBricks ++;
					break;
				case 'X': // block
					blocks[numBlocks].SetX(x);
					blocks[numBlocks].SetY(y);
					numBlocks ++;
					break;
				default:
					break;
			}

			// Check for lowercase a - z (telepad pairs)
			if (c >= 97 && c <= 122) {
				addNewTelepad = true;
				for (uint i = 0; i < numTelepads; i++) {
					if (telepadLetter[i] == c) {
						// If this is the second telepad in the pair
						if (telepadMates[i] == 0) {
							telepads[i].SetX2(x);
							telepads[i].SetY2(y);

							#ifdef DEBUG
							std::cout << "Telepad #" << i << " " << telepadLetter[i] << "2" << " (" << x << ", " << y << ")\n";
							#endif
						}

						// If this is the first telepad in the pair
						if (telepadMates[i] == -1) {
							telepadMates[i] = 0;
							telepads[i].SetX1(x);
							telepads[i].SetY1(y);
							
							#ifdef DEBUG
							std::cout << "Telepad #" << i << " " << telepadLetter[i] << "1" << " (" << x << ", " << y << ")\n";
							#endif
						}
					
						break;
					}
				}

			}

			x += TILE_W;
		}


		// New Line (LF)
		if (c == 10) {
			x = -(width / 2);
			x += (abs(x) % TILE_W); // Align to grid
			
			y += TILE_H;
		}
	}
	fclose(f);
	
	
	// Make the player face the exit
	if (exitX > blocks[0].GetX()) {
		blocks[0].SetDir(1);
	}
	else {
		blocks[0].SetDir(0);
	}

	return true;
}







// -1 = reset, 0 = save, 1 = load
void Undo(char action) {
	static int undoSlot = 0; // The slot number in the ringbuffer that will be written to next time Undo is called
	static int undoStart = 0; // The slot number of the oldest state.  We cannot undo past this slot.
	static int undoEnd = 0; // The slot number of the newest state.  If Undo(1) is called, this is the state that will be loaded.

	if (action == -1) {
		undoSlot = 0;
		undoStart = 0;
		undoEnd = 0;
	}
	
	if (action == 0) {
		if (undoEnd != undoStart && undoSlot == undoStart) {
			undoStart ++;
		}
		// Wrap around undoStart position
		if  (undoStart == static_cast<int>(maxUndo)) undoStart = 0;

		#ifdef DEBUG2
		printf("\nSaving state to undoSlot %d\n", undoSlot);
		#endif
		
		// Save the state of all the blocks
		for (uint i = 0; i < numBlocks; i++) {
			undoBlocks[undoSlot][i] = blocks[i];
		}


		// Make undoEnd point to the most recent state
		undoEnd = undoSlot;
		
		// Increment undoSlot to use the next slot next time
		undoSlot ++;

		// Wrap around undoSlot position
		if (undoSlot == static_cast<int>(maxUndo)) {
			undoSlot = 0;
		}

		
	
		#ifdef DEBUG2
		printf("UndoStart = %d\n", undoStart);
		printf("UndoEnd = %d\n", undoEnd);
		printf("UndoSlot = %d\n", undoSlot);
		#endif

	}
	

	if (action == 1) {
		if (! (undoStart == undoEnd && undoSlot == undoEnd)) {
			#ifdef DEBUG2
			printf("\nUndoing from slot %d\n", undoEnd);
			#endif
			
			// Restore the state of all the blocks
			for (uint i = 0; i < numBlocks; i++) {
				blocks[i] = undoBlocks[undoEnd][i];
			}

			if (undoEnd != undoStart) {
				// Move undoEnd back, so it points to the now most recent undo state
				undoEnd --;
				if (undoEnd == -1) undoEnd = maxUndo - 1;
			}
			
			// Move undoSlot back, so we can overwrite this state next time
			undoSlot --;
			if (undoSlot == -1) undoSlot = maxUndo - 1;

			
			justUndid = true; // Flag to tell telepads to ignore new occupants
			
			#ifdef DEBUG2
			printf("UndoEnd = %d\n", undoEnd);
			printf("UndoSlot = %d\n", undoSlot);
			#endif
		}
	
	}
}
