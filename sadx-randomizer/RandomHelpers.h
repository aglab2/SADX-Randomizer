#pragma once

extern int character[6];
extern int level[18];
extern struct RandomizedEntry randomizedSets[10];
extern int CharacterCopy;

uint8_t getRandomCharacter(bool allow_duplicate = false);
void randomstage(char stage, char act);

void SetGammaTimer(char param_1, char param_2);
void DisableTimeStuff();

void CancelResetPosition();

struct RandomizedEntry
{
	short character;
	short act;
	short level;
};

VoidFunc(PauseQuitDisplay, 0x415450);
VoidFunc(Chaos6Thing, 0x55a19c);
VoidFunc(TwinklePark_NextActAmy, 0x61d1ad);


DataPointer(char, BossHP, 0x3C58150);
DataPointer(char, Emblem, 0x974AE0);
DataPointer(unsigned char, LevelList, 0x3B2C5F8);
DataPointer(unsigned char, SelectedCharacter, 0x3B2A2FD);
DataPointer(char, RNGDoor, 0x3C7457C);
DataPointer(int, BigWeight, 0x3C52510);
