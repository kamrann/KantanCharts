// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <cmath>


struct FFloatRounding
{
	int32 Base;
	int32 Power;
	int32 Multiplier;

	FFloatRounding() :
		Base(10)
		, Power(0)
		, Multiplier(1)
	{}

	inline double GetStepValue() const
	{
		return std::pow< double >(Base, Power) * Multiplier;
	}

	inline FString ExponentAsString() const
	{
		return FString::FromInt(Base) + TEXT("^") + FString::FromInt(Power);
	}
};

struct FFloatRoundedValue
{
	FFloatRounding Rounding;
	int32 StepNumber;

	static inline FFloatRoundedValue FromFloat(float InValue, FFloatRounding const& InRounding)
	{
		auto const Quotient = InValue / InRounding.GetStepValue();
		return FFloatRoundedValue(
			InRounding,
			FMath::RoundToInt(Quotient)
			);
	}

	static inline FFloatRoundedValue FromFloatRoundedUp(float InValue, FFloatRounding const& InRounding)
	{
		return FFloatRoundedValue(
			InRounding,
			FMath::CeilToInt(InValue / InRounding.GetStepValue())
			);
	}

	static inline FFloatRoundedValue FromFloatRoundedDown(float InValue, FFloatRounding const& InRounding)
	{
		return FFloatRoundedValue(
			InRounding,
			FMath::FloorToInt(InValue / InRounding.GetStepValue())
			);
	}

	// @TODO: Rounded towards/away from zero also?

	inline float GetFloatValue() const
	{
		return Rounding.GetStepValue() * StepNumber;
	}

	inline int32 GetMultiplier() const
	{
		return StepNumber * Rounding.Multiplier;
	}

	inline int32 GetBase() const
	{
		return Rounding.Base;
	}

	inline int32 GetPower() const
	{
		return Rounding.Power;
	}

	FFloatRoundedValue RelativeTo(const FFloatRoundedValue& Other) const
	{
		if(Other.IsZero())
		{
			return *this;
		}

		ensure(Rounding.Base == Other.Rounding.Base);
		ensure(Abs().GetFloatValue() >= Other.Abs().GetFloatValue());
		ensure(Rounding.Power < Other.Rounding.Power);

		auto ipow = [](int32 base, int32 exp) -> int32
		{
			int result = 1;
			while(exp)
			{
				if(exp & 1)
				{
					result *= base;
				}
				exp >>= 1;
				base *= base;
			}

			return result;
		};

		// Determine how many of our steps make up one of Other's steps.
		int32 Quotient = ipow(Rounding.Base, Other.Rounding.Power - Rounding.Power) * Other.Rounding.Multiplier / Rounding.Multiplier;

		int32 OtherConvertedSteps = Quotient * Other.StepNumber;
		ensure(FMath::Abs(StepNumber) >= FMath::Abs(OtherConvertedSteps));
		int32 RelativeStepNumber = StepNumber - OtherConvertedSteps;
		FFloatRoundedValue Result(Rounding, RelativeStepNumber);
		return Result;
	}
	
	// Necessary number of digits to display this value in raw form (without exponent)
	inline int32 RequiredDigits() const
	{
		int32 Result = 0;
		
		// Need one digit for every order of magnitude (note zero special case, it requires a digit)
		int32 Mult = FMath::Abs(GetMultiplier());
		if (Mult == 0)
		{
			++Result;
		}
		else
		{
			while (Mult > 0)
			{
				++Result;
				Mult /= 10;
			}
		}
		
		// Required zeroes either before or after decimal point
		// If multiplier is 0 and the rounding power is not negative, we don't need to display leading zeroes
		// (i.e. if step size is non-fractional, just show 0. but if step is e.g. 0.1, should show 0.0)
		if (GetMultiplier() != 0 || Rounding.Power < 0)
		{
			// @TODO: assuming base 10 rounding here!
			if(Rounding.Power >= 0)
			{
				Result += FMath::Abs(Rounding.Power);
			}
			else
			{
				Result = FMath::Max(Result, FMath::Abs(Rounding.Power) + 1);
			}
		}

		return Result;
	}

	// Calculates the power P to which Base must be raised for the value to be displayed in no more than MaxDigits
	// (i.e. Display x Base^P)
	inline int32 RequiredPowerForDisplay(int MaxDigits)
	{
		auto Digits = RequiredDigits();
		auto Diff = Digits - MaxDigits;
		if (Diff > 0)
		{
			auto Power = ((Diff + 2) / 3) * 3;
			return Rounding.Power > 0 ? Power : -Power;
		}
		else
		{
			return 0;
		}
	}

	inline FString MultiplierAsString(int32 DisplayPower) const
	{
		FNumberFormattingOptions Fmt;
		Fmt.SetUseGrouping(false);
		auto RemainingPower = Rounding.Power - DisplayPower;
		auto DecimalPlaces = FMath::Max(-RemainingPower, 0);
		Fmt.SetMinimumFractionalDigits(DecimalPlaces);
		Fmt.SetMaximumFractionalDigits(DecimalPlaces);

		auto Text = FText::Format(FText::FromString(TEXT("{0}")),
			FText::AsNumber(GetFloatValue() * FMath::Pow(10.0f, -DisplayPower), &Fmt)
			);
		return Text.ToString();
	}

	inline FString ExponentAsString() const
	{
		return Rounding.ExponentAsString();
	}

	inline FString AsString() const
	{
		return{};// @TODO MultiplierAsString() + TEXT("x") + ExponentAsString();
	}


	inline void StepUp()
	{
		++StepNumber;
	}

	inline void StepDown()
	{
		--StepNumber;
	}

	inline FFloatRoundedValue& operator++()
	{
		StepUp();
		return *this;
	}

	inline FFloatRoundedValue& operator--()
	{
		StepDown();
		return *this;
	}

	inline FFloatRoundedValue operator++(int)
	{
		auto Temp = *this;
		++(*this);
		return Temp;
	}

	inline FFloatRoundedValue operator--(int)
	{
		auto Temp = *this;
		--(*this);
		return Temp;
	}

	inline FFloatRoundedValue Abs() const
	{
		FFloatRoundedValue Result(*this);
		Result.StepNumber = FMath::Abs(StepNumber);
		return Result;
	}

	inline bool IsZero() const
	{
		return StepNumber == 0;
	}

protected:
	inline FFloatRoundedValue(FFloatRounding const& InRounding, int32 InStepNumber) :
		Rounding(InRounding)
		, StepNumber(InStepNumber)
	{}
};


/*
Utility class for managing rounding axis values to even step sizes.
*/
class FFloatRoundingLevel
{
public:
	FFloatRoundingLevel(int32 InPower = 0, int32 InBase = 10);

public:
	// Make a rounding level with a step size no smaller than the value provided
	static FFloatRoundingLevel MakeFromMinimumStepSize(float InMinStep, int32 InBase = 10);

public:
	// Returns the step size
	inline double GetStepValue() const
	{
		return Rounding.GetStepValue();
	}

	inline int32 GetPower() const
	{
		return Rounding.Power;
	}

	// Round input values off to the current rounding level step size
	inline FFloatRoundedValue RoundUp(float InValue) const
	{
		return FFloatRoundedValue::FromFloatRoundedUp(InValue, Rounding);
	}

	inline FFloatRoundedValue RoundDown(float InValue) const
	{
		return FFloatRoundedValue::FromFloatRoundedDown(InValue, Rounding);
	}

	// Determines number of rounded step values between two unrounded inputs
	inline int32 NumStepValuesBetween(float InMin, float InMax) const
	{
		// @TODO: Deal with special case where min and max are both precisely on step values
		return FMath::TruncToInt((InMax - InMin) / GetStepValue());
	}

	inline FString ExponentAsString() const
	{
		return Rounding.ExponentAsString();
	}

public:
	// Increase/decrease the rounding level step size
	void Increase();
	void Decrease();

protected:
	inline void SetMultiplierIndex(int32 Idx)
	{
		MultiplierIdx = Idx;
		Rounding.Multiplier = MultiplierValues[Idx];
	}

protected:
	// @TODO: Either replace these with float/double for speed, or, since all rounding levels can be
	// specified in terms of integer values, would be nice if we could somehow implement the rounding in
	// integer form.
	FFloatRounding Rounding;
	int32 MultiplierIdx;

	TArray< int32, TInlineAllocator< 4 > > MultiplierValues;
};