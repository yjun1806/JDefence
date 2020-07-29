// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once
#pragma warning(disable:4668)   // x  is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#pragma warning(disable:4005)   // 'TEXT': macro redefinition


#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformAtomics.h"
#include "windows.h"
#include "HideWindowsPlatformAtomics.h"
#endif
#include "Engine/World.h"
#include "EngineMinimal.h"


DECLARE_LOG_CATEGORY_EXTERN(Nova, Log, All);
#define ABLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__)+ TEXT(")"))

// �ڵ尡 ����ִ� ���� �̸��� �Լ�, �׸��� ���� ������ �߰��� ArenaBattle ī�װ��� �α׸� �����.
// �α׸� ����� �Լ��� ���� ������ �ľ��� �� �����ϴ�.
#define ABLOG_S(Verbosity) UE_LOG(Nova, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)

// ABLOG_S ������ ���� ���ڿ��� �߰� ������ ������ �α׸� �����.
#define ABLOG(Verbosity, Format, ...) UE_LOG(Nova, Verbosity, TEXT("%s [ %s ]"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// ��Ÿ�ӿ� ������ �߻��� �� ���������� ���� �α׸� �Ѹ��� �ٷ� �Լ��� ��ȯ�ϴ� ���
// ABCHECK ��ũ�� ���� ������ ���� �ƴ� ��� �αװ� �Ӱ� ���.
#define ABCHECK(Expr, ...) {if(!(Expr)) { ABLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}