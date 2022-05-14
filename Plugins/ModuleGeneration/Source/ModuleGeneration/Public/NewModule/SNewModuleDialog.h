// Copyright Dominik Peacock. All rights reserved.

#pragma once

#include "NewModuleEvents.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

#include "ModuleDescriptor.h"

class SWizard;

class SNewModuleDialog : public SCompoundWidget
{
public:

	DECLARE_DELEGATE_RetVal_TwoParams(UE::ModuleGeneration::FOperationResult, FOnRequestNewModule, const FString& /*OutputDirectory*/, const FModuleDescriptor& /*ClassPath*/)
	
	SLATE_BEGIN_ARGS(SNewModuleDialog)
	{}
		/** A reference to the parent window */
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)
		SLATE_EVENT(FOnRequestNewModule, OnClickFinished)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	// Widget references
	TSharedPtr<SWizard> MainWizard;
	TSharedPtr<SEditableTextBox> ModuleNameEditBox;
	TSharedPtr<SComboBox<TSharedPtr<EHostType::Type>>> SelectableHostTypesComboBox;
	TSharedPtr<SComboBox<TSharedPtr<ELoadingPhase::Type>>> SelectableLoadingPhasesComboBox;

	// Data sources
	TArray<TSharedPtr<FModuleContextInfo>> AvailableModules;
	TArray<TSharedPtr<EHostType::Type>> ModuleTypeOptions;
	TArray<TSharedPtr<ELoadingPhase::Type>> LoadingPhaseOptions;
	
	// Input data
	FString OutputDirectory;
	FString NewModuleName = TEXT("NewModule");
	EHostType::Type SelectedHostType = EHostType::Runtime;
	ELoadingPhase::Type SelectedLoadingPhase = ELoadingPhase::Default;

	// Called by OnClickFinish when finish button is clicked
	FOnRequestNewModule OnClickFinished;

	void PopulateAvailableModules();
	void PopulateModuleTypes();
	void PopulateLoadingPhases();

	TSharedRef<SWidget> CreateMainPage();
	TSharedRef<SWidget> CreateModuleDetailsPanel();
	TSharedRef<SWidget> CreateFooter();

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
};