// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "FormUpCharacter.h"
#include "HudWidget.h"
#include "PlacementPreview.h"
#include "Selectable.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerInputActions.h"
#include "Net/UnrealNetwork.h"
#include "FormationDataAsset.h"

ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer)
{
	bPlacementModeEnabled = false;
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASPlayerController, Selected, COND_OwnerOnly);
}

void ASPlayerController::Handle_DeSelection(AActor* ActorToSelect)
{
	if (ActorToSelect && ActorSelected(ActorToSelect))
	{
		Server_DeSelect(ActorToSelect);
	}
}

void ASPlayerController::Handle_DeSelection(TArray<AActor*> ActorsToSelect)
{
	Server_DeSelect_Group(ActorsToSelect);
}

void ASPlayerController::Handle_Selection(AActor* ActorToSelect)
{
	if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
	{
		if (ActorToSelect && ActorSelected(ActorToSelect))
		{
			Server_DeSelect(ActorToSelect);
		}
		else
		{
			Server_Select(ActorToSelect);
		}
	}
	else
	{
		Server_ClearSelected();
	}
}

void ASPlayerController::Handle_Selection(TArray<AActor*> ActorsToSelect)
{
	Server_Select_Group(ActorsToSelect);
}

FVector ASPlayerController::GetMousePositionOnTerrain() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, WorldLocation, WorldLocation + (WorldDirection * 100000.0f), ECollisionChannel::ECC_GameTraceChannel1))
	{
		if (OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}

	return FVector::ZeroVector;
}

FVector ASPlayerController::GetMousePositionOnSurface() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, WorldLocation, WorldLocation + (WorldDirection * 100000.0f), ECollisionChannel::ECC_Visibility))
	{
		if (OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}

	return FVector::ZeroVector;
}

void ASPlayerController::Tick(float DeltaSeconds)
{
	if (bPlacementModeEnabled && PlacementPreviewActor)
	{
		UpdatePlacement();
	}
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	verify((AssetManager = UAssetManager::GetIfValid()) != nullptr);

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;

	// Create Formation Data
	CreateFormationData();

	// Create Hud
	CreateHUD();
}

bool ASPlayerController::ActorSelected(AActor* ActorToCheck) const
{
	if (ActorToCheck && Selected.Contains(ActorToCheck))
	{
		return true;
	}

	return false;
}

void ASPlayerController::Server_Select_Group_Implementation(const TArray<AActor*>& ActorsToSelect)
{
	Server_ClearSelected();

	TArray<AActor*> ValidActors;
	for (int i = 0; i < ActorsToSelect.Num(); ++i)
	{
		if (ActorsToSelect[i])
		{
			if (ISelectable* Selectable = Cast<ISelectable>(ActorsToSelect[i]))
			{
				ValidActors.Add(ActorsToSelect[i]);
				Selectable->Select();
			}
		}
	}

	for (int j = 0; j < ValidActors.Num(); ++j)
	{
		Selected.Add(ValidActors[j]);
		OnRep_Selected();
	}

	ValidActors.Empty();
}

void ASPlayerController::Server_Select_Implementation(AActor* ActorToSelect)
{
	if (ActorToSelect)
	{
		if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
		{
			Selectable->Select();
			Selected.Add(ActorToSelect);
			OnRep_Selected();
		}
	}
}

void ASPlayerController::Server_DeSelect_Implementation(AActor* ActorToDeSelect)
{
	if (ActorToDeSelect)
	{
		if (ISelectable* Selectable = Cast<ISelectable>(ActorToDeSelect))
		{
			Selectable->DeSelect();
			Selected.Remove(ActorToDeSelect);
			OnRep_Selected();
		}
	}
}

void ASPlayerController::Server_ClearSelected_Implementation()
{
	for (int i = 0; i < Selected.Num(); ++i)
	{
		if (Selected[i])
		{
			if (ISelectable* Selectable = Cast<ISelectable>(Selected[i]))
			{
				Selectable->DeSelect();
			}
		}
	}

	Selected.Empty();
	OnRep_Selected();
}

void ASPlayerController::Server_DeSelect_Group_Implementation(const TArray<AActor*>& ActorsToDeSelect)
{
	for (int i = 0; i < ActorsToDeSelect.Num(); ++i)
	{
		if (ActorsToDeSelect[i])
		{
			for (int j = Selected.Num() - 1; j >= 0; --j)
			{
				if (ActorsToDeSelect[i] == Selected[j])
				{
					if (ISelectable* Selectable = Cast<ISelectable>(ActorsToDeSelect[i]))
					{
						Selectable->DeSelect();
						Selected.RemoveAt(j);
						break;
					}
				}
			}
		}
	}

	OnRep_Selected();

}

void ASPlayerController::OnRep_Selected()
{
	OnSelectedUpdated.Broadcast();
}

void ASPlayerController::CommandSelected(FCommandData CommandData)
{
	Server_CommandSelected(CommandData);
}

void ASPlayerController::UpdateFormation(const EFormation Formation)
{
	CurrentFormation = Formation;

	if (HasGroupSelection() && Selected.IsValidIndex(0))
	{
		CommandSelected(FCommandData(Selected[0]->GetActorLocation(), Selected[0]->GetActorRotation(), ECommandType::CommandMove));
	}
}

void ASPlayerController::UpdateSpacing(const float NewSpacing)
{
	FormationSpacing = NewSpacing;

	if (HasGroupSelection() && Selected.IsValidIndex(0))
	{
		CommandSelected(FCommandData(Selected[0]->GetActorLocation(), Selected[0]->GetActorRotation(), ECommandType::CommandMove));
	}
}

void ASPlayerController::CreateFormationData()
{
	const FPrimaryAssetType AssetType("FormationData");
	TArray<FPrimaryAssetId> Formations;
	AssetManager->GetPrimaryAssetIdList(AssetType, Formations);

	if (Formations.Num() > 0)
	{
		const TArray<FName> Bundles;
		const FStreamableDelegate FormationDataLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ASPlayerController::OnFormationDataLoaded, Formations);
		AssetManager->LoadPrimaryAssets(Formations, Bundles, FormationDataLoadedDelegate);
	}
}

void ASPlayerController::OnFormationDataLoaded(TArray<FPrimaryAssetId> Formations)
{
	for (int i = 0; i < Formations.Num(); ++i)
	{
		if (UFormationDataAsset* FormationDataAsset = Cast<UFormationDataAsset>(AssetManager->GetPrimaryAssetObject(Formations[i])))
		{
			FormationData.Add(FormationDataAsset);
		}
	}
}

UFormationDataAsset* ASPlayerController::GetFormationData()
{
	for (int i = 0; i < FormationData.Num(); ++i)
	{
		if (FormationData[i]->FormationType == CurrentFormation)
		{
			return FormationData[i];
		}
	}

	return nullptr;
}

void ASPlayerController::CalculateOffset(const int Index, FCommandData& CommandData)
{
	if (FormationData.Num() <= 0)
	{
		return;
	}

	if (const UFormationDataAsset* CurrentFormationData = GetFormationData())
	{
		FVector Offset = CurrentFormationData->Offset;

		switch (CurrentFormationData->FormationType)
		{
			case EFormation::Circle:
			{
				if (Index != 0)
				{
					float Angle = (Index / static_cast<float>(Selected.Num() - 1)) * 2 * PI;

					//float MinSpacing = FormationSpacing * 10f;
					//if (Index % 2 == 0)
					//{
						//MinSpacing = MinSpacing * -1;
					//}
					const float Radius = FormationSpacing;

					Offset.X = Radius * FMath::Cos(Angle);
					Offset.Y = Radius * FMath::Sin(Angle);
				}
				break;
			}
			case EFormation::Wall:
			{
				for (float col = 0; col < static_cast<float>(Selected.Num() / 3); col++)
				{
					for (int row = 0; row != col; row++)
					{
						Offset.X = FormationSpacing;
						
					}
				}
				break;
			}
			default:
			{
				if (CurrentFormationData->Alternate)
				{
					//Alternating the side of the position Left and Right Calculation
					if (Index % 2 == 0) 
					{
						//even
						Offset.Y = Offset.Y * -1;
						
						
						
					}

					// Allow two positions on each row, using alternate position
					Offset *= (FMath::Floor(Index + 1) / 2) * FormationSpacing;
					
				}
				else
				{
					// Normal Offset Calculation
					Offset *= Index * FormationSpacing;
				}
			}
		}	

		Offset = CommandData.Rotation.RotateVector(Offset);
		CommandData.Location = CommandData.SourceLocation + Offset;
	}
}

void ASPlayerController::Server_CommandSelected_Implementation(FCommandData CommandData)
{
	if (!HasAuthority())
	{
		return;
	}

	for (int i = 0; i < Selected.Num(); ++i)
	{
		if (AFormUpCharacter* SelectedCharacter = Cast<AFormUpCharacter>(Selected[i]))
		{
			CalculateOffset(i, CommandData);
			SelectedCharacter->CommandMoveToLocation(CommandData);
		}
	}

}

// Enhanced Input//

void ASPlayerController::AddInputMapping(const UInputMappingContext* InputMapping, const int32 MappingPriority) const
{
	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(InputMapping);

		if (!InputSubsystem->HasMappingContext(InputMapping))
		{
			InputSubsystem->AddMappingContext(InputMapping, MappingPriority);
		}
	}
}

void ASPlayerController::RemoveInputMapping(const UInputMappingContext* InputMapping) const
{
	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(InputMapping);
		InputSubsystem->RemoveMappingContext(InputMapping);
	}
}

void ASPlayerController::SetInputDefault(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is null!"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextDefault);

		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextDefault, PlayerActions->MapPriorityDefault);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextDefault);
		}
	}
}

void ASPlayerController::SetInputPlacement(const bool Enabled) const
{
	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextPlacement);
		
		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextPlacement, PlayerActions->MapPriorityPlacement);
			SetInputDefault(!Enabled);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextPlacement);
			SetInputDefault();
		}
	}
}

void ASPlayerController::SetInputShift(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is Null!"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextShift);

		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextShift, PlayerActions->MapPriorityShift);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextShift);
		}
	}
}

void ASPlayerController::SetInputAlt(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is Null!"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextAlt);

		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextAlt, PlayerActions->MapPriorityAlt);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextAlt);
		}
	}
}

void ASPlayerController::SetInputCtrl(const bool Enabled) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionsAsset is Null!"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextCtrl);

		if (Enabled)
		{
			AddInputMapping(PlayerActions->MappingContextCtrl, PlayerActions->MapPriorityCtrl);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextCtrl);
		}
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		InputSubsystem->ClearAllMappings();
		SetInputDefault();
	}
}

void ASPlayerController::SetPlacementPreview()
{
	if (PreviewActorType && !bPlacementModeEnabled)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetMousePositionOnSurface());
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PlacementPreviewActor = GetWorld()->SpawnActor<APlacementPreview>(PreviewActorType, SpawnTransform, SpawnParams);

		if (PlacementPreviewActor)
		{
			//Enable Placement//
			SetInputPlacement();

			//Set Placement mode//
			bPlacementModeEnabled = true;
		}
	}
}

void ASPlayerController::Place()
{
	if (!IsPlacementModeEnabled() || !PlacementPreviewActor)
	{
		return;
	}

	bPlacementModeEnabled = false;

	SetInputPlacement(false);

	Server_Place(PlacementPreviewActor);

}

void ASPlayerController::PlaceCancel()
{
	if (!IsPlacementModeEnabled() || !PlacementPreviewActor)
	{
		return;
	}

	bPlacementModeEnabled = false;

	SetInputPlacement(false);

	EndPlacement();
}

void ASPlayerController::UpdatePlacement() const
{
	if (!PlacementPreviewActor)
	{
		return;
	}

	PlacementPreviewActor->SetActorLocation(GetMousePositionOnSurface());
}

void ASPlayerController::CreateHUD()
{
	if (HudClass)
	{
		HUD = CreateWidget<UHudWidget>(GetWorld(), HudClass);
		if (HUD != nullptr)
		{
			HUD->AddToViewport();
		}
	}
}

void ASPlayerController::Server_Place_Implementation(AActor* PlacementPreviewToSpawn)
{
	if (const APlacementPreview* Preview = Cast<APlacementPreview>(PlacementPreviewToSpawn))
	{
		FTransform SpawnTransform;
		FVector Location = Preview->GetActorLocation();
		SpawnTransform.SetLocation(FVector(Location.X, Location.Y, Location.Z + 100.0f));
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (int UnitNumber = 0; UnitNumber < UnitAmount; UnitNumber++)
		{

			if (AFormUpCharacter* NewUnit = GetWorld()->SpawnActor<AFormUpCharacter>(Preview->PlaceableClass, SpawnTransform, SpawnParams))
			{
				NewUnit->SetOwner(this);
			}
		}
	}

	EndPlacement();
}

void ASPlayerController::EndPlacement_Implementation()
{
	PlacementPreviewActor->Destroy();
}

