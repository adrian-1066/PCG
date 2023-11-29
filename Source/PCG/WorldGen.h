// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "customStructs.h"
#include "Math/UnrealMathUtility.h"
#include "BaseCube.h"
#include "Field/FieldSystemNoiseAlgo.h"
#include "WorldGen.generated.h"

UCLASS()
class PCG_API AWorldGen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGen();
	
	TArray<TArray<FChunk>> WorldArray;
	TArray<FBiome> Biomes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite )
	int32 chunkXSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite )
	int32 ChunkYSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite )
	int32 ChunkZSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite )
	int32 worldSizeX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite )
	int32 worldSizeY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int biomeCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int blocksSpawned;

	int RandomSeed;
	int MaxNoiseScale;

	int GrassBiome;
	int SandBiome;
	int StoneBiome;
	int SnowBiome;
	float caveGenChacne;

	

	void GenerateChunks();
	void PopulateChunk(int x, int y);
	void SpawnCube();
	bool CheckForAir(int worldX, int worldY, int chunkX, int chunkY, int chunkZ);
	int BlockTypeAt(int worldX, int worldY, int chunkX, int chunkY, int chunkZ);
	void CellularAutomata();
	FChunkTypeAndStrength ChunkStrengthCalculator(int x, int y);
	bool IsOutOfBounds(int x, int y);
	void SetNeighbours(int x, int y);

	void EstablishBiomes();
	bool IsAdjacent(const FCoOrds& ChunkA,const FCoOrds& ChunkB);
	bool AreBiomesTouching(const FBiome& BiomeA, const FBiome& BiomeB);
	void BiomeMerge();
	void ChunkAdjustment();

	void PerlinNoiseStart();
	void CaveNoiseGenerator();

	void chunkMergeAdj();
	void chunkAdjLerp(FCoOrds chunkOne, float lowNoise , FCoOrds dir);
	void BlockHeightChange();

	UPROPERTY(EditAnywhere, BlueprintReadWrite )
	TArray<TSubclassOf<AActor>> Test;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
