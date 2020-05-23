#pragma once

enum class RenderLayer : int
{
	Opaque = 0,
	SkinnedOpaque,
	Debug,
	Grass,
	BuildPreview,
	UI,
	Sky,
	Particle,
	Count
};