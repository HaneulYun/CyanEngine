#pragma once

class Image : public MonoBehavior<Image>
{
private:
	friend class GameObject;
	friend class MonoBehavior<Image>;
	Image() = default;
	Image(Image&) = default;

public:
	~Image() {}

	void Start()
	{
	}

	void Update()
	{
	}
};