#pragma once
#include <Windows.h>

class RWLock
{
public:
	RWLock() {}
	~RWLock() {}

	RWLock(const RWLock& rhs) = delete;
	RWLock& operator=(const RWLock& rhs) = delete;

	void EnterWriteLock()
	{
		while (true)
		{
			while (mLockFlag & LF_WRITE_MASK)
				YieldProcessor();
			if ((InterlockedAdd(&mLockFlag, LF_WRITE_FLAG) & LF_WRITE_MASK) == LF_WRITE_FLAG)
			{
				while (mLockFlag & LF_READ_MASK)
					YieldProcessor();

				return;
			}
			InterlockedAdd(&mLockFlag, -LF_WRITE_FLAG);
		}
	}
	void LeaveWriteLock()
	{
		InterlockedAdd(&mLockFlag, -LF_WRITE_FLAG);
	}

	void EnterReadLock()
	{
		while (true)
		{
			while (mLockFlag & LF_WRITE_MASK)
				YieldProcessor();

			if ((InterlockedIncrement(&mLockFlag) & LF_WRITE_MASK) == 0)
				return;
			else
				InterlockedDecrement(&mLockFlag);
		}
	}
	void LeaveReadLock()
	{
		InterlockedDecrement(&mLockFlag);
	}

	long GetLockFlag() const { return mLockFlag; }

private:
	enum LockFlag
	{
		LF_WRITE_MASK = 0x7FF00000,
		LF_WRITE_FLAG = 0x00100000,
		LF_READ_MASK = 0x000FFFFF
	};
	volatile long mLockFlag;
};