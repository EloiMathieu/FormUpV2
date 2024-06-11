// Fill out your copyright notice in the Description page of Project Settings.


#include "SAIController.h"
#include "FormUp/FormUpCharacter.h"

ASAIController::ASAIController(const FObjectInitializer& ObjectInitilizer)
{

}

void ASAIController::CommandMove(FCommandData CommandData)
{
	CurrentCommandData = CommandData;

	MoveToLocation(CommandData.Location);
}

void ASAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwningCharacter = Cast<AFormUpCharacter>(InPawn);
	if (OwningCharacter)
	{
		OwningCharacter->SetAIController(this);
	}
}

void ASAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	OnReachedDestination.Broadcast(CurrentCommandData);
}

