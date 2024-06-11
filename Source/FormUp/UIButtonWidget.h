// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIButtonWidget.generated.h"


class UButton;
class UImage;
class UTextBlock;
class USimpleUIButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonClicked, UUIButtonWidget*, Button, int, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonHovered, UUIButtonWidget*, Button, int, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonUnHovered, UUIButtonWidget*, Button, int, Index);


/**
 * 
 */
UCLASS()
class FORMUP_API UUIButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ButtonImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ButtonText;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonClicked OnButtonClicked;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonHovered OnButtonHovered;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonUnHovered OnButtonUnHovered;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	int32 ButtonIndex;

protected:

	UFUNCTION()
	void OnUIButtonClickedEvent();

	UFUNCTION()
	void OnUIButtonHoveredEvent();

	UFUNCTION()
	void OnUIButtonUnHoveredEvent();



};
