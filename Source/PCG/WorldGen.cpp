// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"
#include "BaseCube.h"
// Sets default values
AWorldGen::AWorldGen()
{

	chunkXSize = 8;
	ChunkYSize = 8;
	ChunkZSize = 2;

	worldSizeX = 4;
	worldSizeY = 4;

	WorldArray.SetNum(worldSizeX);

	for(int x = 0; x < worldSizeX; x++)
	{
		WorldArray[x].SetNum(worldSizeY);
	}
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorldGen::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateChunks();
	SpawnCube();
}

void AWorldGen::GenerateChunks()
{
	for(int i = 0; i < worldSizeX; i++)
	{
		for(int o = 0; o < worldSizeY; o++)
		{
			PopulateChunk(i,o);
		}
	}
}

void AWorldGen::SpawnCube()
{
	UWorld* const World = GetWorld();

	if(World)
	{
		UE_LOG(LogTemp, Warning, TEXT("world is true"));
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		for(int q = 0; q < worldSizeX; q++)
		{
			for(int w = 0; w < worldSizeY; w++)
			{
				int ChunkXCoOrd = (q * chunkXSize)*100;
				int ChunkYCoOrd = (w * ChunkYSize)*100;
				for(int e = 0; e < ChunkZSize; e++)
				{
					for(int r = 0; r < ChunkYSize; r++)
					{
						for(int t = 0; t < chunkXSize; t++)
						{
							//UE_LOG(LogTemp, Warning, TEXT("Array Size: %d"), WorldArray[q][w].ZArray.Num());
							FChunk Temp;
							FSecondArray ZArrayTemp;
							FArrayOne XArrayTemp;
							Temp = WorldArray[q][w];
							ZArrayTemp = Temp.ZArray[e];
							XArrayTemp = ZArrayTemp.SecondArray[r];

							//int ChunkXCoOrd = q * chunkXSize;
							//int ChunkYCoOrd = w * ChunkYSize;

							FVector SpawnLocation = FVector(ChunkXCoOrd + (t*100),ChunkYCoOrd + (r*100),(e*100));
							FRotator SpawnRotation = FRotator(0.0f,0.0f,0.0f);

							bool isEven;
							int evenCheck = q + w;
							if(evenCheck % 2 == 0)
							{
								isEven = true;
							}
							else
							{
								isEven = false;
									
								
							}

							if(isEven)
							{
								World->SpawnActor<AActor>(Test[1], SpawnLocation, SpawnRotation, SpawnParams);
							}
							else
							{
								World->SpawnActor<AActor>(Test[0], SpawnLocation, SpawnRotation, SpawnParams);	
							}
							

							
						}
					}
				}
			}
		}

		//FVector SpawnLocation = FVector(0.0f,0.0f,0.0f);
		// SpawnRotation = FRotator(0.0f,0.0f,0.0f);

		//World->SpawnActor<AActor>(Test, SpawnLocation, SpawnRotation, SpawnParams);
	}
}


void AWorldGen::PopulateChunk(int x, int y)
{

	
	FChunk zArray;
	for(int i = 0; i < ChunkZSize; i++)
	{
		FSecondArray SecondArray;
		for(int z = 0; z < ChunkYSize; z++)
		{
			FArrayOne firstArray;

			for(int q = 0; q < chunkXSize; q++)
			{
				firstArray.FirstArray.Add(0);

				
				//UE_LOG(LogTemp, Warning, TEXT("Element at index %d: %d"), q, firstArray.FirstArray[q]);
			}
			SecondArray.SecondArray.Add(firstArray);
		}
		zArray.ZArray.Add(SecondArray);
	}
	WorldArray[x][y] = zArray;
}

void AWorldGen::CellularAutomata()
{
	//this is stage one
	/*
	 *rules for chunks
	 *if first chunk then set strength to 5 and that the chunks around it cant get weaker
	 *have the chance increase after every new chunk for the next chunk to decrease in strength
	 *when strength is 0 that chunk is a border chunk
	 */
	bool onStartingChunk = true;
	for(int x = 0; x < worldSizeX; x++)
	{
		for (int y = 0; y < worldSizeY; y++)
		{
			if(onStartingChunk)
			{
				onStartingChunk = false;
			}
		}
	}
}


// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

