// Fill out your copyright notice in the Description page of Project Settings.


#include "FormationButtonWidget.h"
#include "Components/TextBlock.h"

void UFormationButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonIndex = static_cast<int>(Formation);

	if (ButtonText)
	{
		ButtonText->SetText(UEnum::GetDisplayValueAsText(Formation));
	}
}