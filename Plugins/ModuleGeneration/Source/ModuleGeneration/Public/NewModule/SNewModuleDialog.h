// Copyright (c) Dominik Peacock 2020

#pragma once

#include "NewModuleEvents.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

#include "ModuleDescriptor.h"

class SWizard;

class SNewModuleDialog : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SNewModuleDialog)
	{}

	/** A reference to the parent window */
	SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)

	SLATE_EVENT(FOnRequestNewModule, OnClickFinished)
	
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	void PopulateAvailableModules();
	FString FindSuitableModulePath() const;
	
	bool CanFinishButtonBeClicked() const;
	EVisibility GetErrorLabelVisibility() const;
	FText GetErrorLabelText() const;
	bool IsModuleNameAvailable() const;
	bool DoesModuleDirectoryAlreadyExist() const;

	// Button events
	void OnClickCancel();
	void OnClickFinish();

	// Edit box: Module name
	FText OnGetModuleName() const;
	void OnModuleNameChanged(const FText& NewText);
	void OnModuleNameCommitted(const FText& NewText, ETextCommit::Type CommitType);

	// Combo box: Module type
	void OnSelectedHostTypeChanged(TSharedPtr<EHostType::Type> Value, ESelectInfo::Type SelectInfo); 
	TSharedRef<SWidget> MakeWidgetForSelectedHostType(TSharedPtr<EHostType::Type> ForHostType) const;
	FText GetSelectedHostTypeText() const;

	// Combo box: Loading phase
	void OnSelectedLoadingPhaseChanged(TSharedPtr<ELoadingPhase::Type> Value, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> MakeWidgetForSelectedLoadingPhase(TSharedPtr<ELoadingPhase::Type> ForLoadingPhase) const;
	FText GetSelectedLoadingPhaseText() const;

	// Edit box: Path
	FText GetOutputPath() const;
	void OnOutputPathChanged(const FText& NewText);
	FReply HandleChooseFolderButtonClicked();
	
	void UpdateInput();
	void CloseContainingWindow();

private:

	// Widget references
	TSharedPtr<SWizard> MainWizard;
	TSharedPtr<SEditableTextBox> ModuleNameEditBox;
	TSharedPtr<SComboBox<TSharedPtr<EHostType::Type>>> SelectableHostTypesComboBox;
	TSharedPtr<SComboBox<TSharedPtr<ELoadingPhase::Type>>> SelectableLoadingPhasesComboBox;

	// Other data
	TArray<TSharedPtr<FModuleContextInfo>> AvailableModules;
	
	// Input data
	FString OutputDirectory;
	FString NewModuleName;
	EHostType::Type SelectedHostType;
	ELoadingPhase::Type SelectedLoadingPhase;

	// Style constants
	const float EditableTextHeight = 26.f;

	// Called by OnClickFinish when finish button is clicked
	FOnRequestNewModule OnClickFinished;
};