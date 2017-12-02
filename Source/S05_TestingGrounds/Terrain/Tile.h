// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "AI/Navigation/NavigationSystem.h"
//#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Tile.generated.h"

USTRUCT()
struct FSpawnPosition
{
	GENERATED_USTRUCT_BODY()

	FVector Location;
	float Rotation;
	float Scale;
};

USTRUCT(BlueprintType)
struct FSpawnGeneration
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int MinSpawn = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int MaxSpawn = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float Radius = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float MinScale = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float MaxScale = 1.f;
};

class UActorPool;

UCLASS()
class S05_TESTINGGROUNDS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void PlaceActors(TSubclassOf<AActor> ToSpawn, FSpawnGeneration SpawnGeneration);
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void PlaceAIPawns(TSubclassOf<APawn> ToSpawn, FSpawnGeneration SpawnGeneration);

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Navigation")
	FVector NavigationBoundsOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector MinExtent;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector MaxExtent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "pool")
	void SetPool(UActorPool* Pool);

private:
	void PositionNavMeshBounds();

	//TArray<FSpawnPosition> RandomSpawnPositions(FSpawnGeneration SpawnGeneration);

	template<class T>
	void RandomlyPlaceActors(TSubclassOf<T> ToSpawn, FSpawnGeneration SpawnGeneration);

	bool FindEmptyLocation(FVector& OutLocation, float Radius);

	void PlaceActor(TSubclassOf<AActor> ToSpawn, const FSpawnPosition& SpawnPosition);

	void PlaceActor(TSubclassOf<APawn> ToSpawn, const FSpawnPosition& SpawnPosition);

	bool CanSpawnAtLocation(FVector Location, float Radius);

	UActorPool* Pool;

	AActor* NavMeshBoundsVolume;
	
};
