#include "fypch.hpp"
#include "KickingState.h"

Frosty::KickingState::KickingState(std::string kickAnimName)
{
	Initiate(kickAnimName);
}

Frosty::KickingState::~KickingState()
{
}

void Frosty::KickingState::Initiate(std::string kickAnimName)
{
	animName = kickAnimName;
}

void Frosty::KickingState::OnInput()
{
}

void Frosty::KickingState::OnUpdate()
{
}

std::string* Frosty::KickingState::getName()
{
	return &animName;
}
