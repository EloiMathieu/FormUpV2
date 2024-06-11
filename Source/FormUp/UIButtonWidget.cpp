// Fill out your copyright notice in the Description page of Project Settings.


#include "UIButtonWidget.h"
#include "Components/Button.h"

void UUIButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button->OnClicked.AddDynamic(this, &UUIButtonWidget::OnUIButtonClickedEvent);
	Button->OnHovered.AddDynamic(this, &UUIButtonWidget::OnUIButtonHoveredEvent);
	Button->OnUnhovered.AddDynamic(this, &UUIButtonWidget::OnUIButtonUnHoveredEvent);
}

void UUIButtonWidget::OnUIButtonClickedEvent()
{
	OnButtonClicked.Broadcast(this, ButtonIndex);
}

void UUIButtonWidget::OnUIButtonHoveredEvent()
{
	OnButtonHovered.Broadcast(this, ButtonIndex);
}

void UUIButtonWidget::OnUIButtonUnHoveredEvent()
{
	OnButtonUnHovered.Broadcast(this, ButtonIndex);
}
