#include "Framework.h"
#include "Script_Instance.h"
#include "Script_Module.h"
#include <angelscript.h>
#include "Scene/Actor.h"

Script_Instance::Script_Instance()
    : scripting(nullptr)
    , script_obj(nullptr)
    , constructor_func(nullptr)
    , start_func(nullptr)
    , update_func(nullptr)
    , actor(nullptr)
    , script_module(nullptr)
    , script_path("")
    , class_name("")
    , constructor_decl("")
    , module_name("")
    , bInstantiated(false)
{
}

Script_Instance::~Script_Instance()
{
    if (script_obj)
    {
        script_obj->Release();
        script_obj = nullptr;
    }

    constructor_func = nullptr;
    start_func = nullptr;
    update_func = nullptr;
    scripting = nullptr;
    bInstantiated = false;
}

auto Script_Instance::Instantiate(const std::string & path, Actor * actor, Scripting * scripting) -> const bool
{
    if (!actor || !scripting)
    {
        LOG_ERROR("Invalid parameter");
        return false;
    }

    this->scripting = scripting;
    this->actor = actor;
    this->script_path = path;
    this->class_name = FileSystem::GetIntactFileNameFromPath(path);
    this->module_name = class_name + std::to_string(actor->GetID());
    this->constructor_decl = class_name + " @" + class_name + "(Actor @)";

    bInstantiated = CreateScriptObject();
    return bInstantiated;
}

void Script_Instance::ExecuteStart()
{
    if (!scripting)
    {
        LOG_ERROR("scripting is nullptr");
        return;
    }

    scripting->ExecuteCall(start_func, script_obj);
}

void Script_Instance::ExecuteUpdate()
{
    if (!scripting)
    {
        LOG_ERROR("scripting is nullptr");
        return;
    }

    scripting->ExecuteCall(update_func, script_obj);
}

auto Script_Instance::CreateScriptObject() -> const bool
{
    if (!scripting)
    {
        LOG_ERROR("Invalid parameter. scripting is nullptr");
        return false;
    }

    //Create module
    script_module = std::make_shared<Script_Module>(scripting, module_name);
    auto bResult = script_module->LoadScript(script_path);
    if (!bResult)
        return false;

    //Get type
    auto type_id = script_module->GetAngelScriptModule()->GetTypeIdByDecl(class_name.c_str());
    asITypeInfo* type = scripting->GetAngelScriptEngine()->GetTypeInfoById(type_id);
    if (!type)
    {
        LOG_ERROR("The type is incorrect");
        return false;
    }

    //Get functions in the script
    start_func = type->GetMethodByDecl("void Start()");
    update_func = type->GetMethodByDecl("void Update()");
    constructor_func = type->GetFactoryByDecl(constructor_decl.c_str());

    if (!constructor_func)
    {
        LOG_ERROR("Couldn't find the appropriate factory for the type '" + class_name + "'");
        return false;
    }

    auto ctx = scripting->RequestContext();
    {
        auto result = ctx->Prepare(constructor_func);
        if (result < 0)
            return false;

        result = ctx->SetArgObject(0, actor);
        if (result < 0)
            return false;

        result = ctx->Execute();
        if (result < 0)
            return false;

        script_obj = *static_cast<asIScriptObject**>(ctx->GetAddressOfReturnValue());

        script_obj->AddRef();
    }
    scripting->ReturnContext(ctx);

    return true;
}
