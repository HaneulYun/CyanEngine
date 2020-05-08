#pragma once

class RenderTexture;

class TerrainData
{
public:
	std::wstring AlphamapTextureName{};
	RenderTexture* heightmapTexture{ nullptr };
	int heightmapHeight{ 0 };
	int heightmapWidth{ 0 };
	Vector3	size{ 0, 0, 0 };

private:
	BYTE *bytes;

public:
	TerrainData() = default;
	~TerrainData(void);

	void Load();
	
	float GetHeight(float x, float z);
	Vector3 GetHeightMapNormal(int x, int z);

	BYTE* GetHeightMapPixels() { return(bytes); }
	int GetHeightMapWidth() { return(heightmapWidth); }
	int GetHeightMapLength() { return(heightmapHeight); }
};
