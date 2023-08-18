// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SplineMeshActor.h"
#include "GrapplinMesh.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AGrapplinMesh : public ASplineMeshActor
{
	GENERATED_BODY()
public:
	AGrapplinMesh();
	// function used to set the start and end point of the grappler
	void setPoints(FVector endPoint);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// setter for the blaster character reference
	void setCharacter(class ABlasterCharacter* character);
protected:
	
private:
	// the spline mesh component
	class USplineMeshComponent* splineMesh;
	//boolean that keep track if the mesh was spawned
	bool bIsSpawned;
	// the blaster character reference
	ABlasterCharacter* mainCharacter;
};
