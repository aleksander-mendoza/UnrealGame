// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


namespace ItemSlot 
{
	const int32 NONE = 0;  // not a clothing item 00000000000000000000000000000000 
	const int32 STOMACH = 1;  // stomach 00000000000000000000000000000001 
	const int32 HEAD = 2;  // head 00000000000000000000000000000010 
	const int32 PANTY = 4;  // panty 00000000000000000000000000000100 
	const int32 NECK = 8;  // neck 00000000000000000000000000001000 
	const int32 SHOULDERS = 16;  // shoulders 00000000000000000000000000010000 
	const int32 LEFT_FOOT = 32;  // left foot  00000000000000000000000000100000 
	const int32 RIGHT_FOOT = 64;  // right foot  00000000000000000000000001000000 
	const int32 LEFT_HAND = 128;  // left hand 00000000000000000000000010000000 
	const int32 RIGHT_HAND = 256;  // right hand 00000000000000000000000100000000 
	const int32 LEFT_THIGH = 512;  // left thigh 00000000000000000000001000000000 
	const int32 RIGHT_THIGH = 1024;  // right thigh 00000000000000000000010000000000 
	const int32 LEFT_SHIN = 2048;  // left shin 00000000000000000000100000000000 
	const int32 RIGHT_SHIN = 4096;  // right shin 00000000000000000001000000000000 
	const int32 LEFT_FOREARM = 8192;  // left forearm 00000000000000000010000000000000 
	const int32 RIGHT_FOREARM = 16384;  // right forearm   00000000000000000100000000000000 
	const int32 LEFT_BREAST = 32768;  // left breast  00000000000000001000000000000000 
	const int32 RIGHT_BREAST = 65536;  // right breast 00000000000000010000000000000000 
	const int32 BACK = 131072;  // back 00000000000000100000000000000000 
	const int32 LEFT_RING = 262144;  // left ring 00000000000001000000000000000000 
	const int32 RIGHT_RING = 524288;  // right ring 00000000000010000000000000000000 
	const int32 LEFT_BRACELET = 1048576;  // left bracelet 00000000000100000000000000000000 
	const int32 RIGHT_BRACELET = 2097152;  // right bracelet 00000000001000000000000000000000 
	const int32 WING = 4194304;  // wing 00000000010000000000000000000000 
	const int32 HOOD = 8388608;  // hood 00000000100000000000000000000000 
	const int32 FACE = 16777216;  // face 00000001000000000000000000000000 
	//__1 =  33554432  UMETA(DisplayName = "... 00000010000000000000000000000000 
	//__2 =  67108864 UMETA(DisplayName = "... 00000100000000000000000000000000 
	//__3 =  134217728 UMETA(DisplayName = "... 00001000000000000000000000000000 
	//__4 =  268435456 UMETA(DisplayName = "... 00010000000000000000000000000000 
	//__5 =  536870912 UMETA(DisplayName = "... 00100000000000000000000000000000 
	//__6 =  1073741824 UMETA(DisplayName = "... 01000000000000000000000000000000 
	const int32 FREE = -2147483648; //can be freely combined without colliding 10000000000000000000000000000000  
	const int32 LEFT_ARM = 8320; // left arm 128+8192 = hand+forearm
	const int32 RIGHT_ARM = 16640; // right arm 256+16384 = hand+forearm
	const int32 LEFT_LEG = 2592; // left leg 32+512+2048 = foot+shin+thigh
	const int32 RIGHT_LEG = 5184; // right leg 64+1024+4096 = foot+shin+thigh
	const int32 FULL_BOTTOM = 7780; // full bottom 4+32+512+2048+64+1024+4096 = foot+shin+thigh+panties
	const int32 BOTTOM_WITHOUT_FEET = 7684; // bottom without feet 4+512+2048+1024+4096 = shin+thigh+panties
	const int32 BREASTS = 98304; // breasts 32768+65536 = left and right breast
	const int32 BOTH_ARMS = 24976; // both arms 16+128+8192+256+16384 = hand+forearm+shoulders
	const int32 TORSO_WITHOUT_SHOULDERS = 229377; // torso without shoulders 1+32768+65536+131072 = breasts+stomach+back
	const int32 FULL_TORSO = 229393; // full torso 1+16+32768+65536+131072 = breasts+stomach+back+shoulders
	const int32 LEOTARD = 229381; // leotard 1+32768+65536+131072+4 = breasts+stomach+back+panties
	const int32 UPPER_BODY = 254353; // upper body 1+16+32768+65536+131072+128+8192+256+16384 = breasts+stomach+back+shoulders+hand+forearm
	
};
