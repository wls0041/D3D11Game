#pragma once
#include "ISubsystem.h"

class Scripting final : public ISubsystem
{
public:
    Scripting(class Context* context);
    ~Scripting();

    const bool Initialize() override;

    auto GetAngelScriptEngine() const -> class asIScriptEngine*;

    auto RequestContext() -> class asIScriptContext*;
    void ReturnContext(class asIScriptContext* ctx);
    void ClearContext();

    auto ExecuteCall
    (
        class asIScriptFunction* script_func,
        class asIScriptObject* script_obj
    ) -> const bool;

    void DiscardModule(const std::string& module_name);

private:
    void LogExceptionInfo(class asIScriptContext* ctx);
    void Message_CallBack(const struct asSMessageInfo& msg);

private:
    class asIScriptEngine* script_engine;
    std::vector<class asIScriptContext*> ctxs;
};