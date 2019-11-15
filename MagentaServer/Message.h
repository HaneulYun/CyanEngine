#pragma once

// 임시 메시지
#define ADD 'A'
#define SUB 'B'
#define EQUAL 'E'

struct Message
{
	char msgId;
	int lParam;
	int mParam;
	int rParam;
};