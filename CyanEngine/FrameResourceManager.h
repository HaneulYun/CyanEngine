#pragma once

struct FrameResourceManager
{
	Scene* scene{ nullptr };

	std::vector<std::unique_ptr<FrameResource>> frameResources;
	static const int NumFrameResources{ NUM_FRAME_RESOURCES };
	FrameResource* currFrameResource{ nullptr };
	int currFrameResourceIndex{ 0 };

	void Update();
};