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
		HUDOverlay = CreateWidget<UUserWidget>(this, HubOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}