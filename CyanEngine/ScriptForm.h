#pragma once
#include "framework.h"

class ScriptForm : public MonoBehavior<ScriptForm>
{
private /*�� ������ private ������ �����ϼ���.*/ :

public  /*�� ������ public ������ �����ϼ���.*/ :

private:
	friend class GameObject;
	friend class MonoBehavior<ScriptForm>;
	ScriptForm() = default;
	ScriptForm(ScriptForm&) = default;

public:
	~ScriptForm() {}

	void Start(/*�ʱ�ȭ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

	void Update(/*������Ʈ �ڵ带 �ۼ��ϼ���.*/)
	{
	}

// �ʿ��� ��� �Լ��� ���� �� ���� �ϼŵ� �˴ϴ�.
};