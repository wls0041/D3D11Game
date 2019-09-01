#include "Framework.h"
#include "Scripting.h"
#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.cpp>
#include "Script/Script_Interface.h"

Scripting::Scripting(Context * context)
    : ISubsystem(context)
    , script_engine(nullptr)
{
}

Scripting::~Scripting()
{
    ClearContext();

    if (script_engine)
    {
        script_engine->ShutDownAndRelease();
        script_engine = nullptr;
    }
}

const bool Scripting::Initialize()
{
    script_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    if (!script_engine)
    {
        LOG_ERROR("Failed to crate AngelScript engine");
        return false;
    }

    RegisterStdString(script_engine);

    auto script_interface = std::make_shared<Script_Interface>();
    script_interface->Register(context, script_engine);

    script_engine->SetMessageCallback
    (
        asMETHOD(Scripting, Message_CallBack),
        this,
        asCALL_THISCALL
    );

    script_engine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, true);

    return true;
}

auto Scripting::GetAngelScriptEngine() const -> class asIScriptEngine *
{
    return script_engine;
}

//asIScriptContext는 함수를 호출할때 사용됨
//overhead를 막기 위해 pooling 해야하나고 메뉴얼에 나옴
auto Scripting::RequestContext() -> class asIScriptContext *
{
    asIScriptContext* ctx = nullptr;
    if (!ctxs.empty())
    {
        ctx = *ctxs.rbegin();
        ctxs.pop_back();
    }
    else
        ctx = script_engine->CreateContext();

    return ctx;
}

void Scripting::ReturnContext(asIScriptContext * ctx)
{
    if (!ctx)
    {
        LOG_ERROR("ctx is nullptr");
        return;
    }

    ctxs.emplace_back(ctx);
    ctx->Unprepare();
}

void Scripting::ClearContext()
{
    for (auto& ctx : ctxs)
        ctx->Release();

    ctxs.clear();
    ctxs.shrink_to_fit();
}

auto Scripting::ExecuteCall(asIScriptFunction * script_func, asIScriptObject * script_obj) -> const bool
{
    auto ctx = RequestContext();
    ctx->Prepare(script_func);
    ctx->SetObject(script_obj);

    auto result = ctx->Execute();
    if (result == asEXECUTION_EXCEPTION)
    {
        LogExceptionInfo(ctx);
        ReturnContext(ctx);
        return false;
    }

    ReturnContext(ctx);
    return true;
}

void Scripting::DiscardModule(const std::string & module_name)
{
    script_engine->DiscardModule(module_name.c_str());
}

void Scripting::LogExceptionInfo(asIScriptContext * ctx)
{
    auto exception_desc = ctx->GetExceptionString();
    auto function       = ctx->GetExceptionFunction();
    auto function_decl  = function->GetDeclaration();
    auto module_name    = function->GetModuleName();
    auto script_path    = function->GetScriptSectionName();
    auto script_file    = FileSystem::GetFileNameFromPath(script_path);
    auto exception_line = std::to_string(ctx->GetExceptionLineNumber());

    LOG_ERROR_F
    (
        "%s, at line %s, in function %s, in script %s",
        exception_desc,
        exception_line,
        function_decl,
        script_file
    );
}

void Scripting::Message_CallBack(const asSMessageInfo & msg)
{
    auto file_name = FileSystem::GetFileNameFromPath(msg.section);
    auto message = msg.message;

    std::string final_message;
    if (file_name != "")
        final_message = file_name + " " + message;
    else
        final_message = message;

    LOG_ERROR(final_message);
}
