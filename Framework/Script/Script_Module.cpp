#include "Framework.h"
#include "Script_Module.h"
#include <scriptbuilder/scriptbuilder.cpp>

Script_Module::Script_Module(Scripting * scripting, const std::string & module_name)
    : scripting(scripting)
    , module_name(module_name)
{
}

Script_Module::~Script_Module()
{
    if (scripting)
        scripting->DiscardModule(module_name);
}

auto Script_Module::LoadScript(const std::string & path) -> const bool
{
    if (!scripting)
    {
        LOG_ERROR("Scripting is nullptr");
        return false;
    }

    builder = std::make_unique<CScriptBuilder>();
    auto result = builder->StartNewModule
    (
        scripting->GetAngelScriptEngine(),
        module_name.c_str()
    );

    if (result < 0)
    {
        LOG_ERROR("Failed to start new module");
        return false;
    }

    result = builder->AddSectionFromFile(path.c_str());

    if (result < 0)
    {
        LOG_ERROR_F("Failed to load script \"%s\"", path);
        return false;
    }

    result = builder->BuildModule();

    if (result < 0)
    {
        LOG_ERROR_F("Failed to compile script \"%s\"", FileSystem::GetFileNameFromPath(path));
        return false;
    }

    return true;
}

auto Script_Module::GetAngelScriptModule() -> class asIScriptModule *
{
    if (!builder)
    {
        LOG_ERROR("Builder is nullptr");
        return nullptr;
    }

    return builder->GetModule();
}
