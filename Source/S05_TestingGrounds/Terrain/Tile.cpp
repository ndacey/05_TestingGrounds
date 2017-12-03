// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "../ActorPool.h"


// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NavigationBoundsOffset = FVector(2000, 0, 0);
	MinExtent = FVector(0, -2000, 0);
	MaxExtent = FVector(4000, 2000, 0);
}

void ATile::SetPool(UActorPool* InPool)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Setting Pool %s"), *(this->GetName()), *(InPool->GetName()))
	Pool = InPool;

	PositionNavMeshBounds();
}

void ATile::PositionNavMeshBounds()
{
	NavMeshBoundsVolume = Pool->Checkout();
	if (NavMeshBoundsVolume == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Not enough actors in pool."), *GetName())
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("[%s] Checked out: {%s}."), *GetName(), *NavMeshBoundsVolume->GetName())
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundsOffset);
	GetWorld()->GetNavigationSystem()->Build();
}

template<class T>
void ATile::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, FSpawnGeneration SpawnGeneration)
{
	int32 NumberToSpawn = FMath::RandRange(SpawnGeneration.MinSpawn, SpawnGeneration.MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(SpawnGeneration.MinScale, SpawnGeneration.MaxScale);
		bool CanSpawn = FindEmptyLocation(SpawnPosition.Location, SpawnGeneration.Radius * SpawnPosition.Scale);
		if (CanSpawn)
		{
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			PlaceActor(ToSpawn, SpawnPosition);
		}
	}
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, FSpawnGeneration SpawnGeneration)
{
	RandomlyPlaceActors(ToSpawn, SpawnGeneration);
}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, FSpawnGeneration SpawnGeneration)
{
	RandomlyPlaceActors(ToSpawn, SpawnGeneration);
}

//TArray<FSpawnPosition> ATile::RandomSpawnPositions(FSpawnGeneration SpawnGeneration)
//{
//	TArray<FSpawnPosition> SpawnPositions;
//	int NumberToSpawn = FMath::RandRange(SpawnGeneration.MinSpawn, SpawnGeneration.MaxSpawn);
//	for (int i = 0; i < NumberToSpawn; i++)
//	{
//		FSpawnPosition SpawnPosition;
//		SpawnPosition.Scale = FMath::RandRange(SpawnGeneration.MinScale, SpawnGeneration.MaxScale);
//		bool found = FindEmptyLocation(SpawnPosition.Location, SpawnGeneration.Radius * SpawnPosition.Scale);
//		if (found)
//		{
//			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
//			SpawnPositions.Add(SpawnPosition);
//		}
//	}
//	return SpawnPositions;
//}





bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FBox Bounds(MinExtent, MaxExtent);
	const int MAX_ATTEMPTS = 100;
	for (int32 i = 0; i < MAX_ATTEMPTS; i++)
	{
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(CandidatePoint, Radius))
		{
			OutLocation = CandidatePoint;
			return true;
		}
	}
	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, const FSpawnPosition& SpawnPosition)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	if (Spawned)
	{
		Spawned->SetActorRelativeLocation(SpawnPosition.Location);
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Spawned->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
		Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
	}	
}

void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, const FSpawnPosition& SpawnPosition)
{
	FRotator Rotation = FRotator(0, SpawnPosition.Rotation, 0);
	APawn* Spawned = GetWorld()->SpawnActor<APawn>(ToSpawn, SpawnPosition.Location, Rotation);
	if (Spawned)
	{
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Spawned->SpawnDefaultController();
		Spawned->Tags.Add(FName("Enemy"));
	}	
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();	
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Pool != nullptr && NavMeshBoundsVolume != nullptr)
	{
		Pool->Return(NavMeshBoundsVolume);
	}	
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	bool bHasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);
	//DEBUG LINES
	//FColor ResultColor = bHasHit ? FColor::Red : FColor::Green;
	//DrawDebugCapsule(GetWorld(), GlobalLocation, 0, Radius, FQuat::Identity, ResultColor, true, 100);
	return !bHasHit;
}

