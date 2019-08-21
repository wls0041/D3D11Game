#pragma once
#include "Core/D3D11/ShaderStage/IShaderStage.h"

class Shader final
{
public:
	template <typename T>
	static constexpr ShaderScope DeduceShaderScope();

public:
	Shader(class Context *context);
	~Shader();

	template <typename T> auto GetShader() -> std::shared_ptr<T>;

	auto GetShaderID() const -> const uint& { return shader_id; }
	auto GetInputLayout() -> std::shared_ptr<InputLayout> { return input_layout; }

	template <typename T>
	void AddShader
	(
		const std::string& path,
		const std::string& entry_point = NOT_ASSIGNED_STR,
		const std::string& shader_model = NOT_ASSIGNED_STR //default parameter -> ��ȣ�� ����. overloading ���� �� ��
	);

	void AddDefine
	(
		const std::string& define,
		const std::string& value = "1"
	);


private:
	class Context *context;
	uint shader_id;

	std::shared_ptr<InputLayout> input_layout;
	std::map<ShaderScope, std::shared_ptr<class IShaderStage>> shaders;
	std::map<std::string, std::string> defines;
};

template<typename T>
inline auto Shader::GetShader() -> std::shared_ptr<T>
{
	auto shader_scope = DeduceShaderScope<T>();
	if (shaders.find(shader_scope) == shaders.end()) return nullptr;

	return std::static_pointer_cast<T>(shaders[shader_scope]);
}

template<typename T>
inline void Shader::AddShader(const std::string & path, const std::string & entry_point, const std::string & shader_model)
{ 
	//Create shader macro
	std::vector<D3D_SHADER_MACRO> shader_macros;

	for (auto& define : defines)
		shader_macros.emplace_back(D3D_SHADER_MACRO{ define.first.c_str(), define.second.c_str() });

	shader_macros.emplace_back(D3D_SHADER_MACRO{ nullptr, nullptr }); //�������� ������ �˷��ִ� null���ڿ� �߰�

	//Create Shader
	auto shader_scope = DeduceShaderScope<T>();
	auto pair_ib = shaders.insert(std::make_pair(shader_scope, std::make_shared<T>(context))); //�̹� ������ �ִ� ���� �ּ�, ������ ���� �ּ� ��ȯ. second�� �������� bool
	auto shader = std::static_pointer_cast<T>(pair_ib.first->second);

	if (pair_ib.second)
		shader->Create(path, entry_point, shader_model, shader_macros.data());
	else
	{
		shader->Clear();
		shader->Create(path, entry_point, shader_model, shader_macros.data());
	}

	if (shader_scope == ShaderScope::VS)
	{
		input_layout = std::make_shared<InputLayout>(context); //����Ʈ�����Ͱ� �˾Ƽ� ������ �ֱ� ������ ������ layout�� �ִ��� Ȯ�� �� ������ �ʿ䰡 ����
		input_layout->Create(shader->GetBlob());
	}
}
