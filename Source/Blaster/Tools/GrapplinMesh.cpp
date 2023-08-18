// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplinMesh.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

AGrapplinMesh::AGrapplinMesh()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	splineMesh = this->GetSplineMeshComponent();
}

// Called every frame
void AGrapplinMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsSpawned) return;

	if (mainCharacter)
	{
		FTransform actorTransform = this->GetActorTransform();
		FTransform grappleTransform = mainCharacter->getGrappleTransform()->GetComponentTransform();
		FVector localStartPosition = UKismetMathLibrary::InverseTransformLocation(actorTransform, grappleTransform.GetLocation());
		splineMesh->SetStartPosition(localStartPosition,true);
	}
}

void AGrapplinMesh::setCharacter(ABlasterCharacter* character)
{
	mainCharacter = character;
}

void AGrapplinMesh::setPoints(FVector endPoint)
{
	if (mainCharacter)
	{
		FTransform actorTransform = this->GetActorTransform();
		FTransform grappleTransform = mainCharacter->getGrappleTransform()->GetComponentTransform();
		FVector localStartPosition = UKismetMathLibrary::InverseTransformLocation(actorTransform,grappleTransform.GetLocation());
		FVector  localEndPosition = UKismetMathLibrary::InverseTransformLocation(actorTransform, endPoint);
		FVector zeroVector = FVector(0.0f, 0.0f, 0.0f);
		splineMesh->SetStartAndEnd(localStartPosition, zeroVector, localEndPosition, zeroVector, true);
		bIsSpawned = true;
	}
	

}

