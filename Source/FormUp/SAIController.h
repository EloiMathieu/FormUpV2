// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FormUp/AIData.h"
#include "SAIController.generated.h"


class AFormUpCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReachedDestinationDelegate, const FCommandData, CommandData);

/**
 * 
 */
UCLASS()
class FORMUP_API ASAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	ASAIController(const FObjectInitializer& ObjectInitilizer);

	UFUNCTION()
	void CommandMove(FCommandData CommandData);

	UPROPERTY()
	FReachedDestinationDelegate OnReachedDestination;

protected:

	virtual void OnPossess(class APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY()
	AFormUpCharacter* OwningCharacter;

	UPROPERTY()
	FCommandData CurrentCommandData;
};
