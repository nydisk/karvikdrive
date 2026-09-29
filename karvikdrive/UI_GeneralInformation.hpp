#pragma once
#include "UI_Base.hpp"

class Car;
class UI_GeneralInformation : public UI_Base {
	const Car& _car;
public:
	UI_GeneralInformation(const Car& car);

	void imguiDraw() override;
};