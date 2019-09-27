#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

/// Very careful when changing solution settings on where the path starts from

// Window/Editor settings (might be moved to a file, settings.txt or something)
#define EDITOR_EXPLORER_WIDTH 300
#define EDITOR_INSPECTOR_WIDTH 300
#define EDITOR_ASSETS_HEIGHT 250
#define EDITOR_MAIN_MENU_BAR_HEIGHT 19

// Folder roots
#define PROJECT_FOLDER_ROOT "../../../"
#define PROJECT_VENDOR_FOLDER_ROOT PROJECT_FOLDER_ROOT "vendor/"
#define PROJECT_LUNAFILES_FOLDER_ROOT PROJECT_VENDOR_FOLDER_ROOT "LunaFiles/"

#define FY_PROJECT_FOLDER_ROOT "../../../Frosty/"
#define FY_ASSETS_FOLDER_ROOT FY_PROJECT_FOLDER_ROOT "assets/"
#define FY_SHADERS_FOLDER_ROOT FY_ASSETS_FOLDER_ROOT "shaders/"

// Shaders
#define FY_SHADER_TESTSHAPE_VERTEX_SHADER FY_SHADERS_FOLDER_ROOT "TestShapeVertex.glsl"
#define FY_SHADER_TESTSHAPE_FRAGMENT_SHADER FY_SHADERS_FOLDER_ROOT "TestShapeFragment.glsl"
#define FY_SHADER_TESTLIGHT_VERTEX_SHADER FY_SHADERS_FOLDER_ROOT "TestLightVS.glsl"
#define FY_SHADER_TESTLIGHT_FRAGMENT_SHADER FY_SHADERS_FOLDER_ROOT "TestLightFS.glsl"

// Luna files
#define FY_TEST_LOAD_MODEL PROJECT_LUNAFILES_FOLDER_ROOT "fox_character_attack.lu"
#define FY_TEST1_LOAD_MODEL PROJECT_LUNAFILES_FOLDER_ROOT "clock.lu"
#define FY_TEST2_LOAD_MODEL PROJECT_LUNAFILES_FOLDER_ROOT "table.lu"



#endif // !DEFINITIONS_HPP
