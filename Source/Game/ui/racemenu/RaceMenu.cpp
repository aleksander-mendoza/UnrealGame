// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceMenu.h"
#include "RaceMenuEntryObject.h"

void URaceMenu::AddMorphTargetEntry(AGameCharacter* player, FName morphTarget, FString name, float minValue, float maxValue, float defaultValue) {

	URaceMenuEntryObjectMorphTarget* entry = NewObject<URaceMenuEntryObjectMorphTarget>();
	entry->InitMorphTarget(FText::FromString(name), player, this, morphTarget, minValue, maxValue, defaultValue);
	ControlListView->AddItem(entry);
}
void URaceMenu::NativeConstruct()
{
	ColorPicker->OnColorValueChanged.AddUniqueDynamic(this, &URaceMenu::OnColorChanged);
	ColorPicker->SetVisibility(ESlateVisibility::Collapsed);
}
void URaceMenu::OnColorChanged(FLinearColor rgb)
{
	if (ColorPickerSubscriber)ColorPickerSubscriber->SetColorValue(rgb);
}

void URaceMenu::setSliderValues(AGameCharacter* character) {
	
	URaceMenuEntryObjectHairdoPicker* hairdoStyleEntry = NewObject<URaceMenuEntryObjectHairdoPicker>();
	hairdoStyleEntry->InitHairdoPicker(character, this);
	ControlListView->AddItem(hairdoStyleEntry);


	URaceMenuEntryObjectHairColorPicker* hairColorEntry = NewObject<URaceMenuEntryObjectHairColorPicker>();
	hairColorEntry->InitHairColorPicker(character, this);
	ControlListView->AddItem(hairColorEntry);

	AddMorphTargetEntry(character, TEXT("pJCMPelvisFwd_25"), TEXT("Pelvis Forward"), -0.25, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_1"), TEXT("Breast Preset 1"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_02"), TEXT("Breast Preset 02"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_03"), TEXT("Breast Preset 03"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_04"), TEXT("Breast Preset 04"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_05"), TEXT("Breast Preset 05"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_06"), TEXT("Breast Preset 06"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_07"), TEXT("Breast Preset 07"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_08"), TEXT("Breast Preset 08"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_09"), TEXT("Breast Preset 09"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_10"), TEXT("Breast Preset 10"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_11"), TEXT("Breast Preset 11"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Breast_Preset_12"), TEXT("Breast Preset 12"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("CTRLBreastsImplants"), TEXT("Breast Implants"), -1.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("CTRLBreastsNatural"), TEXT("Breasts Natural"), -0.25, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMBreastsCleavage"), TEXT("Cleavage"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMBreastsDiameter"), TEXT("Breast Diameter"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMBreastsDownwardSlope"), TEXT("Breast Downward"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMBreastsGone"), TEXT("Breast Gone"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMBreastsHeavy"), TEXT("Breast Heavy"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMBreastsPerkSide"), TEXT("Breast Perky"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMBreastsSmall"), TEXT("Breast Small"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMBreastsUnderCurve"), TEXT("Breast Under"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMBreastsUpwardSlope"), TEXT("Breast Upward"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("L3D_Nails1"), TEXT("Nails 1"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("L3D_Nails2"), TEXT("Nails 2"), -0.3499999940395355, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("L3D_Nails3"), TEXT("Nails 3"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("L3D_Nails4"), TEXT("Nails 4"), -0.4000000059604645, 1.149999976158142, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMNailsLength"), TEXT("Nails Length"), -0.30000001192092896, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMBodySize"), TEXT("Body Size"), -0.5, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMBodyTone"), TEXT("Body Tone"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMBodybuilderDetails"), TEXT("Bodybuilder Details"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMBodybuilderSize"), TEXT("Bodybuilder Size"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMEmaciated"), TEXT("Emaciated"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMFitnessDetails"), TEXT("Fitness Details"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMFitnessSize"), TEXT("Fitness Size"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMHeavy"), TEXT("Heavy"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMHeight"), TEXT("Height"), -1.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMPearFigure"), TEXT("Pear Figure"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMThin"), TEXT("Thin"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("FBMVoluptuous"), TEXT("Voluptuous"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPregnant"), TEXT("Pregnant"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_01"), TEXT("Thicc 01"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_02"), TEXT("Thicc 02"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_03"), TEXT("Thicc 03"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_04"), TEXT("Thicc 04"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_05"), TEXT("Thicc 05"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_06"), TEXT("Thicc 06"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_07"), TEXT("Thicc 07"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_08"), TEXT("Thicc 08"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_09"), TEXT("Thicc 09"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_10"), TEXT("Thicc 10"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_11"), TEXT("Thicc 11"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Thicc_12"), TEXT("Thicc 12"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMCollarboneDetail"), TEXT("Collarbone Detail"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMCostalAngleArched"), TEXT("Costal Angle Arched"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMCostalAnglePointed"), TEXT("Costal Angle Pointed"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMHipBoneSize"), TEXT("Hip BoneS ize"), -0.5, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLineaAlbaDepth"), TEXT("Linea Alba Depth"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLoveHandles"), TEXT("Love Handles"), -0.5, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMNavelDepth"), TEXT("Navel Depth"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMNavelHorizontal"), TEXT("Navel Horizontal"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMNavelOut"), TEXT("Navel Out"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMRectusOuterDetail"), TEXT("Rectus Outer Detail"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMRectusWidth"), TEXT("Rectus Width"), -0.5, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMRibcageSize"), TEXT("Ribcage Size"), -0.5, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMSternumDepth"), TEXT("Sternum Depth"), -0.5, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMStomachSoften"), TEXT("Stomach Soften"), -0.5, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("SCR_Belly_Fat"), TEXT("Belly Fat"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("SCR_Calf_Thickness"), TEXT("Calf Thickness"), -0.5, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Foot_Shapely"), TEXT("Foot Shape"), -1.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_01"), TEXT("Booty 01"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_02"), TEXT("Booty 02"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_03"), TEXT("Booty 03"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_04"), TEXT("Booty 04"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_05"), TEXT("Booty 05"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_06"), TEXT("Booty 06"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_07"), TEXT("Booty 07"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_08"), TEXT("Booty 08"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_09"), TEXT("Booty 09"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_10"), TEXT("Booty 10"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_11"), TEXT("Booty 11"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Booty_12"), TEXT("Booty 12"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("Aelis_Nipples"), TEXT("Aelis Nipples"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMAreolaeDepth"), TEXT("Areolae Depth"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMAreolaeDiameter"), TEXT("Areolae Diameter"), -0.25, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMAreolaePerk"), TEXT("Areolae Perk"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("BJ_Cheeks_-_Balloon_Left"), TEXT("Cheeks - Balloon Left"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("BJ_Cheeks_-_Balloon_Right"), TEXT("Cheeks - Balloon Right"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("BJ_Cheeks_-_Suck_Left"), TEXT("Cheeks - Suck Left"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("BJ_Cheeks_-_Suck_Right"), TEXT("Cheeks - Suck Right"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("BJ_Throat_-_Bulge_02"), TEXT("Throat - Bulge 02"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMG8FAnusOpen"), TEXT("Anus Open"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMAnusOpen"), TEXT("Anus Open 2"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMClitoralHoodDepth"), TEXT("Clitoral Hood Depth"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMClitoralHoodLeftRight"), TEXT("Clitoral Hood Tilt"), -1.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMClitoralHoodLength"), TEXT("Clitoral Hood Length"), -1.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMClitoralHoodRiseDown"), TEXT("Clitoral Hood Rise"), -1.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMClitoralHoodSize"), TEXT("Clitoral Hood Size"), -0.5, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensClose01"), TEXT("Vagina Close 01"), 0.0, 1.2000000476837158, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensClose02"), TEXT("Vagina Close 02"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensClose03"), TEXT("Vagina Close 03"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensOpen01"), TEXT("Vagina Open 01"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensOpen02"), TEXT("Vagina Open 02"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensOpen03"), TEXT("Vagina Open 03"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensPosition"), TEXT("Vagina Position"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensSize01"), TEXT("Vagina Size 01"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensSize02"), TEXT("Vagina Size 02"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMGensSize03"), TEXT("Vagina Size 03"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMajoraOpenL"), TEXT("Labia Majora Open (L)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMajoraOpenR"), TEXT("Labia Majora Open (R)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMajoraStyle01"), TEXT("Labia Majora Style 1"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMajoraStyle02"), TEXT("Labia Majora Style 2"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraEversion"), TEXT("Labia Minora Eversion"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraOffset01"), TEXT("Labia Minora Offset 1"), -1.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraOffset02"), TEXT("Labia Minora Offset 2"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraOffset03"), TEXT("Labia Minora Offset 3"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraOpenL"), TEXT("Labia Minora Open (L)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraOpenR"), TEXT("Labia Minora Open (R)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraPosition"), TEXT("Labia Minora Position"), 0.0, 1.2000000476837158, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraSize"), TEXT("Labia Minora Size" ), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraStyle01"), TEXT("Labia Minora Style 1"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraStyle02"), TEXT("Labia Minora Style 2"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraStyle02Eversion"), TEXT("Labia Minora Style 2 Eversion"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraStyle03"), TEXT("Labia Minora Style 3"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraStyle03Eversion"), TEXT("Labia Minora Style 3 Eversion"), 0.0, 1.5, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMLabiaMinoraStyle04"), TEXT("Labia Minora Style 4"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMajoraLowerLeft"), TEXT("Pull Labia Maj. Low (L)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMajoraLowerRight"), TEXT("Pull Labia Maj. Low (R)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMajoraMiddelLeft"), TEXT("Pull Labia Maj. Mid (L)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMajoraMiddelRight"), TEXT("Pull Labia Maj. Mid (R)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMajoraUpperLeft"), TEXT("Pull Labia Maj. Up (L)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMajoraUpperRight"), TEXT("Pull Labia Maj. Up (R)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMinoraMiddelLeft"), TEXT("Pull Labia Min. Mid (L)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMinoraMiddelRight"), TEXT("Pull Labia Min. Mid (R)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMinoraUpperLeft"), TEXT("Pull Labia Min. Up (L)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("PBMPullLabiaMinoraUpperRight"), TEXT("Pull Labia Min. Up (R)"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("SpaceBetweenLegs"), TEXT("Space Between Legs"), 0.0, 1.0, 0.0);
	AddMorphTargetEntry(character, TEXT("ThinghsThicc"), TEXT("Thick Thighs"), 0.0, 1.0, 0.0);
	
}
