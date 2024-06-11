// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionBox.generated.h"

class UBoxComponent;
class ASPlayerController;
class UDecalComponent;

UCLASS()
class FORMUP_API ASelectionBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASelectionBox();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Start(FVector Position, const FRotator Rotation);

	UFUNCTION()
	void End(const bool bSelect = true, const bool bAddOnly = false);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void BoxAdjust() const;

	UFUNCTION()
	void UnitManage();

	UFUNCTION()
	void HandleHighlight(AActor* ActorInBox, const bool Highlight = true) const;

	UFUNCTION()
	void OnBoxColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	FRotator StartRotation;

	UPROPERTY()
	TArray<AActor*> InBox;

	UPROPERTY()
	TArray<AActor*> CenterInBox;


private:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollider;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* DecalComponent;

	UPROPERTY()
	bool BoxSelect;

	UPROPERTY()
	ASPlayerController* SPlayer;


};
