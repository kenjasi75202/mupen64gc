/* Save-Prompt.c - prompts the user which location to save to
   by Mike Slegeir for Mupen64-GC
 **************************************************************
   TODO: Verify save location so data is not lost?
 */

#include <stdio.h>
#include <string.h>

#include <gcutil.h>
#include <sdcard.h>
#include <ogc/card.h>
#include <ogc/pad.h>
#include "Saves.h"

#define MAX_SAVE_PATH       64

// globals set when user chooses location
int  savetype;
char savepath[MAX_SAVE_PATH];

// Used for Memory card working area
static u8 SysArea[CARD_WORKAREA] ATTRIBUTE_ALIGN (32);
void card_removed_cb(s32 chn, s32 result){ CARD_Unmount(chn); }

// This should be called when the game begins
// or when the user wants to save somewhere else
void select_location(void){
	int selection = 0;
	
	// Get user selection here
	BOOL inited = FALSE;
	while(!inited){
#ifdef USE_GUI
	
#else
	printf("--SAVE GAME--\n");
	printf("Choose slot:\n"
	       "  (A) Slot A\n"
	       "  (B) Slot B\n");
	while(!(PAD_ButtonsHeld(0) & PAD_BUTTON_A || 
	        PAD_ButtonsHeld(0) & PAD_BUTTON_B )); // Wait for input
	if(PAD_ButtonsHeld(0) & PAD_BUTTON_A) selection |= SELECTION_SLOT_A;
	else                                  selection |= SELECTION_SLOT_B;
	while((PAD_ButtonsHeld(0) & PAD_BUTTON_A || 
	       PAD_ButtonsHeld(0) & PAD_BUTTON_B )); // Wait for button release

	printf("Choose card type:\n"
	       "  (A) Memory Card\n"
	       "  (B) SD Card\n");
	while(!(PAD_ButtonsHeld(0) & PAD_BUTTON_A || 
	        PAD_ButtonsHeld(0) & PAD_BUTTON_B )); // Wait for input
	if(PAD_ButtonsHeld(0) & PAD_BUTTON_A) selection |= SELECTION_TYPE_MEM;
	else                                  selection |= SELECTION_TYPE_SD;
	while((PAD_ButtonsHeld(0) & PAD_BUTTON_A || 
	       PAD_ButtonsHeld(0) & PAD_BUTTON_B )); // Wait for button release


	printf("Saving to %s in slot %c...\n",
	        (selection & SELECTION_TYPE_SD) ? "SD card" : "memory card",
	        (selection & SELECTION_SLOT_B)  ? 'B'       : 'A');
#endif
	
	// Determine savepath
	if(selection & SELECTION_TYPE_SD){
		SDCARD_Init();
		if(selection & SELECTION_SLOT_B)
			strcpy(savepath, "dev1:\\N64SAVES\\");
		else
			strcpy(savepath, "dev0:\\N64SAVES\\");
		inited = TRUE;
	} else {
		CARD_Init("N64E", "OS");
		savepath[0] = 0;
		if(CARD_Mount((selection&SELECTION_SLOT_B) ? CARD_SLOTB : CARD_SLOTA,
			                             SysArea, card_removed_cb) < 0){
#ifdef USE_GUI
				
#else
			printf("Cannot mount Memory Card in Slot %c\n",
			        (selection&SELECTION_SLOT_B) ? 'B' : 'A');
#endif
		} else inited = TRUE;
	}
	}
	
	savetype = selection;
}