#include "Framework.h"
#include "InputLayout.h"

InputLayout::InputLayout(Context * context)
    : input_layout(nullptr)
{
    graphics = context->GetSubsystem<Graphics>();
}

InputLayout::~InputLayout()
{
    Clear();
}

void InputLayout::Create(D3D11_INPUT_ELEMENT_DESC * descs, const uint & count, ID3DBlob * blob)
{
    if (!descs || count == 0 || !blob)
        assert(false);

    auto hr = graphics->GetDevice()->CreateInputLayout
    (
        descs,
        count,
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        &input_layout
    );
    assert(SUCCEEDED(hr));
}

void InputLayout::Create(ID3DBlob * blob)
{
    if (!blob)
        assert(false);

    //Create shader reflector
    ID3D11ShaderReflection* reflector = nullptr;
    auto hr = D3DReflect
    (
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        __uuidof(ID3D11ShaderReflection), //IID_ID3D11ShaderReflection,
        reinterpret_cast<void**>(&reflector)
    );
    assert(SUCCEEDED(hr));

    //Get shader information
    D3D11_SHADER_DESC shader_desc;
    reflector->GetDesc(&shader_desc);

    //Get element information
    std::vector<D3D11_INPUT_ELEMENT_DESC> input_layout_descs;

    for (uint i = 0; i < shader_desc.InputParameters; i++)
    {
        D3D11_SIGNATURE_PARAMETER_DESC param_desc;
        reflector->GetInputParameterDesc(i, &param_desc);

        D3D11_INPUT_ELEMENT_DESC element_desc;
        element_desc.SemanticName           = param_desc.SemanticName;
        element_desc.SemanticIndex          = param_desc.SemanticIndex;
        element_desc.InputSlot              = 0;
        element_desc.AlignedByteOffset      = D3D11_APPEND_ALIGNED_ELEMENT;
        element_desc.InputSlotClass         = D3D11_INPUT_PER_VERTEX_DATA;
        element_desc.InstanceDataStepRate   = 0;

        //Check DXGI_FORMAT
        std::string temp = element_desc.SemanticName;
        if (temp == "POSITION")
            element_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        else if (param_desc.Mask == 1)
        {
            if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                element_desc.Format = DXGI_FORMAT_R32_UINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                element_desc.Format = DXGI_FORMAT_R32_SINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                element_desc.Format = DXGI_FORMAT_R32_FLOAT;
        }
        else if (param_desc.Mask <= 3)
        {
            if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                element_desc.Format = DXGI_FORMAT_R32G32_UINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                element_desc.Format = DXGI_FORMAT_R32G32_SINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                element_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
        }
        else if (param_desc.Mask <= 7)
        {
            if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                element_desc.Format = DXGI_FORMAT_R32G32B32_UINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                element_desc.Format = DXGI_FORMAT_R32G32B32_SINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                element_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        else if (param_desc.Mask <= 15)
        {
            if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                element_desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                element_desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                element_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }

        input_layout_descs.emplace_back(element_desc);
    }

    Create
    (
        input_layout_descs.data(),
        input_layout_descs.size(),
        blob
    );
}

void InputLayout::Clear()
{
    SAFE_RELEASE(input_layout);
}

void InputLayout::BindPipeline()
{
    graphics->GetDeviceContext()->IASetInputLayout(input_layout);
}