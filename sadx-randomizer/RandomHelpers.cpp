#include <algorithm>
#include "SADXVariables.h"
#include "SADXEnums.h"
#include "RandomHelpers.h"
#include "Utils.h"
#include "MemAccess.h"
#include "SADXFunctions.h"
#include "MemAccess.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define IS_LEVEL_IN_ARRAY(arr, val) (isValueInArray(arr, (enum LevelIDs)val, ARRAY_SIZE(arr)))
#define PICK_ARRAY_RAND(arr) (arr[rand() % ARRAY_SIZE(arr)])

extern bool RNGCharacters;
extern bool RNGStages;
extern bool Regular;

//Set up several arrays, the "level" one doesn't include some specific stages impossible to beat.
static const enum Characters character[] = {
	Characters_Sonic,
	Characters_Tails,
	Characters_Knuckles,
	Characters_Amy,
	Characters_Gamma,
	Characters_Big,
};

static enum Characters CharacterCopy = Characters_Eggman; //Used to avoid getting the same character twice in a row.
static const enum LevelIDs level[] = {
	LevelIDs_EmeraldCoast,
	LevelIDs_WindyValley,
	LevelIDs_TwinklePark,
	LevelIDs_SpeedHighway,
	LevelIDs_RedMountain,
	LevelIDs_SkyDeck,
	LevelIDs_LostWorld,
	LevelIDs_IceCap,
	LevelIDs_Casinopolis,
	LevelIDs_FinalEgg,
	LevelIDs_HotShelter,
	LevelIDs_Chaos0,
	LevelIDs_Chaos2,
	//LevelIDs_Chaos4,
	//LevelIDs_Chaos6,
	LevelIDs_EggHornet,
	LevelIDs_EggWalker,
	LevelIDs_EggViper,
	LevelIDs_Zero,
	//LevelIDs_E101,
	//LevelIDs_E101R,
	LevelIDs_SandHill,
};

static const int door[] = { 0, 1, 2, 3, 4 }; //set up the 5 doors RNG at Final Egg.

//Initialize Ban few stage impossible to beat, depending on the character.
static const enum LevelIDs bannedLevelsGamma[] = {
	LevelIDs_TwinklePark,
	LevelIDs_Chaos0,
	LevelIDs_Chaos2,
	LevelIDs_Chaos4,
	LevelIDs_Chaos6,
	LevelIDs_EggHornet,
	LevelIDs_EggWalker,
	//LevelIDs_EggViper, 
	LevelIDs_Zero
};
static const enum LevelIDs bannedLevelsBig[] = {
	LevelIDs_IceCap,
	LevelIDs_EggViper
};

//Initiliaze banned regular stage if option is activated
static const enum LevelIDs bannedRegularSonic[] = {
	LevelIDs_WindyValley,
	LevelIDs_RedMountain,
	LevelIDs_SkyDeck,
	LevelIDs_LostWorld,
	LevelIDs_IceCap,
	LevelIDs_Chaos0,
	LevelIDs_Chaos4,
	LevelIDs_EggHornet,
	LevelIDs_EggViper
};
static const enum LevelIDs bannedRegularTails[] = {
	LevelIDs_SkyDeck,
	LevelIDs_Chaos4,
	LevelIDs_EggHornet,
	LevelIDs_EggWalker,
	LevelIDs_SandHill
};
static const enum LevelIDs bannedRegularKnuckles[] = {
	LevelIDs_Chaos2,
	LevelIDs_Chaos4
};
static const enum LevelIDs bannedRegularAmy[] = {
	LevelIDs_Zero
};
static const enum LevelIDs bannedRegularBig[] = {
	LevelIDs_B,
};
static const enum LevelIDs bannedRegularGamma[] = {
	LevelIDs_B,
};

//function Credits
void startcredits() {

	switch (SelectedCharacter)
	{
	case 0:
		EventFlagArray[EventFlags_SonicAdventureComplete] = true;
		EventFlagArray[EventFlags_TailsUnlockedAdventure] = true;
		break;
	case 1:
		EventFlagArray[EventFlags_TailsAdventureComplete] = true;
		EventFlagArray[EventFlags_KnucklesUnlockedAdventure] = true;
		break;
	case 2:
		EventFlagArray[EventFlags_KnucklesAdventureComplete] = true;
		EventFlagArray[EventFlags_AmyUnlockedAdventure] = true;
		break;
	case 3:
		EventFlagArray[EventFlags_AmyAdventureComplete] = true;
		EventFlagArray[EventFlags_BigUnlockedAdventure] = true;
		break;
	case 4:
		EventFlagArray[EventFlags_BigAdventureComplete] = true;
		EventFlagArray[EventFlags_GammaUnlockedAdventure] = true;
		break;
	case 5:
		EventFlagArray[EventFlags_GammaAdventureComplete] = true;
		EventFlagArray[EventFlags_SuperSonicUnlockedAdventure] = true;
		break;
	case 6:
		EventFlagArray[EventFlags_SuperSonicAdventureComplete] = true;
		break;
	}

	GameMode = GameModes_StartCredits;
	GameState = 21;
	Credits_State = 1;
	Load_SEGALOGO_E();
}

static bool ShouldReroll(bool regular)
{
	// Don't roll the same level
	if (CurrentLevel == LevelCopy)
		return true;

	// Don't roll boss after boss
	if (CurrentLevel > 14 && LevelCopy > 14)
		return true;

	// Banned Levels
	switch (CurrentCharacter)
	{
	case Characters_Big:
		if (IS_LEVEL_IN_ARRAY(bannedLevelsBig, CurrentLevel))
			return true;
		break;
	case Characters_Gamma:
		if (IS_LEVEL_IN_ARRAY(bannedLevelsGamma, CurrentLevel))
			return true;
		break;
	}

	// Check for regular
	if (regular)
	{
		switch (CurrentCharacter)
		{
		case Characters_Sonic:
			if (IS_LEVEL_IN_ARRAY(bannedRegularSonic, CurrentLevel))
				return true;
			break;
		case Characters_Tails:
			if (IS_LEVEL_IN_ARRAY(bannedRegularTails, CurrentLevel))
				return true;
			break;
		case Characters_Knuckles:
			if (IS_LEVEL_IN_ARRAY(bannedRegularKnuckles, CurrentLevel))
				return true;
			break;
		case Characters_Amy:
			if (IS_LEVEL_IN_ARRAY(bannedRegularAmy, CurrentLevel))
				return true;
			break;
		case Characters_Big:
			if (IS_LEVEL_IN_ARRAY(bannedRegularBig, CurrentLevel))
				return true;
			break;
		case Characters_Gamma:
			if (IS_LEVEL_IN_ARRAY(bannedRegularGamma, CurrentLevel))
				return true;
			break;
		}
	}

	return false;
}

static void FixAreaAndModel()
{
	MetalSonicFlag = 0;

	if (CurrentLevel == LevelIDs_FinalEgg)
	{
		RNGDoor = PICK_ARRAY_RAND(door);
	}

	switch (CurrentCharacter)
	{
	case Characters_Sonic:
		MetalSonicFlag = rand() % 2;
		if (CurrentLevel == LevelIDs_TwinklePark || CurrentLevel == LevelIDs_Casinopolis)
		{
			CurrentAct = 1;
		}
		else if (CurrentLevel == LevelIDs_EmeraldCoast)
		{
			CurrentAct = 2;
		}
		break;
	case Characters_Amy:
		if (CurrentLevel == LevelIDs_HotShelter)
		{
			CurrentAct = 2;
		}
		break;
	}
}


//function Randomize stage and characters
void randomstage(char stage, char act) 
{
	LastLevel = CurrentLevel;
	LastAct = CurrentAct;

	// TODO: This expects you to go from one story to another...
	// Could be cheesed if you replay the story
	if (Emblem == 3 || Emblem == 16 || Emblem == 22 || Emblem == 26 || Emblem == 31 || Emblem == 37 || Emblem == 39)
	{
		startcredits();
		return;
	}

	if (RNGCharacters)
	{
		do 
		{
			CurrentCharacter = PICK_ARRAY_RAND(character);
		} while (CurrentCharacter == CharacterCopy);
	}

	if (RNGStages)
	{
		// TODO: Document
		if (GameMode != 8)
		{
			do
			{
				CurrentLevel = PICK_ARRAY_RAND(level);
			}
			while (ShouldReroll(Regular));

			FixAreaAndModel();
		}
		else
		{
			// TODO: This can't logically happen
			if (RNGStages == false && Regular == true) //It's technically useless, but I need to make some test later.
			{
				return;
			}
		}
	}
}

//cancel the reset position at 0 after quitting a stage.
void CancelResetPosition() {
	NextLevel = LastLevel;
	NextAct = LastAct;
	GameMode = GameModes_Adventure_Field;
}


//Set Gamma's Timer to 6 min instead of 3.
void SetGammaTimer(char param_1, char param_2) {
	param_1 = 6;
	param_2 = 0;
	TimeFrames = 0;
	TimeMinutes = param_1;
	TimeSeconds = param_2;
	return;
}

//Avoid getting the same character twice in a row and "fix" metal sonic emblems.
void DisableTimeStuff() {
	TimeThing = 0;
	CharacterCopy = (Characters) CurrentCharacter;
	MetalSonicFlag = 0;
	return;
}		