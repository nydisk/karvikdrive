#pragma once
#include <raylib.h>
#include <raymath.h>
#include "rlgl.h"
#include "ShaderManager.hpp"

class ShadowRenderer {
	ShaderManager& _shaders;
	unsigned int _fbo = 0;
	unsigned int _depth = 0;
public:
	static constexpr int ShadowMapSize = 4096;

	explicit ShadowRenderer(ShaderManager& shaders) : _shaders(shaders) {
		initFBO();
	}

	~ShadowRenderer() {
		if (_fbo != 0) rlUnloadFramebuffer(_fbo);
		if (_depth != 0) rlUnloadTexture(_depth);
	}

	void beginPass(Matrix lightSpaceMatrix) {
		_shaders.setMatrix(ShaderType::Shadow, "lightSpaceMatrix", lightSpaceMatrix);

		rlEnableFramebuffer(_fbo);
		rlClearScreenBuffers();
		rlViewport(0, 0, ShadowMapSize, ShadowMapSize);
		rlEnableDepthTest();
		rlDisableColorBlend();
	}

	void endPass() {
		rlEnableColorBlend();
		rlDisableFramebuffer();
		rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());
	}

	void bindToShader(ShaderType type, ShaderManager& shaders) const {
		int loc = shaders.loc(type, "shadowMap");
		rlActiveTextureSlot(1);
		rlEnableTexture(_depth);
		SetShaderValueTexture(shaders.get(type), loc, { _depth, ShadowMapSize, ShadowMapSize, 1, 1 });
	}

	unsigned int depthTexture() const {
		return _depth;
	}

	Shader& shader() {
		return _shaders.get(ShaderType::Shadow);
	}
private:
	void initFBO() {
		_fbo = rlLoadFramebuffer();
		_depth = rlLoadTextureDepth(ShadowMapSize, ShadowMapSize, false);
		rlFramebufferAttach(_fbo, _depth, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);
		if (!rlFramebufferComplete(_fbo))
			std::cout << "incomplete fbo\n";
		rlEnableFramebuffer(0);
	}
};