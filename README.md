# ue4-new-module-tool
This plug-in to Unreal Engine 4  extends the file menu with a New C++ Module button.

What does it do?

This tool aims to automises the creation of new C++ modules in Unreal Engine 4. Up to now, creating new modules was a tedious error prone process. 
This tool adds a new button to File > New C++ Module. You can specifiy a new module name and the tool will create the new module files, update the .uproject file, and regenerate your Visual Studio solution. 
The only thing you will have to do is update your .Target.cs files: in these files, simply add your module's name to the ExtraModulesNames property. The tool does not do this so it does not mess up any custom logic you may have written in the target build files.

Installation

Place the ModuleGeneration folder into your project's Plugins folder. Rebuild your Visual Studio solution.

