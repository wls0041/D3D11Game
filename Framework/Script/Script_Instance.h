#pragma once

class Script_Instance final
{
public:
    Script_Instance();
    ~Script_Instance();

    auto Instantiate
    (
        const std::string& path,
        class Actor* actor,
        class Scripting* scripting
    ) -> const bool;

    auto GetScriptPath() const -> const std::string& { return script_path; }
    auto IsInstantiated() const -> const bool& { return bInstantiated; }

    void ExecuteStart();
    void ExecuteUpdate();

private:
    auto CreateScriptObject() -> const bool;

private:
    class Scripting* scripting;
    class asIScriptObject* script_obj;
    class asIScriptFunction* constructor_func;
    class asIScriptFunction* start_func;
    class asIScriptFunction* update_func;
    class Actor* actor;
    std::shared_ptr<class Script_Module> script_module;
    std::string script_path;
    std::string class_name;
    std::string constructor_decl;
    std::string module_name;
    bool bInstantiated;
};