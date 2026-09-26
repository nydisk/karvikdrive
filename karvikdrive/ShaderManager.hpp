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
	Sky,
	Tree
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
	std::string getShaderPath(const std::string& name) {
		return ShaderDirectory + name;
	}
	void load() {
		_shaders[ShaderType::Shadow] = LoadShader(getShaderPath("shadow.vert").c_str(), getShaderPath("shadow.frag").c_str());
		_shaders[ShaderType::Tree] = LoadShader(getShaderPath("tree.vert").c_str(), getShaderPath("tree.frag").c_str());
		_shaders[ShaderType::Terrain] = LoadShader(getShaderPath("terrain.vert").c_str(), getShaderPath("terrain.frag").c_str());
		_shaders[ShaderType::Car] = LoadShader(getShaderPath("car.vert").c_str(), getShaderPath("car.frag").c_str());
		_shaders[ShaderType::Quantize] = LoadShader(nullptr, getShaderPath("quantize.frag").c_str());
		_shaders[ShaderType::Sky] = LoadShader(nullptr, getShaderPath("sky.frag").c_str());
	}
};