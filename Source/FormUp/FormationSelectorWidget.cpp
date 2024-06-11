// Fill out your copyright notice in the Description page of Project Settings.

#include "FormationSelectorWidget.h"
#include "Components/Slider.h"
#include "FormationButtonWidget.h"
#include "Kismet/GameplayStatics.h"
#include "FormUp/SPlayerController.h"

void UFormationSelectorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	verify((SPlayer = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);

	if (LineButton)
	{
		LineButton->OnButtonClicked.AddDynamic(this, &UFormationSelectorWidget::OnFormationButtonClicked);

		// Set Default Formation
		OnFormationButtonClicked(LineButton, 0);
	}

	if (ColumnButton)
	{
		ColumnButton->OnButtonClicked.AddDynamic(this, &UFormationSelectorWidget::OnFormationButtonClicked);
	}

	if (WedgeButton)
	{
		WedgeButton->OnButtonClicked.AddDynamic(this, &UFormationSelectorWidget::OnFormationButtonClicked);
	}

	if (CircleButton)
	{
		CircleButton->OnButtonClicked.AddDynamic(this, &UFormationSelectorWidget::OnFormationButtonClicked);
	}

	if (WallButton)
	{
		WallButton->OnButtonClicked.AddDynamic(this, &UFormationSelectorWidget::OnFormationButtonClicked);
	}

	if (SpacingSlider)
	{
		SpacingSlider->OnValueChanged.AddDynamic(this, &UFormationSelectorWidget::OnSpacingValueChanged);

		// Set Default Spacing
		OnSpacingValueChanged(SpacingSlider->GetValue());
	}
}

void UFormationSelectorWidget::OnFormationButtonClicked(UUIButtonWidget* Button, int Index)
{
	if (SPlayer)
	{
		SPlayer->UpdateFormation(static_cast<EFormation>(Index));
	}
}

void UFormationSelectorWidget::OnSpacingValueChanged(const float Value)
{
	if (SPlayer)
	{
		SPlayer->UpdateSpacing(Value);
	}
}
