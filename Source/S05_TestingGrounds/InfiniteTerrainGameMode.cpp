// Fill out your copyright notice in the Description page of Project Settings.

#include "InfiniteTerrainGameMode.h"

void AInfiniteTerrainGameMode::PopulateBoundsVolumePool()
{
	TActorIterator<ANavMeshBoundsVolume> VolumeIterator = TActorIterator<ANavMeshBoundsVolume>(GetWorld());
	while (VolumeIterator)
	{
		AddToPool(*VolumeIterator);		
		++VolumeIterator; // VolumeIterator++ is invalid.
	}
}

void AInfiniteTerrainGameMode::AddToPool(ANavMeshBoundsVolume* VolumeToAdd)
{
	UE_LOG(LogTemp, Warning, TEXT("Volume to add: %s"), *VolumeToAdd->GetName())
}
