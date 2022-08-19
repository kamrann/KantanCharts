// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "KantanChartsSlate.h"
#include "SKantanPieChart.h"
#include "SimpleRenderTarget.h"


class FKantanPieElement : public ICustomSlateElement
{
public:
	FKantanPieElement();
	~FKantanPieElement();

	struct FPieSegmentRenderInfo
	{
		float AngleRadians;
		FMaterialRenderProxy* MatProxy;
		FLinearColor Color;
		/* @TODO: Setting the color through a dyn material parameter seems to give gamma correction problem,
		resulting in a different visual color to, for example, a bar chart bar using the same category style.
		Passing the color through the vertex color node to the material appears to work though. Check this.
		*/
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
	bool BeginRenderingCanvas(const FIntRect& InCanvasRect, const FIntRect& InClippingRect, /*FMaterialRenderProxy* MatProxy, */const TArray< FPieSegmentRenderInfo >& Segments);

private:
	/**
	* ICustomSlateElement interface
	*/
	virtual void DrawRenderThread(FRHICommandListImmediate& RHICmdList, const void* InWindowBackBuffer) override;

private:
	/** Render target that the canvas renders to */
	TUniquePtr< class FSimpleRenderTarget > RenderTarget;

	TArray< FPieSegmentRenderInfo > SegmentInfo;
};


SKantanPieChart::~SKantanPieChart()
{
	// Pass ownership of the pie element to the render thread so that it's deleted when the
	// render thread is done with it
	// @TODO: What about the UMaterialInstanceDynamics which the element currently holds material proxies for?
	// Are proxies independent of the object they represent?
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER
		(
		SafeDeletePieElement,
		FPieElementPtr, PieElem, PieElement,
		{
			PieElem.Reset();
		}
	);
}

void SKantanPieChart::Construct(const FArguments& InArgs)
{
	SKantanCategoryChart::Construct(
		SKantanCategoryChart::FArguments()
		.Datasource(InArgs._Datasource)
		.UpdateTickRate(InArgs._UpdateTickRate)
		);

	SetStyle(InArgs._Style);
	SetPieMaterial(InArgs._PieMaterial);

	PieElement = MakeShareable< FKantanPieElement >(new FKantanPieElement());
}

void SKantanPieChart::OnActiveTick(double InCurrentTime, float InDeltaTime)
{
	SKantanCategoryChart::OnActiveTick(InCurrentTime, InDeltaTime);

	if (Datasource != nullptr)
	{
		if (IsMaterialValid())
		{
			auto NumCats = GetNumCategories();

			// Make sure we have a material instance for every category
			while (SegmentMaterialInstances.Num() < NumCats)
			{
				// @TODO: Is okay use of transient package??
				auto MatInst = UMaterialInstanceDynamic::Create(PieMaterial, GetTransientPackage());

				// test
				// @TODO: This test appears to have fixed the issue with the parameter sometimes apparently not
				// getting set in the update. Find out what is going on.
				MatInst->SetScalarParameterValue(FName("AngleA"), -PI / 4);
				MatInst->SetScalarParameterValue(FName("AngleB"), PI / 4);
				//

				SegmentMaterialInstances.Add(MatInst);
			}
		}
	}
}

void SKantanPieChart::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SKantanCategoryChart::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SKantanPieChart::SetStyle(const FKantanPieChartStyle* InStyle)
{
	Style = InStyle;
}

void SKantanPieChart::SetPieMaterial(UMaterialInterface* InMaterial)
{
	PieMaterial.Reset(InMaterial);

	// @TODO: If this is done dynamically, do we need to somehow keep these old instances alive to prevent the
	// unlikely scenario of them being GCd in the small delay before the render thread catches up?
	// Is there something in place to deal with FMaterialRenderProxy having it's parent UMaterial destroyed?
	SegmentMaterialInstances.Empty(SegmentMaterialInstances.Num());
}

float SKantanPieChart::GetChartAreaSize(EChartContentArea::Type Area, EAxis::Type ReqComp, FVector2D const& KnownPlotSize) const
{
	return 0.0f;
}

int32 SKantanPieChart::DrawChartArea(
	EChartContentArea::Type Area,
	const FPaintArgs& Args,
	const FGeometry& Geometry,
	const FGeometry& PlotSpaceGeometry,
	const FSlateRect& MyClippingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
	) const
{
	// Used to track the layer ID we will return.
	int32 RetLayerId = LayerId;

	if (Area != EChartContentArea::Plot)
	{
		return RetLayerId;
	}

	bool bEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect::Type DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	
	// Pre-snap the clipping rect to try and reduce common jitter, since the padding is typically only a single pixel.
	FSlateRect SnappedClippingRect = FSlateRect(FMath::RoundToInt(MyClippingRect.Left), FMath::RoundToInt(MyClippingRect.Top), FMath::RoundToInt(MyClippingRect.Right), FMath::RoundToInt(MyClippingRect.Bottom));

	auto ChartStyle = GetChartStyle();
	
	FSlateRect SlateCanvasRect = Geometry.GetClippingRect();
	FSlateRect ClippedCanvasRect = SlateCanvasRect.IntersectionWith(MyClippingRect);

	FIntRect CanvasRect(
		FMath::TruncToInt(FMath::Max(0.0f, SlateCanvasRect.Left)),
		FMath::TruncToInt(FMath::Max(0.0f, SlateCanvasRect.Top)),
		FMath::TruncToInt(FMath::Max(0.0f, SlateCanvasRect.Right)),
		FMath::TruncToInt(FMath::Max(0.0f, SlateCanvasRect.Bottom)));

	FIntRect ClippingRect(
		FMath::TruncToInt(FMath::Max(0.0f, ClippedCanvasRect.Left)),
		FMath::TruncToInt(FMath::Max(0.0f, ClippedCanvasRect.Top)),
		FMath::TruncToInt(FMath::Max(0.0f, ClippedCanvasRect.Right)),
		FMath::TruncToInt(FMath::Max(0.0f, ClippedCanvasRect.Bottom)));

	if (IsMaterialValid())
	{
		auto NumCats = GetNumCategories();

		if (NumCats > 0)
		{
			TArray< FKantanPieElement::FPieSegmentRenderInfo > Segments;
			float ValueSum = 0.0f;
			for (int32 Idx = 0; Idx < NumCats; ++Idx)
			{
				//
				if (!/*ensure*/(SegmentMaterialInstances.IsValidIndex(Idx)))
				{
					UE_LOG(LogTemp, Error, TEXT("KantanPieChart - drawing without pie element initialized!"));
					break;
				}
				//

				FKantanPieElement::FPieSegmentRenderInfo Seg;

				auto Value = GetCategoryValue(Idx);
				Seg.AngleRadians = Value;
				Segments.Add(Seg);

				ValueSum += Value;
			}

			float BaseAngle = 0.0f;
			for (int32 Idx = 0; Idx < Segments.Num()/*NumCats*/; ++Idx)
			{
				Segments[Idx].AngleRadians = 2 * PI * Segments[Idx].AngleRadians / ValueSum;

				auto const& CatStyle = GetCategoryStyle(GetCategoryId(Idx));

				auto Mat = SegmentMaterialInstances[Idx];
				Mat->SetScalarParameterValue(FName("AngleA"), BaseAngle);
				Mat->SetScalarParameterValue(FName("AngleB"), BaseAngle + Segments[Idx].AngleRadians);
				// @TODO: probably cleaner to have a separate Opacity parameter on the material
				// @TODO: Passing colors as dynamic parameters appears to skip gamma correction, meaning
				// we get an inconsistency between a pie chart and a bar chart drawn with the same color styles.
				// So, currently this is unused and the pie material is set up to use the vertex color instead.
				// Not sure what to do for OutlineColor though.
				Mat->SetVectorParameterValue(FName("BaseColor"), CatStyle.Color.CopyWithNewOpacity(ChartStyle->PieOpacity));
				Mat->SetVectorParameterValue(FName("OutlineColor"), FLinearColor::White);	// @TODO
				Segments[Idx].MatProxy = Mat->GetRenderProxy(false);

				Segments[Idx].Color = CatStyle.Color.CopyWithNewOpacity(ChartStyle->PieOpacity);

				BaseAngle += Segments[Idx].AngleRadians;
			}

			// @TODO: Do I need to do anything regarding thread safety and/or resource release for the mat render proxy???
			if (PieElement->BeginRenderingCanvas(CanvasRect, ClippingRect, Segments))
			{
				FSlateDrawElement::MakeCustom(OutDrawElements, RetLayerId++, PieElement);
			}
		}
	}
	else
	{
		// @TODO: Draw error text
	}

	return RetLayerId - 1;
}



FKantanPieElement::FKantanPieElement() : RenderTarget(MakeUnique< FSimpleRenderTarget >())
{}

FKantanPieElement::~FKantanPieElement()
{}

bool FKantanPieElement::BeginRenderingCanvas(const FIntRect& InCanvasRect, const FIntRect& InClippingRect, const TArray< FPieSegmentRenderInfo >& Segments)
{
	if (InCanvasRect.Size().X > 0 && InCanvasRect.Size().Y > 0 && InClippingRect.Size().X > 0 && InClippingRect.Size().Y > 0)
	{
		/**
		* Struct to contain all info that needs to be passed to the render thread
		*/
		struct FPieRenderInfo
		{
			/** Size of the Canvas tile */
			FIntRect CanvasRect;
			/** How to clip the canvas tile */
			FIntRect ClippingRect;

			TArray< FPieSegmentRenderInfo > SegmentInfo;
		};

		FPieRenderInfo RenderInfo;
		RenderInfo.CanvasRect = InCanvasRect;
		RenderInfo.ClippingRect = InClippingRect;
		RenderInfo.SegmentInfo = Segments;

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER
			(
			BeginRenderingKantanPieCanvas,
			FKantanPieElement*, PieElement, this,
			FPieRenderInfo, InRenderInfo, RenderInfo,
			{
				PieElement->RenderTarget->SetViewRect(InRenderInfo.CanvasRect);
				PieElement->RenderTarget->SetClippingRect(InRenderInfo.ClippingRect);
				PieElement->SegmentInfo = InRenderInfo.SegmentInfo;
			}
		);
		return true;
	}

	return false;
}

void FKantanPieElement::DrawRenderThread(FRHICommandListImmediate& RHICmdList, const void* InWindowBackBuffer)
{
	static const float RadiusMultiplier = FMath::Sqrt(2.0f) + 0.01f;

	FIntRect ViewRect = RenderTarget->GetViewRect();
	FIntRect ClippingRect = RenderTarget->GetClippingRect();

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
		float CurrentTime = FApp::GetCurrentTime() - GStartTime;
		float DeltaTime = FApp::GetDeltaTime();

		FCanvas Canvas(RenderTarget.Get(), nullptr, CurrentTime, CurrentTime, DeltaTime, GMaxRHIFeatureLevel);
		{
			Canvas.SetAllowedModes(0);
			Canvas.SetRenderTargetRect(RenderTarget->GetViewRect());

			auto Origin = FVector2D(
				ViewRect.Width() * 0.5f,
				ViewRect.Height() * 0.5f
				);
			auto Radius = 0.5f * FMath::Min(ViewRect.Width(), ViewRect.Height());
			auto TriRadius = Radius * RadiusMultiplier;

			auto NumSegments = SegmentInfo.Num();
			float BaseAngle = 0.0f;
			for (int32 Idx = 0; Idx < NumSegments; ++Idx)
			{
				auto AngleStillToRender = SegmentInfo[Idx].AngleRadians;
				while (AngleStillToRender > 0.0f)
				{
					// Draw maximum of 90 degrees at a time (this ensures the triangle will not cut inside the circle radius)
					auto SubAngle = FMath::Min(AngleStillToRender, PI * 0.5f);

					float EndAngle = /*(Idx == NumSegments - 1) ? 0.0f :*/ (BaseAngle + SubAngle);

					auto StartOffsetProp = FVector2D(
						FMath::Sin(BaseAngle),
						-FMath::Cos(BaseAngle)
						);
					auto StartOffset = StartOffsetProp * TriRadius;

					auto EndOffsetProp = FVector2D(
						FMath::Sin(EndAngle),
						-FMath::Cos(EndAngle)
						);
					auto EndOffset = EndOffsetProp * TriRadius;

					auto VertexColor = SegmentInfo[Idx].Color;	/*FLinearColor::White;*/

					auto const UVCenter = FVector2D::ZeroVector;
					FCanvasUVTri Tri;
					Tri.V0_Pos = Origin;
					Tri.V0_UV = UVCenter;
					Tri.V0_Color = VertexColor;
					Tri.V1_Pos = Origin + StartOffset;
					Tri.V1_UV = UVCenter + StartOffsetProp * RadiusMultiplier;
					Tri.V1_Color = VertexColor;
					Tri.V2_Pos = Origin + EndOffset;
					Tri.V2_UV = UVCenter + EndOffsetProp * RadiusMultiplier;
					Tri.V2_Color = VertexColor;
					FCanvasTriangleItem TriItem(Tri, nullptr);
					TriItem.MaterialRenderProxy = SegmentInfo[Idx].MatProxy;
					Canvas.DrawItem(TriItem);

					BaseAngle = EndAngle;
					AngleStillToRender -= SubAngle;
				}
			}
		}
		Canvas.Flush_RenderThread(RHICmdList, true);
	}
	RenderTarget->ClearRenderTargetTexture();
	RHICmdList.SetScissorRect(false, 0, 0, 0, 0);
}
