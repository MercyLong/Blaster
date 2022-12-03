// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MutiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "InterFaces/OnlineSessionInterface.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch)
{
	// 
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	// 从游戏实例中获取多人会话子系统
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MutiPlayerSessionsSubsystem = GameInstance->GetSubsystem<UMutiplayerSessionsSubsystem>();
	}

	// 绑定回调函数
	if (MutiPlayerSessionsSubsystem)
	{
		MutiPlayerSessionsSubsystem->MutiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MutiPlayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MutiPlayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MutiPlayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MutiPlayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

// 初始化函数
//
bool UMenu::Initialize()
{
	// super 父类
	if (!Super::Initialize())
	{
		return false;
	}

	// 为按钮绑定点击事件
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTeardown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

// 回调函数
// 创建会话的回调函数
void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Session created successfully"))
			);
		}

		// 创建会话成功后跳转至大厅
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Faild to create session"))
			);
		}
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccessful)
{

}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{

}

void UMenu::OnDestroySession(bool bWasSuccessful)
{

}

void UMenu::OnStartSession(bool bWasSuccessful)
{

}

// 按钮
//
void UMenu::HostButtonClicked()
{
	// 调用创建会话函数
	if (MutiPlayerSessionsSubsystem)
	{
		MutiPlayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Green,
			FString(TEXT("JoinButton Clicked"))
		);
	}
}

// 菜单剔除函数
// 在完成创建会话后调用
void UMenu::MenuTeardown()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeDate;
			PlayerController->SetInputMode(InputModeDate);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
