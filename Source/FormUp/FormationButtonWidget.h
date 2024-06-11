// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIButtonWidget.h"
#include "AIData.h"
#include "FormationButtonWidget.generated.h"




/**
 * 
 */
UCLASS()
class FORMUP_API UFormationButtonWidget : public UUIButtonWidget
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TEnumAsByte<EFormation> Formation;

};
