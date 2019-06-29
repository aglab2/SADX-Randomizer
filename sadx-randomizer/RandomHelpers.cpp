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
	//LevelIDs_SandHill,
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
	LevelIDs_Zero,
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
	LevelIDs_FinalEgg,
	LevelIDs_Chaos0,
	LevelIDs_Chaos4,
	LevelIDs_Chaos6,
	LevelIDs_EggHornet,
	LevelIDs_EggViper
};
static const enum LevelIDs bannedRegularTails[] = {
	LevelIDs_SpeedHighway,
	LevelIDs_SkyDeck,
	LevelIDs_Chaos4,
	LevelIDs_EggHornet,
	LevelIDs_EggWalker,
	LevelIDs_SandHill
};
static const enum LevelIDs bannedRegularKnuckles[] = {
	LevelIDs_Chaos2,
	LevelIDs_Chaos4,
	LevelIDs_Chaos6,
};
static const enum LevelIDs bannedRegularAmy[] = {
	LevelIDs_Zero
};
static const enum LevelIDs bannedRegularBig[] = {
	LevelIDs_Chaos4,
};
static const enum LevelIDs bannedRegularGamma[] = {
	LevelIDs_Chaos2,
};

// Linear accessors
static constexpr int getIdx(enum LevelIDs lvl)
{
	return findValueInArray(level, lvl, ARRAY_SIZE(level));
}

static constexpr int getIdx(enum Characters chr)
{
	return findValueInArray(character, chr, ARRAY_SIZE(character));
}

static bool shownLevelCombos[ARRAY_SIZE(level)][ARRAY_SIZE(character)] = { false };
void shownLevelsInit()
{
#define SET_SHOWN_FROM_BANNED(prefix, ch) for (int i = 0; i < ARRAY_SIZE(banned##prefix##ch); i++) { int lvl = getIdx(banned##prefix##ch[i]); if (lvl == -1) continue; shownLevelCombos[lvl][getIdx(Characters_##ch)] = true; }

	SET_SHOWN_FROM_BANNED(Levels, Big)
	SET_SHOWN_FROM_BANNED(Levels, Gamma)

	if (Regular)
	{
		SET_SHOWN_FROM_BANNED(Regular, Sonic)
		SET_SHOWN_FROM_BANNED(Regular, Tails)
		SET_SHOWN_FROM_BANNED(Regular, Knuckles)
		SET_SHOWN_FROM_BANNED(Regular, Amy)
		SET_SHOWN_FROM_BANNED(Regular, Big)
		SET_SHOWN_FROM_BANNED(Regular, Gamma)
	}
#undef SET_SHOWN_FROM_BANNED
}

static bool shownLevels[ARRAY_SIZE(level)] = { false };
bool areAllLevelShown()
{
	for (int i = 0; i < ARRAY_SIZE(level); i++)
		if (!shownLevels[i])
			return false;

	return true;
}

void dropAllLevelShown()
{
	for (int i = 0; i < ARRAY_SIZE(level); i++)
		shownLevels[i] = false;
}

template<typename T, size_t SIZE, typename AllowFunctor>
int PickRandomIdx(T(&arr)[SIZE], AllowFunctor allowed)
{
	int allowedCount = 0;
	for (int idx = 0; idx < SIZE; idx++)
		if (allowed(idx))
			allowedCount++;

	int id = rand() % allowedCount;
	int idx = 0;

	// Find first one that could be considered
	while (!allowed(idx)) idx++;

	for (int i = 0; i < id; i++)
	{
		// Find the next one
		idx++;
		while (!allowed(idx)) idx++;
	}

	return idx;
}

static enum LevelIDs RandomLevel()
{
	int lvlIdx = PickRandomIdx(level, [](int lvlIdx) {
		return !shownLevels[lvlIdx];
	});

	shownLevels[lvlIdx] = true;
	return level[lvlIdx];
}

static enum Characters RandomCharacter(enum LevelIDs level)
{
	int lvlIdx = getIdx(level);
	int shownChrIdx = getIdx(CharacterCopy);

	int chrIdx = PickRandomIdx(character, [&shownChrIdx, &lvlIdx](int chrIdx) {
		return !shownLevelCombos[lvlIdx][chrIdx] && chrIdx != shownChrIdx;
	});

	//allowedLevelCombos[lvlIdx][chrIdx] = true;
	return character[chrIdx];
}

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

static void FixAreaAndModel()
{
	CurrentAct = 0;
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
	/*
	if (Emblem == 3 || Emblem == 16 || Emblem == 22 || Emblem == 26 || Emblem == 31 || Emblem == 37 || Emblem == 39)
	{
		startcredits();
		return;
	}
	*/
	if (areAllLevelShown())
	{
		startcredits();
		dropAllLevelShown();
		return;
	}

	if (RNGCharacters && RNGStages)
	{
		if (GameMode != GameModes_Movie && GameMode != GameModes_StartAdventure && GameMode != GameModes_Menu)
		{
			CurrentLevel = RandomLevel();
			CurrentCharacter = RandomCharacter((enum LevelIDs)CurrentLevel);
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

		return;
	}

	if (RNGCharacters)
	{
		CurrentCharacter = RandomCharacter((enum LevelIDs)CurrentLevel);
		FixAreaAndModel();
	}

	if (RNGStages)
	{
		// TODO: Document
		if (GameMode != GameModes_Movie && GameMode != GameModes_StartAdventure && GameMode != GameModes_Menu)
		{
			CurrentLevel = RandomLevel();
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