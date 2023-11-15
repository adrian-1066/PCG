// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCube.h"

// Sets default values
ABaseCube::ABaseCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	RootComponent = CubeMesh;
	UE_LOG(LogTemp, Warning, TEXT("attempting to find mesh"));
	/*
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("Engine/BasicShapes/Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("static mesh added"));
		CubeMesh->SetStaticMesh(CubeMeshAsset.Object);
	}
*/

}

// Called when the game starts or when spawned
void ABaseCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

