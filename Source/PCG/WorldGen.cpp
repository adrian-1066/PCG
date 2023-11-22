// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"

// Sets default values
AWorldGen::AWorldGen()
{

	chunkXSize = 6;
	ChunkYSize = 6;
	ChunkZSize = 11;
 
	worldSizeX = 10;
	worldSizeY = 10;
	biomeCount = 0;

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
	EstablishBiomes();
	BiomeMerge();
	//EstablishBiomes();
	BiomeMerge();
	//EstablishBiomes();
	BiomeMerge();
	BiomeMerge();
	BiomeMerge();
	//EstablishBiomes();
	//BiomeMerge();

	for(int i = 0; i < Biomes.Num(); i++)
	{
		int temp;
		temp = FMath::RandRange(1, 5);
		Biomes[i].BiomeNoiseScale = temp;
		//UE_LOG(LogTemp, Warning, TEXT("biome chunk scale is %f: "), Biomes[i].BiomeNoiseScale);
	}
	//PerlinNoiseStart();
	ChunkAdjustment();

	if(WorldArray[0][0].Neighbours.Num() == 3)
	{
		//UE_LOG(LogTemp, Warning, TEXT("00 has 3 chunks near it"));
	}
	biomeCount = Biomes.Num();
	for(int i = 0; i < Biomes.Num(); i++)
	{
		for(int x = 0; x < Biomes[i].BiomeChunks.Num(); x++)
		{
			//UE_LOG(LogTemp, Warning, TEXT("biome type then chunk type is %d: %d"), Biomes[i].BiomeType,Biomes[i].BiomeChunks[x].ChunkType);
		}
	}
	SpawnCube();
}

void AWorldGen::GenerateChunks()
{
	for(int i = 0; i < worldSizeX; i++)
	{
		for(int o = 0; o < worldSizeY; o++)
		{
			//float test = FMath::PerlinNoise2D(FVector2D(i + 0.1,o + 0.1));
			//test++;
			//UE_LOG(LogTemp, Warning, TEXT("perlin noise test value is: %f"), test);
			PopulateChunk(i,o);
		}
	}
}

void AWorldGen::SpawnCube()
{
	UWorld* const World = GetWorld();

	if(World)
	{
		//UE_LOG(LogTemp, Warning, TEXT("world is true"));
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
							bool canSpawn = CheckForAir(q,w,t,r,e);
							int blockIs = WorldArray[q][w].ZArray[e].SecondArray[r].FirstArray[t];
							if(blockIs != -1)
							{
								FVector SpawnLocation = FVector(ChunkXCoOrd + (t*100),ChunkYCoOrd + (r*100),(e*100));
								FRotator SpawnRotation = FRotator(0.0f,0.0f,0.0f);
								World->SpawnActor<AActor>(Test[WorldArray[q][w].ChunkType], SpawnLocation, SpawnRotation, SpawnParams);	
								if(canSpawn)
								{
									
								}
								
							}
							//UE_LOG(LogTemp, Warning, TEXT("element at world array x y z is: %d %d %d %d %d %d"),q,w,t,r,e, blockIs);
							
							if(canSpawn)
							{
								
							}
							else
							{
								//UE_LOG(LogTemp, Warning, TEXT("block no spawn"));
									
								
							}

							
							//UE_LOG(LogTemp, Warning, TEXT("Array Size: %d"), WorldArray[q][w].ZArray.Num());
							FChunk Temp;
							FSecondArray ZArrayTemp;
							FArrayOne XArrayTemp;
							Temp = WorldArray[q][w];
							ZArrayTemp = Temp.ZArray[e];
							XArrayTemp = ZArrayTemp.SecondArray[r];

							//int ChunkXCoOrd = q * chunkXSize;
							//int ChunkYCoOrd = w * ChunkYSize;

						
							if(!WorldArray[q][w].IsAlive)
							{
								
							}
							
							
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

bool AWorldGen::CheckForAir(int worldX, int worldY, int chunkX, int chunkY, int chunkZ)
{
	//needs to know if block is at border
	//if not at border then it needs to check its up, down, left, right, forward, backward
	//if at border it needs to check the adjacent border to see if visable
	bool airFound = false;
	int currentBlockTypeToCheck;

	for(int i = 0; i < 6; i++)
	{
		if(!airFound)
		{
			break;
		}
		switch(i)
		{
		case (0):
			if(chunkZ >= ChunkZSize)
			{
				airFound = true;
			}
			else
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX,worldY,chunkX,chunkY,chunkZ+1);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
					return true;
				}
					//do check
				
			}
			
			break;
		case (1):
			if(chunkZ <= 0)
			{
				airFound = true;
			}
			else
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX,worldY,chunkX,chunkY,chunkZ-1);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
				}
					
				
			}
			break;
		case(2):
			if(chunkY == 0)
			{
				//check world array x y-1
				currentBlockTypeToCheck = BlockTypeAt(worldX,worldY-1,chunkX,ChunkYSize-1,chunkZ);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
				}
			}
			else
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX,worldY,chunkX,chunkY-1,chunkZ);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
				}
					
				
			}
			
			break;
		case(3):
			if(chunkY >= ChunkYSize-1)
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX,worldY+1,chunkX,0,chunkZ);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
				}
				//check world array x y+1
			}
			else
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX,worldY,chunkX,chunkY+1,chunkZ);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
				}
			}
			
			break;
		case(4):
			if(chunkX <= 0)
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX-1,worldY,chunkXSize-1,chunkY,chunkZ);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
				}
				//check world array x-1 y
			}
			else
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX,worldY,chunkX-1,chunkY,chunkZ);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
				}
			}
			
			break;
		case(5):
			if(chunkX >= chunkXSize-1)
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX+1,worldY,0,chunkY,chunkZ);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
				}
				//check world array x+1 y
			}
			else
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX,worldY,chunkX + 1,chunkY,chunkZ);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
				}
					
			}
			
			break;
			
		}
		
	}
	return airFound;
		
}

int AWorldGen::BlockTypeAt(int worldX, int worldY, int chunkX, int chunkY, int chunkZ)
{
	if(IsOutOfBounds(worldX,worldY))
	{
		return -1;
	}
	return WorldArray[worldX][worldY].ZArray[chunkZ].SecondArray[chunkY].FirstArray[chunkX];
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
				firstArray.FirstArray.Add(-1);

				//UE_LOG(LogTemp, Warning, TEXT("Element at index %d: %d: %d"), i,z,q);
				
			}
			SecondArray.SecondArray.Add(firstArray);
		}
		zArray.ZArray.Add(SecondArray);

		
	}
	zArray.IsAlive = true;
	float randStr = FMath::RandRange(0,5);
	int randType = FMath::RandRange(0,3);
	FChunkTypeAndStrength tempType;
	tempType.chunkStr = randStr;
	tempType.chunkType = randType;
	zArray.ChunkStrength = randStr;
	zArray.ChunkType = randType;
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
				biomeCount++;
			}
			else
			{
				WorldArray[x][y].CanDecrease = true;
			}

			WorldArray[x][y].IsAlive = true;
			WorldArray[x][y].HasBeenGenerated = true;
			WorldArray[x][y].ChanceToGetWeaker = 25.0f;
			SetNeighbours(x,y);
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
		//UE_LOG(LogTemp, Log, TEXT("Iteration %d"), i)
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

	//UE_LOG(LogTemp, Log, TEXT("Size of TArray: %d"), typeAndStr.Num());

	if(typeAndStr.Num() == 0)
	{
		//needs to be a new chunk
		int newChunkType = FMath::RandRange(0,3);
		FChunkTypeAndStrength newChunk;
		biomeCount++;
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
	int decreaseChance = FMath::RandRange(0,100);
	if(decreaseChance <= 90)
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
	//UE_LOG(LogTemp, Warning, TEXT("is out of bounds started"));
	if(x < 0 || x >= worldSizeX)
	{
		//UE_LOG(LogTemp, Warning, TEXT("is out of bounds"));
		return true;
	}

	if(y < 0 || y >= worldSizeY)
	{
		//UE_LOG(LogTemp, Warning, TEXT("is out of bounds"));
		return true;
	}
	//UE_LOG(LogTemp, Warning, TEXT("is not out of bounds"));

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
			//UE_LOG(LogTemp, Warning, TEXT("checking 0"));
			tempX = x -1;
			tempY = y -1;
			break;

		case 1:
			//UE_LOG(LogTemp, Warning, TEXT("checking 1"));
			tempX = 0;
			tempY = y - 1;
			
			break;
		case 2:
			//UE_LOG(LogTemp, Warning, TEXT("checking 2"));
			tempX = x + 1;
			tempY = y - 1;
			
			break;
		case 3:
			//UE_LOG(LogTemp, Warning, TEXT("checking 3"));
			tempX = x - 1;
			tempY = y;
			
			break;
		case 4:
			//UE_LOG(LogTemp, Warning, TEXT("checking 4"));
			tempX = x + 1;
			tempY = y;
			
			break;
		case 5:
			//UE_LOG(LogTemp, Warning, TEXT("checking 5"));
			tempX = x - 1;
			tempY = y + 1;
			
			break;
		case 6:
			//UE_LOG(LogTemp, Warning, TEXT("checking 6"));
			tempX = x;
			tempY = y + 1;
			
			break;
		case 7:
			//UE_LOG(LogTemp, Warning, TEXT("checking 7"));
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

bool AWorldGen::IsAdjacent(const FCoOrds& ChunkA,const FCoOrds& ChunkB)
{
	const FChunk& ChunkAData = WorldArray[ChunkA.x][ChunkA.y];
	for (const FCoOrds& Neighbor : ChunkAData.Neighbours)
	{
		if (Neighbor.x == ChunkB.x && Neighbor.y == ChunkB.y)
		{
			return true;
		}
	}
	return false;

}


void AWorldGen::EstablishBiomes()
{
	for (int32 x = 0; x < worldSizeX; x++)
	{
		for (int32 y = 0; y < worldSizeY; y++)
		{
			bool bFoundInBiome = false;
			for (FBiome& Biome : Biomes)
			{
				if (Biome.BiomeType == WorldArray[x][y].ChunkType)
				{
					for (const FCoOrds& ChunkCoords : Biome.PositionInWorld)
					{
						if (IsAdjacent(ChunkCoords, FCoOrds{ x, y }))
						{
							Biome.PositionInWorld.Add(FCoOrds{ x, y });
							Biome.BiomeChunks.Add(WorldArray[x][y]);
							//Biome.BiomeStrength = 0;
							Biome.BiomeStrength += WorldArray[x][y].ChunkStrength;
							bFoundInBiome = true;
							break;
						}
					}
					if (bFoundInBiome)
						break;
				}
			}

			if (!bFoundInBiome)
			{
				FBiome NewBiome;
				NewBiome.BiomeType = WorldArray[x][y].ChunkType;
				NewBiome.PositionInWorld.Add(FCoOrds{ x, y });
				NewBiome.BiomeChunks.Add(WorldArray[x][y]);
				NewBiome.BiomeStrength = 0;
				NewBiome.BiomeStrength += WorldArray[x][y].ChunkStrength;
				Biomes.Add(NewBiome);
			}
		}
	}

	biomeCount = Biomes.Num();
}

void AWorldGen::BiomeMerge()
{
	int NumBiomes = Biomes.Num();

	for (int32 i = 0; i < NumBiomes; ++i)
	{
		for (int32 j = 0; j < NumBiomes; ++j)
		{
			if(j == i)
			{
				break;
			}
			if(j >= NumBiomes)
			{
				break;
			}
			if(i >= NumBiomes)
			{
				break;
			}
			const FBiome& BiomeA = Biomes[i];
			const FBiome& BiomeB = Biomes[j];

			if (AreBiomesTouching(BiomeA, BiomeB))
			{
				//int strengthCheck;
				bool canMerge = true;

				if(BiomeA.BiomeStrength >= 1000.0f)
				{
					canMerge = false;
				}
				else if(BiomeB.BiomeStrength >= 1000.0f)
				{
					canMerge = false;
				}

				if(canMerge)
				{
					//UE_LOG(LogTemp, Warning, TEXT("luuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuck"));
					TArray<FChunk> tempNewBiome;
					float newStr = 0;
					if(BiomeA.BiomeStrength > BiomeB.BiomeStrength)
					{
						//merge
						for(int x = 0; x < Biomes[j].BiomeChunks.Num(); x++)
						{
							//UE_LOG(LogTemp, Warning, TEXT("fuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuck"));
							
							tempNewBiome.Add(Biomes[j].BiomeChunks[x]);
							
						}

						for(int x = 0; x < Biomes[i].BiomeChunks.Num(); x++)
						{
							tempNewBiome.Add(Biomes[i].BiomeChunks[x]);
						}

						newStr = BiomeA.BiomeStrength + BiomeB.BiomeStrength;

						Biomes[i].BiomeChunks = tempNewBiome;
						Biomes[i].BiomeStrength = newStr;
						
						Biomes.RemoveAt(j);
						NumBiomes--;
					
					}
					else if(BiomeB.BiomeStrength > BiomeA.BiomeStrength)
					{
						for(int x = 0; x < Biomes[j].BiomeChunks.Num(); x++)
						{
							tempNewBiome.Add(Biomes[j].BiomeChunks[x]);
						}

						for(int x = 0; x < Biomes[i].BiomeChunks.Num(); x++)
						{
							tempNewBiome.Add(Biomes[i].BiomeChunks[x]);
						}

						newStr = BiomeA.BiomeStrength + BiomeB.BiomeStrength;

						Biomes[j].BiomeChunks = tempNewBiome;
						Biomes[j].BiomeStrength = newStr;
						
						Biomes.RemoveAt(i);

						
						NumBiomes--;
						//merge
					}
					
				}
				
				// Do something if BiomeA and BiomeB are touching
				// For example: Output their types or take any specific action
				//UE_LOG(LogTemp, Warning, TEXT("Biomes of type %d and %d are touching!"), BiomeA.BiomeType, BiomeB.BiomeType);
			}
		}
	}

}


bool AWorldGen::AreBiomesTouching(const FBiome& BiomeA, const FBiome& BiomeB)
{
	for (const FCoOrds& ChunkA : BiomeA.PositionInWorld)
	{
		for (const FCoOrds& ChunkB : BiomeB.PositionInWorld)
		{
			if (IsAdjacent(ChunkA, ChunkB))
			{
				return true; // Biomes are touching
			}
		}
	}
	return false; // Biomes are not touching

}

void AWorldGen::ChunkAdjustment()
{
	int blockType = 0;
	for(int a = 0; a < Biomes.Num(); a++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("STR in Biome index is %d: %f"), a, Biomes[a].BiomeStrength);
		bool isEven;
		int evenCheck = a;
		if(evenCheck % 2 == 0)
		{
			isEven = true;
			//UE_LOG(LogTemp, Warning, TEXT("is even"));
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("is false"));
			isEven = false;
		}

		//Biomes[a].BiomeNoiseScale = FMath::RandRange(1, 5);

		UE_LOG(LogTemp, Warning, TEXT("chunks in biome is  %d: %d"), a, Biomes[a].BiomeChunks.Num());
		for(int s = 0; s < Biomes[a].BiomeChunks.Num(); s++)
		{
			if(isEven)
			{
				//UE_LOG(LogTemp, Warning, TEXT("ahhhhhhhhhhh"));
				Biomes[a].BiomeChunks[s].IsAlive = false;
			}
			Biomes[a].BiomeChunks[s].ChunkType = Biomes[a].BiomeType;
		}
	}

	PerlinNoiseStart();


	
	for(int i = 0; i < worldSizeX; i++)
	{
		for(int z = 0; z < worldSizeY; z++)
		{
			//bool isDone[chunkXSize][ChunkYSize];
			TArray<TArray<bool>> isDone;
			isDone.SetNum(chunkXSize);
			

			for(int b=0;b<chunkXSize;b++)
			{
				isDone[b].SetNum(ChunkYSize);
				
				
				for(int n=0; n < ChunkYSize; n++)
				{
					isDone[b][n] = false;
				}
			}

			for(int q = 0; q < WorldArray[i][z].ZArray.Num(); q++)
			{
				for(int w = 0; w < WorldArray[i][z].ZArray[q].SecondArray.Num(); w++)
				{
					for(int e = 0; e < WorldArray[i][z].ZArray[q].SecondArray[w].FirstArray.Num(); e++)
					{
						if(isDone[e][w])
						{
							break;
						}
						if(WorldArray[i][z].ZArray[q].SecondArray[w].FirstArray[e] == -1)
						{
							UE_LOG(LogTemp, Warning, TEXT("loop done block was air"));
							//found
							//break;
							WorldArray[i][z].ZArray[q].SecondArray[w].FirstArray[e] = WorldArray[i][z].ChunkType;	
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("loop skipped block was there"));
							isDone[e][w] = true;
							//WorldArray[z][i].ZArray[q].SecondArray[w].FirstArray[e] = WorldArray[i][z].ChunkType;
							break;
							
						}
						
					}
				}
			}
		}
	}
}

void AWorldGen::PerlinNoiseStart()
{
	
	
/*
	for(int i = 0; i < Biomes.Num(); i++)
	{
		for(int o = 0; o < Biomes[i].BiomeChunks.Num();o++)
		{
				for(int j = 0; j < Biomes[i].BiomeChunks[o].ZArray[0].SecondArray.Num();j++)
				{
					for(int k = 0; k < Biomes[i].BiomeChunks[o].ZArray[0].SecondArray[j].FirstArray.Num();k++)
					{
						float zHeight = FMath::PerlinNoise2D(FVector2D(j + 0.1,k + 0.1));
						zHeight++;
						int temp = FMath::RoundToInt(zHeight);
						temp = Biomes[i].BiomeNoiseScale * temp;
						//UE_LOG(LogTemp, Warning, TEXT("zHeight value is: %d"), temp);
						Biomes[i].BiomeChunks[o].ZArray[temp].SecondArray[j].FirstArray[k] = Biomes[i].BiomeType;
						int blockIs = Biomes[i].BiomeChunks[o].ZArray[temp].SecondArray[j].FirstArray[k];
						//UE_LOG(LogTemp, Warning, TEXT("element at biome z y x is: %d %d %d %d %d %d"),i,o,temp,j,k, blockIs);
					}
				}
			
		}
	}*/

	for(int q = 0; q < worldSizeX; q++)
	{
		for(int w = 0; w < worldSizeY; w++)
		{
			for(int y = 0; y < WorldArray[q][w].ZArray[0].SecondArray.Num(); y++)
			{
				for(int x = 0; x < WorldArray[q][w].ZArray[0].SecondArray[y].FirstArray.Num(); x++)
				{
					WorldArray[q][w].ChunkNoiseScale = 3;//FMath::RandRange(1,5);
					int xMul = chunkXSize * q;
					int yMul = ChunkYSize * w;
					float zHeight = FMath::PerlinNoise2D(FVector2D(x + xMul + 0.1,y + yMul + 0.1));
					zHeight++;
					zHeight *= WorldArray[q][w].ChunkNoiseScale;
					int temp = FMath::RoundToInt(zHeight);
					//temp = temp * WorldArray[q][w].ChunkNoiseScale;

					WorldArray[q][w].ZArray[temp].SecondArray[y].FirstArray[x] = WorldArray[q][w].ChunkType;
					UE_LOG(LogTemp, Warning, TEXT("element at chunk z y x is: %d %d %d %d %d %d"),q,w,temp,y,x, WorldArray[q][w].ZArray[temp].SecondArray[y].FirstArray[x]);
				}
			}
		}
	}
}






// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

