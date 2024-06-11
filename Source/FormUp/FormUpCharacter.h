// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FormUp/AIData.h"
#include "Selectable.h"
#include "SAIController.h"
#include "GameFramework/Character.h"
#include "FormUpCharacter.generated.h"

class ASAIController;



UCLASS(Blueprintable)
class AFormUpCharacter : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:
	AFormUpCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:

	UFUNCTION()
	FTransform GetPositionTransform(const FVector Position) const;

	// ISelectable Interface//

	virtual void Select() override;
	virtual void DeSelect() override;
	virtual void Highlight(const bool Highlight) override;
	// End ISelectable Interface//

	UPROPERTY()
	bool Selected;

	// Command Functions //
public:

	UFUNCTION()
	void CommandMoveToLocation(const FCommandData ComandData);

	UFUNCTION()
	void SetAIController(ASAIController* NewAIController) { SAIController = NewAIController; }

protected:

	UFUNCTION()
	void CommandMove(const FCommandData CommandData);

	UFUNCTION()
	void DestinationReached(const FCommandData CommandData);

	UFUNCTION()
	void SetWalk() const;

	UFUNCTION()
	void SetRun() const;

	UFUNCTION()
	void SetSprint() const;

	UFUNCTION()
	void SetOrientation(const float DeltaTime);

	UFUNCTION()
	bool IsOrientated() const;

	UFUNCTION()
	void SetMoveMarker(const FVector Location);

	UPROPERTY()
	UCharacterMovementComponent* CharacterMoveComp;

	UPROPERTY()
	float MaxSpeed;

	UPROPERTY()
	FRotator TargetOrientation;

	UPROPERTY()
	uint8 ShouldOrientate;

	UPROPERTY()
	ASAIController* SAIController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI Settings")
	TSubclassOf<AActor> MoveMarkerClass;

	UPROPERTY()
	AActor* MoveMarker;


	// End Command Functions //

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;


};

