// Copyright Dominik Peacock. All rights reserved.

#pragma once

#include "CoreMinimal.h"

namespace UE::ModuleGeneration
{
	struct FBaseOperationResult
	{
		TOptional<FString> ErrorMessage;

		bool IsSuccess() const { return !ErrorMessage.IsSet(); }
		bool IsFailure() const { return !IsSuccess(); }

		operator bool() const
		{
			return IsSuccess();
		}

	protected:
		
		FBaseOperationResult(TOptional<FString> ErrorMessage = {})
			: ErrorMessage(MoveTemp(ErrorMessage))
		{}
	};
	
	template<typename T>
	struct TOperationResult : FBaseOperationResult
	{
		/** Set on success */
		TOptional<T> OperationResult;

		static TOperationResult<T> MakeSuccess(T Result)
		{
			return TOperationResult(MoveTemp(Result));
		}

		static TOperationResult<T> MakeFailure(FString Failure)
		{
			return TOperationResult(Failure);
		}

		template<typename Other>
		static TOperationResult<T> MakeFailure(const TOperationResult<Other>& Operation)
		{
			check(Operation.IsFailure());
			return TOperationResult::MakeFailure(Operation.ErrorMessage.GetValue());
		}

	private:
		TOperationResult(T Result)
			: OperationResult(MoveTemp(Result))
		{}
		TOperationResult(FString ErrorMessage = {})
			: FBaseOperationResult(MoveTemp(ErrorMessage))
		{}
	};
	
	template<>
	struct TOperationResult<void> : FBaseOperationResult
	{
		static TOperationResult<void> MakeSuccess()
		{
			return TOperationResult();
		}

		static TOperationResult<void> MakeFailure(FString Failure)
		{
			return TOperationResult(Failure);
		}

		template<typename Other>
		static TOperationResult<void> MakeFailure(const TOperationResult<Other>& Operation)
		{
			check(Operation.IsFailure());
			return TOperationResult<void>(Operation.ErrorMessage);
		}
		
	private:
		TOperationResult(TOptional<FString> ErrorMessage = {})
			: FBaseOperationResult(MoveTemp(ErrorMessage))
		{}
	};
	using FOperationResult = TOperationResult<void>;
}