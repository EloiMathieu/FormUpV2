#pragma once

#include "AIData.generated.h"

UENUM(BlueprintType)
enum ECommandType
{
	CommandMove,
	CommandMoveFast,
	CommandMoveSlow,
	CommandMoveAttack
};

UENUM(BlueprintType)
enum EFormation
{
	Line,
	Column,
	Wedge,
	Circle,
	Wall
};

USTRUCT(BlueprintType)
struct FCommandData
{
	GENERATED_BODY()

	FCommandData() : SourceLocation(FVector::ZeroVector), Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator), Type(ECommandType::CommandMove), Target(nullptr) {}
	FCommandData(const FVector InLocation, const FRotator InRotation, const ECommandType InType, AActor* InTarget = nullptr) :
	SourceLocation(InLocation), Location(InLocation), Rotation(InRotation), Type(InType), Target(InTarget) {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SourceLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECommandType> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* Target;

};