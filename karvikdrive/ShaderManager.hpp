#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <raylib.h>

enum class ShaderType {
	Shadow,
	Terrain,
	Car,
	Quantize,
	Sky
};

enum class ShaderClass {
	Vertex,
	Fragment,
	SharedGLSL
};

class ShaderManager {
	inline static const std::string ShaderDirectory = "shd/";

	struct PairHash {
		size_t operator()(const std::pair<ShaderType, std::string>& p) const {
			return std::hash<int>()((int)p.first) ^ std::hash<std::string>()(p.second);
		}
	};

	std::unordered_map<ShaderType, Shader> _shaders;
	std::unordered_map<std::pair<ShaderType, std::string>, int, PairHash> _locationCache;
public:
	ShaderManager() {
		load();
	}

	~ShaderManager() {
		for (auto& [type, shader] : _shaders) {
			UnloadShader(shader);
		}
	}

	Shader& get(ShaderType type) {
		return _shaders.at(type);
	}

	const Shader& get(ShaderType type) const {
		return _shaders.at(type);
	}

	void reload() {
		for (auto& [type, shader] : _shaders) {
			UnloadShader(shader);
		}

		_locationCache.clear();
		load();
		std::cout << "reloaded shaders\n";
	}

	int loc(ShaderType type, const std::string& name) {
		auto key = std::make_pair(type, name);
		auto it = _locationCache.find(key);
		if (it != _locationCache.end()) return it->second;
		int l = GetShaderLocation(_shaders.at(type), name.c_str());
		_locationCache[key] = l;
		return l;
	}

	template<typename T>
	void set(ShaderType type, const std::string& name, const T* value, int uniformType) {
		SetShaderValue(_shaders.at(type), loc(type, name), value, uniformType);
	}

	void setMatrix(ShaderType type, const std::string& name, Matrix m) {
		SetShaderValueMatrix(_shaders.at(type), loc(type, name), m);
	}
private:
	std::string getShaderPath(const std::string& name, ShaderClass shaderClass) {
		return ShaderDirectory + (shaderClass == ShaderClass::Vertex ? "vert/" : shaderClass == ShaderClass::Fragment ? "frag/" : "") + name;
	}

	std::string loadShaderSource(const std::string& path) {
		std::ifstream file(path);

		if (!file.is_open()) {
			std::cerr << "failed to open shader file: " << path << std::endl;
			return "";
		}

		std::stringstream out;
		std::string line;
		while (std::getline(file, line)) {
			size_t p = line.find("#include \"");
			if (p != std::string::npos) {
				size_t start = p + 10;
				size_t end = line.find('"', start);
				out << loadShaderSource(getShaderPath(line.substr(start, end - start), ShaderClass::SharedGLSL)) << "\n";
			}
			else {
				out << line << "\n";
			}
		}

		return out.str();
	}

	void loadShader(ShaderType type, const std::optional<std::string>& vertexFile, const std::optional<std::string>& fragmentFile) {
		std::string vertexShader = "";
		std::string fragmentShader = "";

		std::cout << "ShaderManager: loading " << (vertexFile.has_value() ? vertexFile.value() : "<no vert>") << " " << (fragmentFile.has_value() ? fragmentFile.value() : "<no frag>") << std::endl;

		if (vertexFile.has_value()) vertexShader = loadShaderSource(getShaderPath(vertexFile.value(), ShaderClass::Vertex));
		if (fragmentFile.has_value()) fragmentShader = loadShaderSource(getShaderPath(fragmentFile.value(), ShaderClass::Fragment));
		
		const char* vsCode = vertexShader.empty() ? nullptr : vertexShader.c_str();
		const char* fsCode = fragmentShader.empty() ? nullptr : fragmentShader.c_str();

		_shaders[type] = LoadShaderFromMemory(vsCode, fsCode);
	}

	void load() {
		loadShader(ShaderType::Shadow, "shadow.vert", std::nullopt);
		loadShader(ShaderType::Terrain, "basic.vert", "terrain.frag");
		loadShader(ShaderType::Car, "basic.vert", "car.frag");
		loadShader(ShaderType::Quantize, std::nullopt, "quantize.frag");
		loadShader(ShaderType::Sky, std::nullopt, "sky.frag");
	}
};