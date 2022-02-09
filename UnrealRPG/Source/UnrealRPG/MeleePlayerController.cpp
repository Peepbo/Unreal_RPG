// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleePlayerController.h"
#include "Blueprint/UserWidget.h"

AMeleePlayerController::AMeleePlayerController()
{

}

void AMeleePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HubOverlayClass) 
	{
		HUBOverlay = CreateWidget<UUserWidget>(this, HubOverlayClass);
		if (HUBOverlay)
		{
			HUBOverlay->AddToViewport();
			HUBOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}