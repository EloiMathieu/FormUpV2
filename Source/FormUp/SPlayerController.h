// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FormUp/AIData.h"
#include "Engine/AssetManager.h"
#include "HudWidget.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectedUpdatedDelegate);

class UInputMappingContext;

/**
 * 
 */
UCLASS()
class FORMUP_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()



public:
	ASPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	
	UFUNCTION()
	void Handle_DeSelection(AActor* ActorToSelect);
	void Handle_DeSelection(TArray<AActor*> ActorsToSelect);

	UFUNCTION()
	void Handle_Selection(AActor* ActorToSelect);
	void Handle_Selection(TArray<AActor*> ActorsToSelect);

	UFUNCTION()
	FVector GetMousePositionOnTerrain() const;

	UFUNCTION()
	FVector GetMousePositionOnSurface() const;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	bool HasGroupSelection() const { return Selected.Num() > 1; }

	UPROPERTY()
	FSelectedUpdatedDelegate OnSelectedUpdated;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	bool ActorSelected(AActor* ActorToCheck) const;

	UFUNCTION(Server, Reliable)
	void Server_Select(AActor* ActorToSelect);

	UFUNCTION(Server, Reliable)
	void Server_Select_Group(const TArray<AActor*>& ActorsToSelect);

	UFUNCTION(Server, Reliable)
	void Server_DeSelect(AActor* ActorToDeSelect);

	UFUNCTION(Server, Reliable)
	void Server_DeSelect_Group(const TArray<AActor*>& ActorsToDeSelect);

	UFUNCTION(Server, Reliable)
	void Server_ClearSelected();

	UFUNCTION()
	void OnRep_Selected();

	UPROPERTY(ReplicatedUsing = OnRep_Selected)
	TArray<AActor*> Selected;



	// Command Functions //

public:

	UFUNCTION()
	void CommandSelected(FCommandData CommandData);

	UFUNCTION()
	void UpdateFormation(const EFormation Formation);

	UFUNCTION()
	void UpdateSpacing(const float NewSpacing);


protected:

	UFUNCTION(Server, Reliable)
	void Server_CommandSelected(FCommandData CommandData);

	UFUNCTION()
	void CreateFormationData();

	UFUNCTION()
	void OnFormationDataLoaded(TArray<FPrimaryAssetId> Formations);

	UFUNCTION()
	UFormationDataAsset* GetFormationData();

	UFUNCTION()
	void CalculateOffset(const int Index, FCommandData& CommandData);

	UPROPERTY()
	TEnumAsByte<EFormation> CurrentFormation;

	UPROPERTY()
	float FormationSpacing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Units")
	float UnitAmount;

	UPROPERTY()
	UAssetManager* AssetManager;

	UPROPERTY()
	TArray<UFormationDataAsset*> FormationData;


	// End Command Functions //
	

	// Enhanced Input //
public:

	UFUNCTION()
	void AddInputMapping(const UInputMappingContext* InputMapping, const int32 MappingPriority = 0) const;

	UFUNCTION()
	void RemoveInputMapping(const UInputMappingContext* InputMapping) const;

	UFUNCTION()
	void SetInputDefault(const bool Enabled = true) const;

	UFUNCTION()
	void SetInputPlacement(const bool Enabled = true) const;

	UFUNCTION()
	void SetInputShift(const bool Enabled = true) const;

	UFUNCTION()
	void SetInputAlt(const bool Enabled = true) const;

	UFUNCTION()
	void SetInputCtrl(const bool Enabled = true) const;

	UFUNCTION()
	UDataAsset* GetInputActionsAsset() const { return PlayerActionsAsset; }

protected:

	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Player Settings")
	UDataAsset* PlayerActionsAsset;

	//Placement//

public:

	UFUNCTION()
	bool IsPlacementModeEnabled() const { return bPlacementModeEnabled; }

	UFUNCTION()
	void SetPlacementPreview();

	UFUNCTION()
	void Place();

	UFUNCTION()
	void PlaceCancel();

protected:

	UFUNCTION()
	void UpdatePlacement() const;

	UFUNCTION(Server, Reliable)
	void Server_Place(AActor* PlacementPreviewToSpawn);

	UFUNCTION(Client, Reliable)
	void EndPlacement();

	UPROPERTY()
	bool bPlacementModeEnabled;

	UPROPERTY()
	AActor* PlacementPreviewActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Placeable")
	TSubclassOf<AActor> PreviewActorType;

	// UI //

public:
	 
	UFUNCTION()
	void CreateHUD();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|UI")
	TSubclassOf<UUserWidget> HudClass;

	UPROPERTY()
	UHudWidget* HUD;
};
