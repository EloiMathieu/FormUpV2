// Fill out your copyright notice in the Description page of Project Settings.


#include "PlacementPreview.h"

// Sets default values
APlacementPreview::APlacementPreview()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	bReplicates = false;
	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void APlacementPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlacementPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

