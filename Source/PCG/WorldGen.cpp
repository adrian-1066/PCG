// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen.h"

// Sets default values
AWorldGen::AWorldGen()
{
	srand(time(nullptr));
	RandomSeed = FMath::RandRange(10000,99999);
	chunkXSize = 6;
	ChunkYSize = 6;
	ChunkZSize = 100;
	worldSizeX = 10;
	worldSizeY = 10;
	biomeCount = 0;
	WorldArray.SetNum(worldSizeX);
	GrassBiome = 0;
	SandBiome = 1;
	StoneBiome = 2;
	SnowBiome = 3;
	blocksSpawned = 0;
	caveGenChacne = -0.25f;
	MaxNoiseScale = (ChunkZSize/2) - 1;
	if(MaxNoiseScale <= 0)
	{
		MaxNoiseScale = 0;
	}
	for(int x = 0; x < worldSizeX; x++)
	{
		WorldArray[x].SetNum(worldSizeY);
	}
	PrimaryActorTick.bCanEverTick = true;
}
void AWorldGen::BeginPlay()
{
	Super::BeginPlay();
	GenerateChunks();
	CellularAutomata();
	EstablishBiomes();
	BiomeMerge();
	for(int i = 0; i < Biomes.Num(); i++)
	{
		int temp;
		switch (Biomes[i].BiomeType)
		{
			case(0):
				temp = FMath::RandRange((MaxNoiseScale/2)-5, MaxNoiseScale/2);
				break;
			case(1):
				temp = FMath::RandRange((MaxNoiseScale/2)-5, MaxNoiseScale/2);
				break;
			case(2):
				temp = FMath::RandRange(MaxNoiseScale-10, MaxNoiseScale);
				break;
			case(3):
				temp = FMath::RandRange((MaxNoiseScale/2)-5, MaxNoiseScale/2);
				break;
			default:
				temp = FMath::RandRange((MaxNoiseScale/2)-5, MaxNoiseScale/2);
				break;
		}
		Biomes[i].BiomeNoiseScale = temp;
	}
	ChunkAdjustment();
	biomeCount = Biomes.Num();
	CaveNoiseGenerator();
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
								if(canSpawn)
								{
									FVector SpawnLocation = FVector(ChunkXCoOrd + (t*100),ChunkYCoOrd + (r*100),(e*100));
									FRotator SpawnRotation = FRotator(0.0f,0.0f,0.0f);
									blocksSpawned++;
									World->SpawnActor<AActor>(Test[WorldArray[q][w].ChunkType], SpawnLocation, SpawnRotation, SpawnParams);	
								}
							}
							FChunk Temp;
							FSecondArray ZArrayTemp;
							FArrayOne XArrayTemp;
							Temp = WorldArray[q][w];
							ZArrayTemp = Temp.ZArray[e];
							XArrayTemp = ZArrayTemp.SecondArray[r];
							if(!WorldArray[q][w].IsAlive)
							{
							}
						}
					}
				}
			}
		}
	}
}

bool AWorldGen::CheckForAir(int worldX, int worldY, int chunkX, int chunkY, int chunkZ)
{
	bool airFound = false;
	int currentBlockTypeToCheck;

	for(int i = 0; i < 6; i++)
	{
		if(airFound)
		{
			break;
		}
		switch(i)
		{
		case (0):
			if(chunkZ >= ChunkZSize-1)
			{
				airFound = true;
				return true;
			}
			else
			{
				currentBlockTypeToCheck = BlockTypeAt(worldX,worldY,chunkX,chunkY,chunkZ+1);
				if(currentBlockTypeToCheck == -1)
				{
					airFound = true;
					return true;
				}
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
	if(x < 0 || x >= worldSizeX)
	{
		return true;
	}
	if(y < 0 || y >= worldSizeY)
	{
		return true;
	}
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
			tempX = x -1;
			tempY = y -1;
			break;
		case 1:
			tempX = 0;
			tempY = y - 1;
			break;
		case 2:
			tempX = x + 1;
			tempY = y - 1;
			break;
		case 3:
			tempX = x - 1;
			tempY = y;
			break;
		case 4:
			tempX = x + 1;
			tempY = y;
			break;
		case 5:
			tempX = x - 1;
			tempY = y + 1;
			break;
		case 6:
			tempX = x;
			tempY = y + 1;
			break;
		case 7:
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

				if(BiomeA.BiomeStrength >= 100.0f)
				{
					canMerge = false;
				}
				else if(BiomeB.BiomeStrength >= 100.0f)
				{
					canMerge = false;
				}

				if(BiomeA.BiomeChunks.Num() <= 3)
				{
					canMerge = true;
				}
				else if(BiomeB.BiomeChunks.Num() <= 3)
				{
					canMerge = true;
				}

				if(canMerge)
				{
					FBiome tempNewBiome;
					FCoOrds coOrdsToAdd;
					float newStr = 0;
					if(BiomeA.BiomeStrength > BiomeB.BiomeStrength)
					{
						//merge
						for(int x = 0; x < Biomes[j].BiomeChunks.Num(); x++)
						{
							tempNewBiome.BiomeChunks.Add(Biomes[j].BiomeChunks[x]);
							coOrdsToAdd.x = Biomes[j].PositionInWorld[x].x;
							coOrdsToAdd.y = Biomes[j].PositionInWorld[x].y;
							tempNewBiome.PositionInWorld.Add(coOrdsToAdd);
							tempNewBiome.BiomeType = BiomeA.BiomeType;
							
							
						}

						for(int x = 0; x < Biomes[i].BiomeChunks.Num(); x++)
						{
							tempNewBiome.BiomeChunks.Add(Biomes[i].BiomeChunks[x]);
							coOrdsToAdd.x = Biomes[i].PositionInWorld[x].x;
							coOrdsToAdd.y = Biomes[i].PositionInWorld[x].y;
							tempNewBiome.PositionInWorld.Add(coOrdsToAdd);
						}

						newStr = BiomeA.BiomeStrength + BiomeB.BiomeStrength;

						Biomes[i] = tempNewBiome;
						Biomes[i].BiomeStrength = newStr;
						
						Biomes.RemoveAt(j);
						NumBiomes--;
					
					}
					else if(BiomeB.BiomeStrength > BiomeA.BiomeStrength)
					{
						for(int x = 0; x < Biomes[j].BiomeChunks.Num(); x++)
						{
							tempNewBiome.BiomeChunks.Add(Biomes[j].BiomeChunks[x]);
							coOrdsToAdd.x = Biomes[j].PositionInWorld[x].x;
							coOrdsToAdd.y = Biomes[j].PositionInWorld[x].y;
							tempNewBiome.PositionInWorld.Add(coOrdsToAdd);
							tempNewBiome.BiomeType = BiomeB.BiomeType;
						}

						for(int x = 0; x < Biomes[i].BiomeChunks.Num(); x++)
						{
							tempNewBiome.BiomeChunks.Add(Biomes[i].BiomeChunks[x]);
							coOrdsToAdd.x = Biomes[i].PositionInWorld[x].x;
							coOrdsToAdd.y = Biomes[i].PositionInWorld[x].y;
							tempNewBiome.PositionInWorld.Add(coOrdsToAdd);
						}

						newStr = BiomeA.BiomeStrength + BiomeB.BiomeStrength;

						Biomes[j] = tempNewBiome;
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
			WorldArray[Biomes[a].PositionInWorld[s].x][Biomes[a].PositionInWorld[s].y].ChunkType = Biomes[a].BiomeType;
			float tempNoiseAdj = FMath::PerlinNoise2D(FVector2D(Biomes[a].PositionInWorld[s].x + RandomSeed,Biomes[a].PositionInWorld[s].y + RandomSeed));
			int tempIntAdj = FMath::RoundToInt(tempNoiseAdj);
			

			WorldArray[Biomes[a].PositionInWorld[s].x][Biomes[a].PositionInWorld[s].y].ChunkNoiseScale = Biomes[a].BiomeNoiseScale + tempIntAdj;
		}
	}

	//PerlinNoiseStart();


	
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

					for(int f = 0; f < ChunkZSize; f++)
					{
						if(WorldArray[i][z].ZArray[f].SecondArray[n].FirstArray[b] <= -1)
						{
							//WorldArray[i][z].ZArray[f].SecondArray[n].FirstArray[b] = WorldArray[i][z].ChunkType;
						}
						else
						{
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
	for(int q = 0; q < worldSizeX; q++)
	{
		for(int w = 0; w < worldSizeY; w++)
		{
			float tempScale = FMath::PerlinNoise2D(FVector2D(q + 0.035,w + 0.035));
			tempScale += 1;
			tempScale *= 10;
			tempScale = FMath::RoundToInt(tempScale);
			
			//WorldArray[q][w].ChunkNoiseScale = tempScale;
			for(int y = 0; y < WorldArray[q][w].ZArray[0].SecondArray.Num(); y++)
			{
				for(int x = 0; x < WorldArray[q][w].ZArray[0].SecondArray[y].FirstArray.Num(); x++)
				{
					
					int xMul = chunkXSize * q;
					int yMul = ChunkYSize * w;

					//the smaller the float that is added on the flatter the terrain will be
					float zHeight = FMath::PerlinNoise2D(FVector2D(((1.25 * RandomSeed )+ xMul + x),((1.25*RandomSeed)+ yMul + y)));
					
					zHeight += 1.0f;
					zHeight *= WorldArray[q][w].ChunkNoiseScale;
					int temp = FMath::FloorToInt(zHeight);
					//temp += WorldArray[q][w].ChunkNoiseScale;
					//temp -= zHeight;
					
					
					//temp = temp * WorldArray[q][w].ChunkNoiseScale;

					WorldArray[q][w].ZArray[temp].SecondArray[y].FirstArray[x] = WorldArray[q][w].ChunkType;
					UE_LOG(LogTemp, Warning, TEXT("element at chunk z y x is: %d %d %d %d %d %d"),q,w,temp,y,x, WorldArray[q][w].ZArray[temp].SecondArray[y].FirstArray[x]);
				}
			}
		}
	}


	
}

void AWorldGen::CaveNoiseGenerator()
{
	//attempting to use 3d perlin noise to generate caves

	for(int worldX = 0; worldX < worldSizeX; worldX++)
	{
		for(int worldY = 0; worldY < worldSizeY; worldY++)
		{
			for(int z = 0; z < ChunkZSize; z++)
			{
				if(z == 0)
				{
					continue;
				}
				for(int y = 0; y <ChunkYSize; y++)
				{
					for(int x = 0; x < chunkXSize; x++)
					{
						int xMul = chunkXSize * worldX;
						int yMul = ChunkYSize * worldY;
						
						float isAir = FMath::PerlinNoise3D(FVector((x + xMul + RandomSeed) / 5.0f,(y + yMul + RandomSeed)/5.0f,(z + RandomSeed)/5.0f));
						//UE_LOG(LogTemp, Warning, TEXT("isAir is: %f"),isAir);
						float tempChance = caveGenChacne;
						if(WorldArray[worldX][worldY].ChunkType == 2)
						{
							tempChance -= (z/500);
						}
						else
						{
							tempChance -= (z/100);
						}

						if(isAir < tempChance)
						{
							WorldArray[worldX][worldY].ZArray[z].SecondArray[y].FirstArray[x] = -1;
						}
						else if(z <= WorldArray[worldX][worldY].ChunkNoiseScale)
						{
							WorldArray[worldX][worldY].ZArray[z].SecondArray[y].FirstArray[x] = WorldArray[worldX][worldY].ChunkType;
						}
					}
				}
			}
			int xMul = chunkXSize * worldX;
			int yMul = ChunkYSize * worldY;

			//the smaller the float that is added on the flatter the terrain will be
			
			for(int z = WorldArray[worldX][worldY].ChunkNoiseScale; z < WorldArray[worldX][worldY].ChunkNoiseScale + 2; z++ )
			{
				for(int y = 0; y < ChunkYSize; y++)
				{
					for(int x = 0; x < chunkXSize; x++ )
					{
						float zHeight = FMath::PerlinNoise3D(FVector((x + xMul + RandomSeed) / 10.0f,(y + yMul + RandomSeed)/10.0f,(z + RandomSeed)/10.0f));

						if(zHeight < 0.0f)
						{
							WorldArray[worldX][worldY].ZArray[z].SecondArray[y].FirstArray[x] = WorldArray[worldX][worldY].ChunkType;
						}
					}
				}
			}
			
/*
			for(int x = 0; x < chunkXSize; x++ )
			{
				for(int y = 0; y < ChunkYSize; y++)
				{
					float zHeight = FMath::PerlinNoise2D(FVector2D(((x * RandomSeed )+ xMul + x),((y *RandomSeed)+ yMul + y)));
					
					zHeight += 1.0f;
					zHeight *= 2;
					int temp = FMath::FloorToInt(zHeight);
					//temp *= 2;
					for(int z = WorldArray[worldX][worldY].ChunkNoiseScale; z < WorldArray[worldX][worldY].ChunkNoiseScale + temp; z++)
					{
						float isBlock = FMath::PerlinNoise3D(FVector((x + xMul + RandomSeed) / 10.0f,(y + yMul + RandomSeed)/10.0f,(z + RandomSeed)/10.0f));
						if(isBlock <= 0.7f)
						{
							WorldArray[worldX][worldY].ZArray[z].SecondArray[y].FirstArray[x] = WorldArray[worldX][worldY].ChunkType;
						}
						
					}
				}
			}*/
		}
	}
}

// Called every frame
void AWorldGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

