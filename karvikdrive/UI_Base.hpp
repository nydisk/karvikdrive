#pragma once

struct UI_Base {
	virtual ~UI_Base() = default;
	virtual void imguiDraw() = 0;
};