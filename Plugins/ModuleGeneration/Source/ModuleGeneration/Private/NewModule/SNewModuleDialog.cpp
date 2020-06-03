// Copyright (c) Dominik Peacock 2020

#include "NewModule/SNewModuleDialog.h"
#include "Logging.h"

#include "DesktopPlatformModule.h"
#include "GameProjectGeneration/Public/GameProjectUtils.h"
#include "IDesktopPlatform.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Workflow/SWizard.h"

#define LOCTEXT_NAMESPACE "FModuleGenerationModule"

void SNewModuleDialog::Construct(const FArguments& InArgs)
{
	static TArray<TSharedPtr<EHostType::Type>> ALL_MODULE_TYPES = []()
	{
		TArray<TSharedPtr<EHostType::Type>> result;
		for(size_t i = EHostType::Runtime; i < EHostType::Max; ++i)
		{
			TSharedPtr<EHostType::Type> value = MakeShareable (new EHostType::Type);
			*value = static_cast<EHostType::Type>(i);
			result.Add(value);
		}
		return result;
	}();
	static TArray<TSharedPtr<ELoadingPhase::Type>> ALL_LOADING_PHASES = []()
	{
		TArray<TSharedPtr<ELoadingPhase::Type>> result;
		for (size_t i = ELoadingPhase::EarliestPossible; i < ELoadingPhase::Max; ++i)
		{
			TSharedPtr<ELoadingPhase::Type> value = MakeShareable(new ELoadingPhase::Type);
			*value = static_cast<ELoadingPhase::Type>(i);
			result.Add(value);
		}
		return result;
	}();
	static TSharedPtr<EHostType::Type> INITIALLY_SELECTED_HOST_TYPE =
		*ALL_MODULE_TYPES.FindByPredicate([](auto e) { return *e.Get() == EHostType::Runtime; });
	static TSharedPtr<ELoadingPhase::Type> INITIALL_SELECTED_LOADING_PHASE =
		*ALL_LOADING_PHASES.FindByPredicate([](auto e) { return *e.Get() == ELoadingPhase::Default; });
	
	PopulateAvailableModules();

	OnClickFinished = InArgs._OnClickFinished;

	OutputDirectory = FindSuitableModulePath();
	NewModuleName = "NewModule";
	SelectedHostType = EHostType::Runtime;
	SelectedLoadingPhase = ELoadingPhase::Default;
	
	ChildSlot
	[
		SNew(SBorder)
		.Padding(18)
		.BorderImage( FEditorStyle::GetBrush("Docking.Tab.ContentAreaBrush") )
		[
			SNew(SVerticalBox)

			+SVerticalBox::Slot()
			[
				SAssignNew(MainWizard, SWizard)
				.ShowPageList(false)

				.ButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
				.CancelButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
				.FinishButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
				.ButtonTextStyle(FEditorStyle::Get(), "LargeText")
				.ForegroundColor(FEditorStyle::Get().GetSlateColor("WhiteBrush"))

				.CanFinish(this, &SNewModuleDialog::CanFinishButtonBeClicked)
				.FinishButtonText(LOCTEXT("NewModule_FinishButtonText", "Create Module"))
				.FinishButtonToolTip (
					LOCTEXT("NewModule_FinishButtonToolTip", "Creates the code files to add your new module.")
					)
				.OnCanceled(this, &SNewModuleDialog::OnClickCancel)
				.OnFinished(this, &SNewModuleDialog::OnClickFinish)
				.InitialPageIndex(0)

				// Error message at bottom
				.PageFooter()
				[
					SNew(SBorder)
					.Visibility( this, &SNewModuleDialog::GetErrorLabelVisibility )
					.BorderImage( FEditorStyle::GetBrush("NewClassDialog.ErrorLabelBorder") )
					.Padding(FMargin(0, 5))
					.Content()
					[
						SNew(SHorizontalBox)

						+SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.Padding(2.f)
						.AutoWidth()
						[
							SNew(SImage)
							.Image(FEditorStyle::GetBrush("MessageLog.Warning"))
						]

						+SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text( this, &SNewModuleDialog::GetErrorLabelText )
							.TextStyle( FEditorStyle::Get(), "NewClassDialog.ErrorLabelFont" )
						]
					]
				]

				+SWizard::Page()
				[
					SNew(SVerticalBox)

					// Title
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0)
					[
						SNew(STextBlock)
						.TextStyle( FEditorStyle::Get(), "NewClassDialog.PageTitle" )
						.Text( LOCTEXT( "NewModule_Title", "New C++ Module" ) )
					]

					// Title spacer
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0, 2, 0, 8)
					[
						SNew(SSeparator)
					]

					// Page description and view options
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0, 10)
					[
						SNew(SHorizontalBox)

						+SHorizontalBox::Slot()
						.FillWidth(1.f)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(
								LOCTEXT("NewModule_PageDescription", "This will add a new C++ module to your game project and update the .uproject or .uplugin file, respectively.")
								)
						]
					]

					// First page: name module, module type, and loading phase
					+SVerticalBox::Slot()
					.Padding(2, 2)
					.AutoHeight()
					.HAlign(EHorizontalAlignment::HAlign_Fill)//
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Fill)//
						.FillWidth(1.f)//
						[
							SNew(SVerticalBox)

							+SVerticalBox::Slot()
							.AutoHeight()
							.VAlign(VAlign_Center)
							[
								// Gray details panel
								SNew(SBorder)
								.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryTop"))
								.BorderBackgroundColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f ))
								.Padding(FMargin(6.0f, 4.0f, 7.0f, 4.0f))
								[
									SNew(SVerticalBox)

									+SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0)
									[
										SNew(SGridPanel)
										.FillColumn(1, 1.0f)

										// Name label
										+SGridPanel::Slot(0, 0)
										.VAlign(VAlign_Center)
										.Padding(0, 0, 12, 0)
										[
											SNew(STextBlock)
											.TextStyle( FEditorStyle::Get(), "NewClassDialog.SelectedParentClassLabel" )
											.Text( LOCTEXT( "CreateModule_NameLabel", "Name" ) )
										]

										// Name edit box
										+SGridPanel::Slot(1, 0)
										.Padding(0.0f, 3.0f)
										.VAlign(VAlign_Center)
										[
											SNew(SBox)
											.HeightOverride(EditableTextHeight)
											[
												SAssignNew(ModuleNameEditBox, SEditableTextBox)
												.Text(this, &SNewModuleDialog::OnGetModuleName)
												.OnTextChanged(this, &SNewModuleDialog::OnModuleNameChanged)
												.OnTextCommitted(this, &SNewModuleDialog::OnModuleNameCommitted)
											]
										]
										// Host type and loading phase
										+SGridPanel::Slot(2, 0)
										.Padding(0.0f, 3.0f)
										.VAlign(VAlign_Center)
										[
											SNew(SBox)
											.HeightOverride(EditableTextHeight)
											[
												SNew(SHorizontalBox)
												// Host type
												+ SHorizontalBox::Slot()
												.AutoWidth()
												.Padding(6.0f, 0.0f, 0.0f, 0.0f)
												[
													SAssignNew(SelectableHostTypesComboBox, SComboBox<TSharedPtr<EHostType::Type>>)
													.Visibility(EVisibility::Visible)
													.ToolTipText(LOCTEXT("CreateModule_ModuleHostTypeTip", "Choose your module's loading context"))
													.OptionsSource(&ALL_MODULE_TYPES)
													.InitiallySelectedItem(INITIALLY_SELECTED_HOST_TYPE)
													.OnSelectionChanged(this, &SNewModuleDialog::OnSelectedHostTypeChanged)
													.OnGenerateWidget(this, &SNewModuleDialog::MakeWidgetForSelectedHostType)
													[
														SNew(STextBlock)
														.Text(this, &SNewModuleDialog::GetSelectedHostTypeText)
													]
												]

												// Loading phase
												+ SHorizontalBox::Slot()
													.AutoWidth()
													.Padding(6.0f, 0.0f, 0.0f, 0.0f)
													[
														SAssignNew(SelectableLoadingPhasesComboBox, SComboBox<TSharedPtr<ELoadingPhase::Type>>)
														.Visibility(EVisibility::Visible)
													.ToolTipText(LOCTEXT("CreateModule_ModuleLoadingPhaseTip", "Choose your module's loading phase"))
													.OptionsSource(&ALL_LOADING_PHASES)
													.InitiallySelectedItem(INITIALL_SELECTED_LOADING_PHASE)
													.OnSelectionChanged(this, &SNewModuleDialog::OnSelectedLoadingPhaseChanged)
													.OnGenerateWidget(this, &SNewModuleDialog::MakeWidgetForSelectedLoadingPhase)
													[
														SNew(STextBlock)
														.Text(this, &SNewModuleDialog::GetSelectedLoadingPhaseText)
													]
												]
											]
										]

										// Path label
										+SGridPanel::Slot(0, 1)
										.VAlign(VAlign_Center)
										.Padding(0, 0, 12, 0)
										[
											SNew(STextBlock)
											.TextStyle( FEditorStyle::Get(), "NewClassDialog.SelectedParentClassLabel" )
											.Text( LOCTEXT( "CreateModule_PathLabel", "Path"))
										]
										// Path edit box
										+SGridPanel::Slot(1, 1)
										.VAlign(VAlign_Center)
										[
											SNew(SVerticalBox)
											+SVerticalBox::Slot()
											.Padding(0)
											.AutoHeight()
											[
												SNew(SBox)
												.HeightOverride(EditableTextHeight)
												[
													SNew(SHorizontalBox)

													+SHorizontalBox::Slot()
													.FillWidth(1.0f)
													[
														SNew(SEditableTextBox)
														.Text(this, &SNewModuleDialog::GetOutputPath)
														.OnTextChanged(this, &SNewModuleDialog::OnOutputPathChanged)
													]
												]
											]
										]
										// Choose folder button
										+SGridPanel::Slot(2, 1)
										.Padding(0.0f, 3.0f)
										.VAlign(VAlign_Center)
										[
											SNew(SBox)
											.HeightOverride(EditableTextHeight)
											[
												SNew(SHorizontalBox)
												+SHorizontalBox::Slot()
												.FillWidth(1.f)
												.HAlign(HAlign_Fill)
												.Padding(6.0f, 0.0f, 0.0f, 0.0f)
												[
													SNew(SButton)
													.VAlign(VAlign_Center)
													.OnClicked(this, &SNewModuleDialog::HandleChooseFolderButtonClicked)
													.ToolTipText(LOCTEXT("CreateModule_ChooseFolderTooltip", "You can choose either the 'Source' folder in your project's root directory or of any plugin in the 'Plugins' folder."))
													.Text(LOCTEXT("BrowseButtonText", "Choose folder"))
												]
											]
										]
									]
								]
							]
						]
					]
				]
			]
		]
	];
}

void SNewModuleDialog::PopulateAvailableModules()
{
	TArray<FModuleContextInfo> CurrentModules = GameProjectUtils::GetCurrentProjectModules();
	check(CurrentModules.Num()); 

	TArray<FModuleContextInfo> CurrentPluginModules = GameProjectUtils::GetCurrentProjectPluginModules();

	CurrentModules.Append(CurrentPluginModules);

	AvailableModules.Reserve(CurrentModules.Num());
	for (const FModuleContextInfo& ModuleInfo : CurrentModules)
	{
		AvailableModules.Emplace(MakeShareable(new FModuleContextInfo(ModuleInfo)));
	}

	// TODO: discover engine module
}

FString SNewModuleDialog::FindSuitableModulePath() const
{
	// Should not happen: if it does, then PopulateAvailableModules was not called before
	check(AvailableModules.Num());
	
	const FString ProjectName = FApp::GetProjectName();

	// Find initially selected module based on simple fallback in this order..
	// Main project module, a  runtime module
	TSharedPtr<FModuleContextInfo> ProjectModule;
	TSharedPtr<FModuleContextInfo> RuntimeModule;
	for (const auto& AvailableModule : AvailableModules)
	{
		if (AvailableModule->ModuleName == ProjectName)
		{
			ProjectModule = AvailableModule;
			break;
		}

		if (AvailableModule->ModuleType == EHostType::Runtime)
		{
			RuntimeModule = AvailableModule;
		}
	}

	TSharedPtr<FModuleContextInfo> SelectedModule;
	if (ProjectModule.IsValid())
	{
		SelectedModule = ProjectModule;
	}
	if (RuntimeModule.IsValid())
	{
		SelectedModule = RuntimeModule;
	}

	if(SelectedModule != nullptr)
	{
		FString foundPath = ProjectModule->ModuleSourcePath;
		// TODO: Use platform portable character instead of slash (this may only work on windows)
		foundPath.RemoveFromEnd(ProjectModule->ModuleName + FString("/"));
		return foundPath;
	}
	return "";
}

bool SNewModuleDialog::CanFinishButtonBeClicked() const
{
	return IsModuleNameAvailable() && !DoesModuleDirectoryAlreadyExist();
}

EVisibility SNewModuleDialog::GetErrorLabelVisibility() const
{
	return GetErrorLabelText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

FText SNewModuleDialog::GetErrorLabelText() const
{
	if(!IsModuleNameAvailable())
	{
		return FText::Format(LOCTEXT("NewModule_ModuleUnavailable", "The module '{0}' is already in use."), FText::FromString(NewModuleName));
	}
	if(DoesModuleDirectoryAlreadyExist())
	{
		return FText::Format(LOCTEXT("NewModule_ModuleFolderAlreadyExists", "The target directory already contains a folder named '{0}'"), FText::FromString(NewModuleName));
	}
	return FText::GetEmpty();
}

bool SNewModuleDialog::IsModuleNameAvailable() const
{
	const bool bDoesModuleNameExist = !AvailableModules.ContainsByPredicate([this](auto e)
		{
			return e->ModuleName.Equals(NewModuleName);
		});
	
	return bDoesModuleNameExist;
}

bool SNewModuleDialog::DoesModuleDirectoryAlreadyExist() const
{
	const bool bDoesModuleFolderExist = IFileManager::Get().DirectoryExists(*FPaths::Combine(OutputDirectory, NewModuleName));
	return bDoesModuleFolderExist;
}

void SNewModuleDialog::OnClickCancel()
{
	CloseContainingWindow();
}

void SNewModuleDialog::OnClickFinish()
{
	OnClickFinished.ExecuteIfBound(
		OutputDirectory, 
		FModuleDescriptor(FName(*NewModuleName), SelectedHostType, SelectedLoadingPhase),
		FOnCreateNewModuleError::CreateLambda([](auto Error)
		{
			const FText ErrorMessageUnformatted =
				LOCTEXT("NewModule_Error_Message", "Failed to create new module.\n\n\nReason: {0}\n\nSome files may already have been created.");
			const FText ErrorMessage = FText::Format(FTextFormat(ErrorMessageUnformatted), FText::FromString(Error));
			const FText ErrorTitle = 
				LOCTEXT("NewModule_Error_Title", "Error creating new C++ Module");
			FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage, &ErrorTitle);
		}
	));
	
	CloseContainingWindow();
}

FText SNewModuleDialog::OnGetModuleName() const
{
	return FText::FromString(NewModuleName);
}

void SNewModuleDialog::OnModuleNameChanged(const FText& NewText)
{
	NewModuleName = NewText.ToString();
	UpdateInput();
}

void SNewModuleDialog::OnModuleNameCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		if (CanFinishButtonBeClicked())
		{
			OnClickFinish();
		}
	}
}

void SNewModuleDialog::OnSelectedHostTypeChanged(TSharedPtr<EHostType::Type> Value, ESelectInfo::Type SelectInfo)
{
	SelectedHostType = *Value.Get();
	UpdateInput();
}

TSharedRef<SWidget> SNewModuleDialog::MakeWidgetForSelectedHostType(TSharedPtr<EHostType::Type> ForHostType) const
{
	const FText text = FText::Format(LOCTEXT("CreateModule_SelectHostTypeComboText", "{0}"), FText::FromString(EHostType::ToString(*ForHostType.Get())));
	return SNew(STextBlock)
		.Text(text);
}

FText SNewModuleDialog::GetSelectedHostTypeText() const
{
	return FText::Format(LOCTEXT("CreateModule_SelectedHostTypeComboText", "{0}"), FText::FromString(EHostType::ToString(SelectedHostType)));
}

void SNewModuleDialog::OnSelectedLoadingPhaseChanged(TSharedPtr<ELoadingPhase::Type> Value, ESelectInfo::Type SelectInfo)
{
	SelectedLoadingPhase = *Value.Get();
	UpdateInput();
}

TSharedRef<SWidget> SNewModuleDialog::MakeWidgetForSelectedLoadingPhase(TSharedPtr<ELoadingPhase::Type> ForLoadingPhase) const
{
	const FText text = FText::Format(LOCTEXT("CreateModule_SelectLoadingPhaseComboText", "{0}"), FText::FromString(ELoadingPhase::ToString(*ForLoadingPhase.Get())));
	return SNew(STextBlock)
		.Text(text);
}

FText SNewModuleDialog::GetSelectedLoadingPhaseText() const
{
	return FText::Format(LOCTEXT("CreateModule_SelectedLoadingPhaseComboText", "{0}"), FText::FromString(ELoadingPhase::ToString(SelectedLoadingPhase)));
}

FText SNewModuleDialog::GetOutputPath() const
{
	return FText::FromString(OutputDirectory);
}

void SNewModuleDialog::OnOutputPathChanged(const FText& NewText)
{
	OutputDirectory = NewText.ToString();
	UpdateInput();
}

FReply SNewModuleDialog::HandleChooseFolderButtonClicked()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
		void* ParentWindowWindowHandle = (ParentWindow.IsValid()) ? ParentWindow->GetNativeWindow()->GetOSWindowHandle() : nullptr;

		FString FolderName;
		const FString Title = LOCTEXT("NewClassBrowseTitle", "Choose a source location").ToString();
		const bool bFolderSelected = DesktopPlatform->OpenDirectoryDialog(
			ParentWindowWindowHandle,
			Title,
			OutputDirectory,
			FolderName
		);

		if (bFolderSelected)
		{
			if (!FolderName.EndsWith(TEXT("/")))
			{
				FolderName += TEXT("/");
			}

			OutputDirectory = FolderName;
			UpdateInput();
		}
	}

	return FReply::Handled();
}

void SNewModuleDialog::UpdateInput()
{

}

void SNewModuleDialog::CloseContainingWindow()
{
	TSharedPtr<SWindow> ContainingWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());

	if (ContainingWindow.IsValid())
	{
		ContainingWindow->RequestDestroyWindow();
	}
}

#undef LOCTEXT_NAMESPACE