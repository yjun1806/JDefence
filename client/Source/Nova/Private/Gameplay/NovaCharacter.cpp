// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NovaCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/WidgetComponent.h"
#include "NovaGameInstance.h"
//#include "NovaCharacterHpBarWidget.h"
#include "NovaPlayerController.h"

//////////////////////////////////////////////////////////////////////////
// ANovaCharacter

ANovaCharacter::ANovaCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NovaCharacter"));

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	


	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));



	//애니메이션 설정 부분
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	

	AttackingCountNumber = -1;
	IsAttacking = false;
	CharacterCurrentHP = 1000.0f;
	CharacterMaxHP = 1000.0f;
	IsPlayerAlive = true;
	

	//

	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bDoCollisionTest = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	IsSkill_1_CoolDown = false;
	IsSkill_2_CoolDown = false;
	IsSkillUsing = false;
	SkillUsingNumber = 0;

}

void ANovaCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ANovaCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ANovaCharacter::SetCharacterName(const int32 name)
{
	CharacterID = name;
}

void ANovaCharacter::SetAttackingNumber(int32 number)
{
	AttackingCountNumber = number;
}

void ANovaCharacter::SetIsAttacking(bool boo)
{
	IsAttacking = boo;
}

void ANovaCharacter::UpdateCharacterHPbar()
{
	
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANovaCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ANovaCharacter::CharacterJump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ANovaCharacter::CharacterStopJump);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANovaCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANovaCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANovaCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANovaCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ANovaCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ANovaCharacter::TouchStopped);

	//마우스 왼쪽버튼 공격 지정
	InputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ANovaCharacter::Attack);
}



void ANovaCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		//Jump();
}

void ANovaCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		//StopJumping();
}



void ANovaCharacter::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	// SweepSingleByChannel : 하나의 물체만 충돌하도록 설정하는 메소드
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult, // 충돌 결과를 저장할 변수
		GetActorLocation(), // 첫번째 시작 위치
		GetActorLocation() + GetActorForwardVector()*200.0f, // 두번째 시작위치
		FQuat::Identity, // FQuat: 3차원 공간에서 축을 중심으로 회전을 표현할수있는 부동소수점 쿼터니언
		ECollisionChannel::ECC_GameTraceChannel2, // 몬스터공격 채널 4번
		FCollisionShape::MakeSphere(50.0f), // 충돌 형체의 모양 지름 50짜리 구체
		Params);

	if (bResult && UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
	{
		if (HitResult.Actor.IsValid())
		{
			AMonsterCharacter* mon = Cast<AMonsterCharacter>(HitResult.GetActor());
			if (mon != nullptr) {

				if (mon->IsMonsterAlive == true) {
					//UNovaGameInstance::NovaSocketcli->SendAttackInfo(CharacterID, mon->Monsterid, 1); // 캐릭터의 공격인 경우 어택타입 1~

					flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
					auto msg = object::CreateAttackInfo(builder,
						CharacterID,
						mon->Monsterid,
						1);
					auto pk = object::CreatePacket(builder, object::Type_AttackInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

					builder.Finish(pk);

					uint8_t* send_buffer = builder.GetBufferPointer();

					// ---------
					int32 sent = 0;

					// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
					bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);

					ABLOG(Warning, TEXT("Hit Attack Monster ID : %d (IsAlive : %d(1=true, 0=false)"), mon->Monsterid, mon->IsMonsterAlive);
				}
			}
		}
	}
}


void ANovaCharacter::Skill_1_Check(const FHitResult& result)
{
	if (UNovaGameInstance::MyRoomInfo->RoomOwner == UNovaGameInstance::IDcode)
	{
		if (result.Actor.IsValid())
		{
			AMonsterCharacter* mon = Cast<AMonsterCharacter>(result.GetActor());
			if (mon != nullptr) {
				if (mon->IsMonsterAlive == true) {
					//UNovaGameInstance::NovaSocketcli->SendAttackInfo(CharacterID, mon->Monsterid, 2); // 스킬 1번공격 2
					flatbuffers::FlatBufferBuilder builder(8192); // 데이터를 담을 플랫버퍼빌더를 만든다.
					auto msg = object::CreateAttackInfo(builder,
						CharacterID,
						mon->Monsterid,
						2);
					auto pk = object::CreatePacket(builder, object::Type_AttackInfo, msg.Union()); // 패킷에 묶어준다. 반드시 타입 지정!!

					builder.Finish(pk);

					uint8_t* send_buffer = builder.GetBufferPointer();

					// ---------
					int32 sent = 0;

					// 연결된 소켓에 데이터를 보내고, 결과값을 받는다.
					bool nSendLend = UNovaGameInstance::NovaSocketcli->NovaSocket->Send(send_buffer, builder.GetSize(), sent);

					ABLOG(Warning, TEXT("Hit SKill1 Monster ID : %d (IsAlive : %d(1=true, 0=false)"), mon->Monsterid, mon->IsMonsterAlive);
				}
			}
		}
	}
}

int32 ANovaCharacter::GetCharacterID()
{
	return CharacterID;
}

void ANovaCharacter::CharacterJump()
{
	ACharacter::Jump();
	//GLog->Log("Character Movement::Jump()");
}

void ANovaCharacter::CharacterStopJump()
{
	ACharacter::StopJumping();
	//GLog->Log("Character Movement::StopJumping()");

}

void ANovaCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	//UNovaGameInstance::PlayerPositionChanged = true;
	//GLog->Log("Character Movement::TurnAtRate()");

}

void ANovaCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	//UNovaGameInstance::PlayerPositionChanged = true;
	//GLog->Log("Character Movement::LookUp()");
}

void ANovaCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		//UNovaGameInstance::PlayerPositionChanged = true;
		//GLog->Log("Character Movement::MoveForward()");


	}
}

void ANovaCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
		//UNovaGameInstance::PlayerPositionChanged = true;
		//GLog->Log("Character Movement::MoveRight()");


	}
}
