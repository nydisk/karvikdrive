#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <filesystem>
#include <map>
#include <raylib.h>
#include "ShaderManager.hpp"

namespace fs = std::filesystem;

struct TreeRenderInfo {
	Mesh mesh;
	Material mat;
	float rh;
};

class TreeRenderer {
	inline static const std::string TreeDirectory = "assets/tree";
	std::map<std::string, TreeRenderInfo> _trees{};
	ShaderManager& _shaders;
public:
	TreeRenderer(ShaderManager& shaders) : _shaders(shaders) {
		loadTrees();
	}

	void render(const std::vector<Vector3>& trees, const std::vector<Vector3>& woods, Shader* overrideShader) {
		auto& info = _trees.at("pine");

		// exactly like the example
		//info.mat.shader = _shaders.get(ShaderType::Tree);
		//info.mat.shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(info.mat.shader, "mvp");
		//info.mat.shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(info.mat.shader, "matModel");

		Matrix transforms[10];
		for (int i = 0; i < 10; i++)
			transforms[i] = MatrixTranslate(i * 5.0f, 0.0f, 0.0f);

		DrawMeshInstanced(info.mesh, info.mat, transforms, 10);
	}
private:
	void loadTrees() {
		std::cout << "treerenderer: loading tree types\n";
		for (const auto& file : fs::directory_iterator(TreeDirectory)) {
			if (file.path().extension() != ".png") continue;
			std::string fname = file.path().stem().string(); 
			size_t us = fname.find_first_of('_');
			std::string treeName = fname.substr(0, us);
			float realHeight = (float)std::stoi(fname.substr(us + 1));
			
			std::cout << "treerenderer: found tree '" << treeName << "' with specified height " << realHeight << '\n';
			
			std::cout << " * loading texture\n";
			Texture2D tex = LoadTexture(file.path().string().c_str());
			
			TreeRenderInfo info{};
			info.rh = realHeight;
			info.mat = LoadMaterialDefault();

			float aspect = (float)tex.height / tex.width;
			//info.mesh = GenMeshPlane(realHeight / aspect, realHeight, 1, 1);
			info.mesh = GenMeshCube(2.0f, 2.0f, 2.0f);
			info.mat.maps[MATERIAL_MAP_DIFFUSE].texture = tex;

			_trees[treeName] = std::move(info);
			std::cout << " * done :]\n";
		}
		std::cout << "treerenderer: loaded " << _trees.size() << " tree type(s)\n";
	}
};