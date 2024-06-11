// Fill out your copyright notice in the Description page of Project Settings.


#include "HudWidget.h"
#include "FormationSelectorWidget.h"
#include "Kismet/GameplayStatics.h"
#include "FormUp/SPlayerController.h"

void UHudWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((SPlayer = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);

	//Disable Hud
	SetFormationSelectionWidget(false);

	if (SPlayer)
	{
		SPlayer->OnSelectedUpdated.AddDynamic(this, &UHudWidget::OnSelectionUpdated);
	}
}

void UHudWidget::SetFormationSelectionWidget(const bool bEnabled) const
{
	if (FormationSelectionWidget)
	{
		FormationSelectionWidget->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UHudWidget::OnSelectionUpdated()
{
	if (SPlayer)
	{
		SetFormationSelectionWidget(SPlayer->HasGroupSelection());
	}
}
