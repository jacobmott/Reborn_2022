// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RB_AC_PainActor.generated.h"

class URB_AC_ActorDebugger;

UCLASS()
class REBORN_2022_API ARB_AC_PainActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARB_AC_PainActor();

  UPROPERTY(EditAnywhere, Category = "DamageConfig")
  TSubclassOf<UDamageType> DamageType;

  UPROPERTY(EditAnywhere, Category = "DamageConfig")
  float TickDmgTime;

  UPROPERTY(EditAnywhere, Category = "DamageConfig")
  float DamageAmount;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
  UStaticMeshComponent* StaticMeshComp;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh");
  URB_AC_ActorDebugger* ActorDebuggerComp;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

  //UFUNCTION(Server, unrealiable, BlueprintCallable, Category = "Network")
  UFUNCTION()
  void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  //UFUNCTION(Server, unrealiable, BlueprintCallable, Category = "Network")
  UFUNCTION()
  void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

  //UFUNCTION(NetMulticast, unreliable, BlueprintCallable, Category = "Network")
  UFUNCTION()
  void ApplyDmg(AActor* OverlappedActor);

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

private:
  TMap<FString, FTimerHandle> CurrentOverlappedActors;


 


};
