// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <iostream>
#include "UObject/ObjectMacros.h"

#include "customStructs.generated.h"


USTRUCT(BlueprintType)
struct PCG_API FArrayOne
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category="someshit")
	TArray<int32> FirstArray;

	
};

USTRUCT(BlueprintType)
struct PCG_API FSecondArray
{
	
	GENERATED_BODY()
	TArray<FArrayOne> SecondArray;
	
};

USTRUCT(BlueprintType)
struct FCoOrds
{
	GENERATED_BODY()
	int x;
	int y;
	
};

USTRUCT(BlueprintType)
struct FChunkTypeAndStrength
{
	GENERATED_BODY()
	int chunkType;
	int chunkStr;
	
};

USTRUCT(BlueprintType)
struct FChunk
{
	GENERATED_BODY()
	TArray<FSecondArray> ZArray;
	int ChunkType;
	float ChunkStrength;
	float ChanceToGetWeaker;
	bool CanDecrease;
	bool StrengthDidDecrease;
	bool HasBeenGenerated;
	bool IsAlive;
	float ChunkNoiseScale;
	TArray<FCoOrds> Neighbours;
	FSecondArray Heights; 
	
};

USTRUCT(BlueprintType)
struct FBiome
{
	GENERATED_BODY()
	TArray<FChunk> BiomeChunks;
	TArray<FCoOrds> PositionInWorld;
	float BiomeStrength;
	float BiomeNoiseScale;
	int BiomeType;
	
};
