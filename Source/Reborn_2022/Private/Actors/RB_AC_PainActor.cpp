// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RB_AC_PainActor.h"
#include "RB_AC_ActorDebugger.h"

// Sets default values
ARB_AC_PainActor::ARB_AC_PainActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

  StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
  StaticMeshComp->SetupAttachment(RootComponent);

  ActorDebuggerComp = CreateDefaultSubobject<URB_AC_ActorDebugger>(TEXT("ActorDebuggerComp"));

  TickDmgTime = 2.0f; // 2 seconds
  DamageAmount = 10.0f;

}

// Called when the game starts or when spawned
void ARB_AC_PainActor::BeginPlay()
{
	Super::BeginPlay();

  StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ARB_AC_PainActor::OnOverlapBegin);
  StaticMeshComp->OnComponentEndOverlap.AddDynamic(this, &ARB_AC_PainActor::OnOverlapEnd);

  ActorDebuggerComp->AddDebuggerActor(this);

	SetReplicates(true);

}


void ARB_AC_PainActor::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    AActor* OverlappedActor = nullptr;

    if (OtherActor){
      ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapBegin: OtherActor: ") + OtherActor->GetName(), true, false, FColor::Green, ActorDebuggerComp->GetGroupEnumValue(1), this);
      OverlappedActor = OtherActor;
    }
    else if (OverlappedComp) {
      ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapBegin: OverlappedComp:  ")+OverlappedComp->GetName(), true, false, FColor::Green, ActorDebuggerComp->GetGroupEnumValue(1), this);
      OverlappedActor = OverlappedComp->GetOwner();
    }
    else if (OtherComp) {
      ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapBegin: OtherComp: ") + OtherComp->GetName(), true, false, FColor::Green, ActorDebuggerComp->GetGroupEnumValue(1), this);
      OverlappedActor = OverlappedComp->GetOwner();
    }
    else {
      ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapBegin: Didnt have anything? "), true, false, FColor::Green, ActorDebuggerComp->GetGroupEnumValue(1), this);
      return;
    }

    if (!OverlappedActor){ return; }

    FTimerDelegate TimerDel;
    //Binding the function with specific values
    TimerDel.BindUFunction(this, FName("ApplyDmg"), OverlappedActor);
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, TickDmgTime, true);
    //Have to store the timer after SetTimer call becuase it gets its value set in the SetTimer call
    CurrentOverlappedActors.Add(OverlappedActor->GetName(), TimerHandle);
    ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapBegin: TimerHandle: ") + TimerHandle.ToString(), true, false, FColor::Blue, ActorDebuggerComp->GetGroupEnumValue(2), this);
}


void ARB_AC_PainActor::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
  int32 OtherBodyIndex)
{

  AActor* OverlappedActor = nullptr;

  if (OtherActor) {
    ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapEnd: OtherActor: ") + OtherActor->GetName(), true, false, FColor::Green, ActorDebuggerComp->GetGroupEnumValue(2), this);
    OverlappedActor = OtherActor;
  }
  else if (OverlappedComp) {
    ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapEnd: OverlappedComp:  ") + OverlappedComp->GetName(), true, false, FColor::Green, ActorDebuggerComp->GetGroupEnumValue(2), this);
    OverlappedActor = OverlappedComp->GetOwner();
  }
  else if (OtherComp) {
    ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapEnd: OtherComp: ") + OtherComp->GetName(), true, false, FColor::Green, ActorDebuggerComp->GetGroupEnumValue(2), this);
    OverlappedActor = OverlappedComp->GetOwner();
  }
  else {
    ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapEnd: Didnt have anything? "), true, false, FColor::Green, ActorDebuggerComp->GetGroupEnumValue(2), this);
    return;
  }

  if (!OverlappedActor) { return; }

  FTimerHandle TimerHandle = CurrentOverlappedActors[OverlappedActor->GetName()];
  ActorDebuggerComp->PrintString(GetWorld(), TEXT("OnOverlapEnd: TimerHandle: ") + TimerHandle.ToString(), true, false, FColor::Blue, ActorDebuggerComp->GetGroupEnumValue(2), this);
  GetWorldTimerManager().ClearTimer(TimerHandle);
  CurrentOverlappedActors.Remove(OverlappedActor->GetName());

}

void ARB_AC_PainActor::ApplyDmg(AActor* OverlappedActor)
{
  ActorDebuggerComp->PrintString(GetWorld(), TEXT("ARB_AC_PainActor: ApplyDmg: "), true, false, FColor::Cyan, ActorDebuggerComp->GetGroupEnumValue(3), this);
  //ActorDebuggerComp->PrintString(GetWorld(), TEXT("ApplyDmg"), true, false, FColor::Red, 5.0f);
  //Server needs to handle damage, otherwise client and server will handle damage
  //Client may want to do something here though, like particle effects
  if (HasAuthority()){
    OverlappedActor->TakeDamage(DamageAmount, FDamageEvent(DamageType), nullptr, GetOwner());
  }
}



// Called every frame
//void ARB_AC_PainActor::Tick(float DeltaTime)
//{

  //Super::Tick(DeltaTime);

  //TimePassed += DeltaTime;
  //if (OverlapedActor){
  //  TimePassed += DeltaTime;
  //  if ((TimePassed >= TickDmgTime)) {
  //    // This needs to be fixed: TODO: we are asking each client to update the same actor
  //    //Thats why we see 3 calls per actor in the screen.. and then 1 final call to get the health to 0
  //    //GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, TEXT("Ticking! and have actor"));
  //    ActorDebuggerComp->PrintString(GetWorld(), TEXT("Time Passed And Had Overlapped Actor, Need to do Dmg"), true, false, FColor::Orange, 5.0f);
  //    //ClientTakeDmg(OverlapedActor, 10.0f, FDamageEvent(DamageType), nullptr, GetOwner());
  //    TimePassed = 0.0f;
  //  }
  //}


//}



