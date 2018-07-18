// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnrealClient.h"


/**
* This class may only be accessed from the render thread
*/
class FSimpleRenderTarget : public FRenderTarget
{
public:
	/** FRenderTarget interface */
	virtual FIntPoint GetSizeXY() const
	{
		return ViewRect.Size();//ClippingRect.Size();
	}

	/** Sets the texture that this target renders to */
	void SetRenderTargetTexture(FTexture2DRHIRef& InRHIRef)
	{
		RenderTargetTextureRHI = InRHIRef;
	}

	/** Clears the render target texture */
	void ClearRenderTargetTexture()
	{
		RenderTargetTextureRHI.SafeRelease();
	}

	/** Sets the viewport rect for the render target */
	void SetViewRect(const FIntRect& InViewRect)
	{
		ViewRect = InViewRect;
	}

	/** Gets the viewport rect for the render target */
	const FIntRect& GetViewRect() const
	{
		return ViewRect;
	}

	/** Sets the clipping rect for the render target */
	void SetClippingRect(const FIntRect& InClippingRect)
	{
		ClippingRect = InClippingRect;
	}

	/** Gets the clipping rect for the render target */
	const FIntRect& GetClippingRect() const
	{
		return ClippingRect;
	}

	const FSlateRenderTransform& GetRenderTransform() const
	{
		return RenderTransform;
	}

	void SetRenderTransform(const FSlateRenderTransform& InRenderTransform)
	{
		RenderTransform = InRenderTransform;
	}

private:
	FIntRect ViewRect;
	FIntRect ClippingRect;
	FSlateRenderTransform RenderTransform;
};


