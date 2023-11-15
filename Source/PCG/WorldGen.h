// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "customStructs.h"
#include "BaseCube.h"
#include "WorldGen.generated.h"

UCLASS()
class PCG_API AWorldGen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldGen();
	
	TArray<TArray<FChunk>> WorldArray;
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

	void GenerateChunks();
	void PopulateChunk(int x, int y);
	void SpawnCube();
	void CellularAutomata();

	UPROPERTY(EditAnywhere, BlueprintReadWrite )
	TArray<TSubclassOf<AActor>> Test;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
