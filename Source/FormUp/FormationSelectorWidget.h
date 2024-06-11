// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIButtonWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "FormationButtonWidget.h"
#include "FormationSelectorWidget.generated.h"

class ASPlayerController;
/**
 * 
 */
UCLASS()
class FORMUP_API UFormationSelectorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* LineButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* ColumnButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* WedgeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* CircleButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* WallButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* SpacingSlider;

protected:

	UFUNCTION()
	void OnFormationButtonClicked(UUIButtonWidget* Button, int Index);
	
	UFUNCTION()
	void OnSpacingValueChanged(const float Value);

	UPROPERTY()
	ASPlayerController* SPlayer;
};
