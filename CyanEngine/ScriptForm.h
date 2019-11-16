#pragma once
#include "framework.h"

class ScriptForm : public Component
{
private:
	// �� ������ private ������ �����ϼ���.

public:
	// �� ������ public ������ �����ϼ���.

private:
	friend class GameObject;
	ScriptForm() = default;
	ScriptForm(ScriptForm&) = default;

public:
	~ScriptForm() {}
	virtual Component* Duplicate() { return new ScriptForm; }
	virtual Component* Duplicate(Component* component) { return new ScriptForm(*(ScriptForm*)component); }

	void Start()
	{
		// �ʱ�ȭ �ڵ带 �ۼ��ϼ���.
	}

	void Update()
	{
		// ������Ʈ �ڵ带 �ۼ��ϼ���.
	}

	// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};

