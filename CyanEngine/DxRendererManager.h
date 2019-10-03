#pragma once

class DxRendererManager : public RendererManager,
	public Singleton<DxRendererManager>
{
private:
	DxRendererManager() {};

public:
	~DxRendererManager() override {};
};

