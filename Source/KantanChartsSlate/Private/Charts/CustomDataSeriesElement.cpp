// Copyright (C) 2015-2017 Cameron Angus. All Rights Reserved.

#include "IDataSeriesElement.h"
#include "SimpleRenderTarget.h"

#include "RenderingCommon.h"
#include "Engine/Texture2D.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Geometry.h"
#include "DrawElements.h"


class FTexture;

class FCustomDataSeriesElement : public IDataSeriesElement, public ICustomSlateElement
{
public:
	FCustomDataSeriesElement(UTexture2D* Tex, FVector2D PntSz, FLinearColor Cl, FBox2D UVs) :
		PointTexture(Tex),
		PointSize(PntSz),
		PointUVs(UVs),
		Color(Cl),
		RenderTarget(MakeUnique< FSimpleRenderTarget >()),
		RenderData()
	{}

	virtual FSlateRenderTransform GetPointTransform(const FSlateRenderTransform& CartesianToPlotXform, const FGeometry& PlotSpaceGeometry) const override;
	virtual void RenderSeries(
		const FGeometry& PlotSpaceGeometry,
		const FSlateRect& ClipRect,
		TArray< FVector2D >&& Points,
		int32 LayerId,
		FSlateWindowElementList& OutDrawElements
	) override;

private:
	TWeakObjectPtr< UTexture2D > PointTexture;
	FVector2D PointSize;
	FBox2D PointUVs;
	FLinearColor Color;

	struct FRenderData
	{
		FTexture* TextureResource;
		FVector2D UV_0;
		FVector2D UV_1;
		FLinearColor Color;
		FVector2D PointSize;
		TArray< FVector2D > Points;
	};

	/**
	* Sets up the canvas for rendering
	*
	* @param	InCanvasRect			Size of the canvas tile
	* @param	InClippingRect			How to clip the canvas tile
	* @param	InExpressionPreview		Render proxy for the Material preview
	* @param	bInIsRealtime			Whether preview is using realtime values
	*
	* @return	Whether there is anything to render
	*/
	bool BeginRenderingCanvas(
		const FIntRect& InCanvasRect,
		const FIntRect& InClippingRect,
		UTexture2D* InTexture, 
		FVector2D InUV_0, 
		FVector2D InUV_1, 
		FLinearColor InColor, 
		FVector2D InPointSize, 
		TArray< FVector2D >&& InPoints
	);

	/**
	* ICustomSlateElement interface
	*/
	virtual void DrawRenderThread(FRHICommandListImmediate& RHICmdList, const void* InWindowBackBuffer) override;

private:
	/** Render target that the canvas renders to */
	TUniquePtr< class FSimpleRenderTarget > RenderTarget;
	FRenderData RenderData;
};


bool FCustomDataSeriesElement::BeginRenderingCanvas(const FIntRect& InCanvasRect, const FIntRect& InClippingRect, UTexture2D* InTexture, FVector2D InUV_0, FVector2D InUV_1, FLinearColor InColor, FVector2D InPointSize, TArray< FVector2D >&& InPoints)
{
	if (InCanvasRect.Size().X > 0 && InCanvasRect.Size().Y > 0 && InClippingRect.Size().X > 0 && InClippingRect.Size().Y > 0 && InPoints.Num() > 0)
	{
		/**
		* Struct to contain all info that needs to be passed to the render thread
		*/
		struct FSeriesRenderInfo
		{
			/** Size of the Canvas tile */
			FIntRect CanvasRect;
			/** How to clip the canvas tile */
			FIntRect ClippingRect;

			TSharedPtr< FRenderData > RenderData;
		};

		FSeriesRenderInfo RenderInfo;
		RenderInfo.CanvasRect = InCanvasRect;
		RenderInfo.ClippingRect = InClippingRect;
		RenderInfo.RenderData = MakeShareable< FRenderData >(new FRenderData);
		RenderInfo.RenderData->TextureResource = InTexture ? InTexture->Resource : GWhiteTexture;
		RenderInfo.RenderData->UV_0 = InUV_0;
		RenderInfo.RenderData->UV_1 = InUV_1;
		RenderInfo.RenderData->Color = InColor;
		RenderInfo.RenderData->PointSize = InPointSize;
		RenderInfo.RenderData->Points = MoveTemp(InPoints);

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER
			(
			BeginRenderingKantanCartesianSeriesCanvas,
			FCustomDataSeriesElement*, SeriesElement, this,
			FSeriesRenderInfo, InRenderInfo, MoveTemp(RenderInfo),
			{
				SeriesElement->RenderTarget->SetViewRect(InRenderInfo.CanvasRect);
				SeriesElement->RenderTarget->SetClippingRect(InRenderInfo.ClippingRect);
				SeriesElement->RenderData = MoveTemp(*InRenderInfo.RenderData);
			}
		);
		return true;
	}

	return false;
}

void FCustomDataSeriesElement::DrawRenderThread(FRHICommandListImmediate& RHICmdList, const void* InWindowBackBuffer)
{
	const FIntRect ViewRect = RenderTarget->GetViewRect();
	// Clip the canvas to avoid having to set UV values
	// @test: FIntRect ClippingRect = FIntRect(FIntPoint(0, 0), FIntPoint(2, 2));
	const FIntRect ClippingRect = RenderTarget->GetClippingRect();

	// @TODO: The scissor rect appears to be ignored...
	RHICmdList.SetScissorRect(
		true,
		ClippingRect.Min.X,
		ClippingRect.Min.Y,
		ClippingRect.Max.X,
		ClippingRect.Max.Y
		);
	RenderTarget->SetRenderTargetTexture(*(FTexture2DRHIRef*)InWindowBackBuffer);
	{
		// Check realtime mode for whether to pass current time to canvas
		const float CurrentTime = FApp::GetCurrentTime() - GStartTime;
		const float DeltaTime = FApp::GetDeltaTime();

		FCanvas Canvas(RenderTarget.Get(), nullptr, CurrentTime, CurrentTime, DeltaTime, GMaxRHIFeatureLevel);
		{
			Canvas.SetAllowedModes(0);
			bool bTestIsScaled = Canvas.IsScaledToRenderTarget();
			Canvas.SetScaledToRenderTarget(false);
			
			FCanvasTileItem Tile(
				FVector2D(),
				RenderData.TextureResource,
				RenderData.PointSize,
				RenderData.UV_0,
				RenderData.UV_1,
				RenderData.Color);
			Tile.BlendMode = ESimpleElementBlendMode::SE_BLEND_Masked;
			for (auto const& Pnt : RenderData.Points)
			{
				Tile.Position = Pnt;
				Canvas.DrawItem(Tile);
			}
		}
		Canvas.Flush_RenderThread(RHICmdList, true);
	}
	RenderTarget->ClearRenderTargetTexture();
	
	RHICmdList.SetScissorRect(false, 0, 0, 0, 0);
}

FSlateRenderTransform FCustomDataSeriesElement::GetPointTransform(const FSlateRenderTransform& CartesianToPlotXform, const FGeometry& PlotSpaceGeometry) const
{
	return Concatenate(
		CartesianToPlotXform,
		PlotSpaceGeometry.GetAccumulatedRenderTransform()
	);
}

void FCustomDataSeriesElement::RenderSeries(
	const FGeometry& PlotSpaceGeometry,
	const FSlateRect& ClipRect,
	TArray< FVector2D >&& Points,
	int32 LayerId,
	FSlateWindowElementList& OutDrawElements
)
{
	const FIntRect CanvasRect(
		0,
		0,
		// @TODO: Hack, since we are rendering our tile later in absolute coords, seems that whatever we return from
		// FSimpleRenderTarget::GetSizeXY needs to encompass our drawing area wrt absolute origin...
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Right)),
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Bottom))
	);
	const FIntRect ClippingRect(
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Left)),
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Top)),
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Right)),
		FMath::TruncToInt(FMath::Max(0.0f, ClipRect.Bottom))
	);

	if (BeginRenderingCanvas(
		CanvasRect,
		ClippingRect,
		PointTexture.Get(),//SeriesStyle.HasValidPointStyle() ? SeriesStyle.PointStyle->DataPointTexture : nullptr,
		PointUVs.Min,
		PointUVs.Max,
		Color,//SeriesStyle.Color * FLinearColor(1, 1, 1, ChartStyle->DataOpacity),
		PointSize,//FVector2D(DP_PixelSize, DP_PixelSize),
		MoveTemp(Points)
	))
	{
		FSlateDrawElement::MakeCustom(OutDrawElements, LayerId, SharedThis(this));
	}
}


TSharedRef< IDataSeriesElement, ESPMode::ThreadSafe > MakeCustomSeriesElement(UTexture2D* Tex, FVector2D PntSz, FLinearColor Cl, FBox2D UVs)
{
	return MakeShareable(new FCustomDataSeriesElement(Tex, PntSz, Cl, UVs));
}

