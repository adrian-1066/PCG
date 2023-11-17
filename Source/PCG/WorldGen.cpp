// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"
#include "BaseCube.h"
// Sets default values
AWorldGen::AWorldGen()
{

	chunkXSize = 8;
	ChunkYSize = 8;
	ChunkZSize = 2;
 
	worldSizeX = 10;
	worldSizeY = 10;

	WorldArray.SetNum(worldSizeX);

	GrassBiome = 0;
	SandBiome = 1;
	StoneBiome = 2;
	SnowBiome = 3;

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
	//SetNeighbours(0,0);
	CellularAutomata();

	if(WorldArray[0][0].Neighbours.Num() == 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("00 has 3 chunks near it"));
	}
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
							World->SpawnActor<AActor>(Test[WorldArray[q][w].ChunkType], SpawnLocation, SpawnRotation, SpawnParams);
							/*

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
							}*/
							

							
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
	int chunkType = WorldArray[x][y].ChunkType;
	for(int i = 0; i < ChunkZSize; i++)
	{
		FSecondArray SecondArray;
		for(int z = 0; z < ChunkYSize; z++)
		{
			FArrayOne firstArray;

			for(int q = 0; q < chunkXSize; q++)
			{
				firstArray.FirstArray.Add(chunkType);

				
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
	FChunkTypeAndStrength chunkTypeStr;
	for(int x = 0; x < worldSizeX; x++)
	{
		for (int y = 0; y < worldSizeY; y++)
		{
			if(onStartingChunk)
			{
				onStartingChunk = false;
				WorldArray[x][y].CanDecrease = false;
				WorldArray[x][y].ChunkStrength = 5;
				int RandomNum = FMath::RandRange(0,3);
				WorldArray[x][y].ChunkType = RandomNum;
			}
			else
			{
				WorldArray[x][y].CanDecrease = true;
			}

			WorldArray[x][y].IsAlive = true;
			WorldArray[x][y].HasBeenGenerated = true;
			WorldArray[x][y].ChanceToGetWeaker = 25.0f;
			chunkTypeStr = ChunkStrengthCalculator(x,y);
			WorldArray[x][y].ChunkStrength = chunkTypeStr.chunkStr;
			WorldArray[x][y].ChunkType = chunkTypeStr.chunkType;
		}
	}
}

FChunkTypeAndStrength AWorldGen::ChunkStrengthCalculator(int x, int y)
{
	FChunkTypeAndStrength newStrength;
	TArray<FChunkTypeAndStrength> typeAndStr;
	FChunk currentChunk = WorldArray[x][y];

	int typeGrassStr = 0;//0
	int typeSandStr = 0;//1
	int typeSnowStr = 0;//3
	int typeStoneStr = 0;//2

	for(int i = 0; i < currentChunk.Neighbours.Num(); i++)
	{
		if(WorldArray[currentChunk.Neighbours[i].x][currentChunk.Neighbours[i].y].IsAlive)
		{
			FChunkTypeAndStrength tempTypeStr;
			tempTypeStr.chunkType = WorldArray[currentChunk.Neighbours[i].x][currentChunk.Neighbours[i].y].ChunkType;
			tempTypeStr.chunkStr = WorldArray[currentChunk.Neighbours[i].x][currentChunk.Neighbours[i].y].ChunkStrength;
			if(tempTypeStr.chunkStr != 0)
			{
				typeAndStr.Add(tempTypeStr);
			}
			
		}
	}

	if(typeAndStr.Num() == 0)
	{
		//needs to be a new chunk
		int newChunkType = FMath::RandRange(0,3);
		FChunkTypeAndStrength newChunk;
		newChunk.chunkStr = 5;
		newChunk.chunkType = newChunkType;

		return newChunk;
	}

	for(int z = 0; z < typeAndStr.Num(); z++)
	{
		int currentType = typeAndStr[z].chunkType;
		switch (currentType)
		{
			case(0):
				typeGrassStr += typeAndStr[z].chunkStr;
				break;
			case(1):
				typeSandStr += typeAndStr[z].chunkStr;
				break;
			case(2):
				typeStoneStr += typeAndStr[z].chunkStr;
				break;
			case(3):
				typeSnowStr += typeAndStr[z].chunkStr;
				break;
		}
	}


	FChunkTypeAndStrength currentStrongestType;
	currentStrongestType.chunkType = 0;
	currentStrongestType.chunkStr = typeGrassStr;

	for(int i = 0; i < 4; i++)
	{
		switch (i)
		{
			case(0):
				
				break;
			case(1):
				if(typeSandStr > currentStrongestType.chunkStr)
				{
					currentStrongestType.chunkType = 1;
					currentStrongestType.chunkStr = typeSandStr;
				}
				break;
			case(2):
				if(typeStoneStr > currentStrongestType.chunkStr)
				{
					currentStrongestType.chunkType = 2;
					currentStrongestType.chunkStr = typeStoneStr;
				}
				break;
			case(3):
				if(typeSnowStr > currentStrongestType.chunkStr)
				{
					currentStrongestType.chunkType = 3;
					currentStrongestType.chunkStr = typeSnowStr;
				}
				break;
		}
	}

	int totalTypes = 0;

	for(int z = 0; z < currentChunk.Neighbours.Num(); z++)
	{
		if(currentStrongestType.chunkType == WorldArray[currentChunk.Neighbours[z].x][currentChunk.Neighbours[z].y].ChunkType)
		{
			totalTypes++;
		}
	}

	float averageStr = currentStrongestType.chunkStr / totalTypes;
	int decreaseChance = FMath::RandRange(0,1);
	if(decreaseChance == 1)
	{
		averageStr--;
		if(averageStr < 0)
		{
			averageStr = 0;
		}
	}
	newStrength.chunkStr = averageStr;
	newStrength.chunkType = currentStrongestType.chunkType;

		
	
	

	return newStrength;
}

bool AWorldGen::IsOutOfBounds(int x, int y)
{
	UE_LOG(LogTemp, Warning, TEXT("is out of bounds started"));
	if(x < 0 || x >= worldSizeX)
	{
		UE_LOG(LogTemp, Warning, TEXT("is out of bounds"));
		return true;
	}

	if(y < 0 || y >= worldSizeY)
	{
		UE_LOG(LogTemp, Warning, TEXT("is out of bounds"));
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("is not out of bounds"));

	return false;
}

void AWorldGen::SetNeighbours(int x, int y)
{
	int tempX;
	int tempY;

	for(int i = 0; i < 8; i++)
	{
		
		switch (i)
		{
		case 0:
			UE_LOG(LogTemp, Warning, TEXT("checking 0"));
			tempX = x -1;
			tempY = y -1;
			break;

		case 1:
			UE_LOG(LogTemp, Warning, TEXT("checking 1"));
			tempX = 0;
			tempY = y - 1;
			
			break;
		case 2:
			UE_LOG(LogTemp, Warning, TEXT("checking 2"));
			tempX = x + 1;
			tempY = y - 1;
			
			break;
		case 3:
			UE_LOG(LogTemp, Warning, TEXT("checking 3"));
			tempX = x - 1;
			tempY = y;
			
			break;
		case 4:
			UE_LOG(LogTemp, Warning, TEXT("checking 4"));
			tempX = x + 1;
			tempY = y;
			
			break;
		case 5:
			UE_LOG(LogTemp, Warning, TEXT("checking 5"));
			tempX = x - 1;
			tempY = y + 1;
			
			break;
		case 6:
			UE_LOG(LogTemp, Warning, TEXT("checking 6"));
			tempX = x;
			tempY = y + 1;
			
			break;
		case 7:
			UE_LOG(LogTemp, Warning, TEXT("checking 7"));
			tempX = x + 1;
			tempY = y + 1;
			
			break;
		}

		bool isOutOfRange = IsOutOfBounds(tempX, tempY);
		if(!isOutOfRange)
		{
			FCoOrds tempCoOrds;
			tempCoOrds.x = tempX;
			tempCoOrds.y = tempY;
			WorldArray[x][y].Neighbours.Add(tempCoOrds);
		}
		
	}
}





// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

