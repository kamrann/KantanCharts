// Copyright (C) 2015-2018 Cameron Angus. All Rights Reserved.

#include "IDataSeriesElement.h"
#include "SimpleRenderTarget.h"

#include "Rendering/RenderingCommon.h"
#include "Engine/Texture2D.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "Layout/Geometry.h"
#include "Rendering/DrawElements.h"


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

	virtual void RenderSeries(
		const FGeometry& PlotSpaceGeometry,
		const FSlateRect& ClipRect,
		const FSlateRenderTransform& CartesianToPlotXform,
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
		const FSlateRenderTransform& InTransform,
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


bool FCustomDataSeriesElement::BeginRenderingCanvas(const FIntRect& InCanvasRect, const FIntRect& InClippingRect, const FSlateRenderTransform& InTransform, UTexture2D* InTexture, FVector2D InUV_0, FVector2D InUV_1, FLinearColor InColor, FVector2D InPointSize, TArray< FVector2D >&& InPoints)
{
	if (InCanvasRect.Size().X > 0 && InCanvasRect.Size().Y > 0 && InClippingRect.Size().X > 0 && InClippingRect.Size().Y > 0 && InPoints.Num() > 0)
	{
		for (auto& Pnt : InPoints)
		{
			Pnt = InTransform.TransformPoint(Pnt);

			// Offset half of the tile size, so the point image is drawn centered on its coordinates
			Pnt -= InPointSize * 0.5f;

			// @NOTE: This seems to help avoid an infrequent issue with the tile being stretched/distorted slightly, however believe have still seen one distortion after adding this...
			Pnt = FVector2D(
				FMath::TruncToFloat(Pnt.X),
				FMath::TruncToFloat(Pnt.Y)
			);
		}

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

		ENQUEUE_RENDER_COMMAND(BeginRenderingKantanCartesianSeriesCanvas)(
			[SeriesElement = this, InRenderInfo = MoveTemp(RenderInfo)](FRHICommandListImmediate& RHICmdList)
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

void FCustomDataSeriesElement::RenderSeries(
	const FGeometry& PlotSpaceGeometry,
	const FSlateRect& ClipRect,
	const FSlateRenderTransform& CartesianToPlotXform,
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

	const auto Transform = Concatenate(
		CartesianToPlotXform,
		PlotSpaceGeometry.GetAccumulatedRenderTransform()
	);
	if (BeginRenderingCanvas(
		CanvasRect,
		ClippingRect,
		Transform,
		PointTexture.Get(),
		PointUVs.Min,
		PointUVs.Max,
		Color,
		PointSize,
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

