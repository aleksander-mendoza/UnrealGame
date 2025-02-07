// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorPicker.h"



UMaterialInstanceDynamic* getDynamicMaterial(FSlateBrush& Brush)
{
	UObject* Resource = Brush.GetResourceObject();

	// If we already have a dynamic material, return it.
	if (UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Resource))
	{
		return DynamicMaterial;
	}
	// If the resource has a material interface we'll just update the brush to have a dynamic material.
	else if (UMaterialInterface* Material = Cast<UMaterialInterface>(Resource))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
		Brush.SetResourceObject(DynamicMaterial);

		return DynamicMaterial;
	}

	//TODO UMG can we do something for textures?  General purpose dynamic material for them?

	return nullptr;
}



void UColorPicker::NativeConstruct()
{
	Super::NativeConstruct();
	Color2DSlider->OnValueChangedX.AddUniqueDynamic(this, &UColorPicker::OnColorValueChange);
	BrightnessSlider->OnValueChanged.AddUniqueDynamic(this, &UColorPicker::OnBrightnessValueChange);


}
void UColorPicker::OnBrightnessValueChange(float val)
{
	//Color2DSlider->WidgetStyle.BackgroundImage.
	if (UMaterialInstanceDynamic* m = getDynamicMaterial(Color2DSlider->WidgetStyle.BackgroundImage)) {
		m->SetScalarParameterValue(TEXT("val"), val);
	}
	hsv.B = val;
	rgb = hsv.HSVToLinearRGB();
	Color2DSlider->SetSliderHandleColor(rgb);
}
void UColorPicker::OnColorValueChange(float x)
{
	FVector2D v = Color2DSlider->GetValue()-0.5;
	double angle = atan2(v.Y, v.X);
	double hue = (angle + PI)/(PI +PI);
	double len = v.Length();
	if (abs(len) > 0.5) {
		len = 0.5;
		double newX = 0.5 + len * cos(angle);
		double newY = 0.5 + len * sin(angle);
		Color2DSlider->SetValue(FVector2D(newX, newY));
	}
	double sat = len * 2.1;
	hsv = FLinearColor(hue * 360.,sat,1.);
	rgb = hsv.HSVToLinearRGB();
	Color2DSlider->SetSliderHandleColor(rgb);
	if (UMaterialInstanceDynamic* m = getDynamicMaterial(BrightnessSlider->WidgetStyle.NormalBarImage)) {
		m->SetScalarParameterValue(TEXT("hue"), hue);
		m->SetScalarParameterValue(TEXT("sat"), sat);
	}
	if (UMaterialInstanceDynamic* m = getDynamicMaterial(BrightnessSlider->WidgetStyle.HoveredBarImage)) {
		m->SetScalarParameterValue(TEXT("hue"), hue);
		m->SetScalarParameterValue(TEXT("sat"), sat);
	}
	

}
