#pragma once

// �ӽ� �޽���
#define ADD 0x0041
#define SUB 'B'
#define EQUAL 'E'

struct Message
{
	char msgId;
	int lParam;
	int mParam;
	int rParam;
};