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

// 코드가 들어있는 파일 이름과 함수, 그리고 라인 정보를 추가해 ArenaBattle 카테고리로 로그를 남긴다.
// 로그를 사용한 함수의 실행 시점을 파악할 때 유용하다.
#define ABLOG_S(Verbosity) UE_LOG(Nova, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)

// ABLOG_S 정보에 형식 문자열로 추가 정보를 지정해 로그를 남긴다.
#define ABLOG(Verbosity, Format, ...) UE_LOG(Nova, Verbosity, TEXT("%s [ %s ]"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// 런타임에 문제가 발생할 때 붉은색으로 에러 로그를 뿌리고 바로 함수를 반환하는 기능
// ABCHECK 매크로 안의 조건이 참이 아닌 경우 로그가 붉게 뜬다.
#define ABCHECK(Expr, ...) {if(!(Expr)) { ABLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}