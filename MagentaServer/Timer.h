#pragma once
#include <queue>
#include <chrono>
#include <thread>
#include "RWLock.h"
#include "etc.h"

using namespace std;
using namespace chrono;

class Timer
{
public:
	
	HANDLE g_iocp;
	priority_queue<event_type> timer_queue;	// 포인터로 선언해서 new delete 하라
	thread timer_thread;
	RWLock timer_lock;

	Timer() {}
	~Timer() { timer_thread.join(); }

	void init_timer(HANDLE iocp)
	{
		g_iocp = iocp;

		timer_thread = thread(&Timer::do_timer, this);
	}

	void add_timer(int obj_id, ENUMOP op_type, int duration, int tg_id)
	{
		timer_lock.EnterWriteLock();
		event_type ev{ obj_id, op_type, high_resolution_clock::now() + milliseconds(duration), tg_id };
		timer_queue.push(ev);
		timer_lock.LeaveWriteLock();
	}

	void do_timer()
	{
		while (true) {
			this_thread::sleep_for(1ms);
			while (true) {
				timer_lock.EnterWriteLock();
				if (true == timer_queue.empty()) {
					timer_lock.LeaveWriteLock();
					break;
				}
				auto now_t = high_resolution_clock::now();
				event_type temp_ev = timer_queue.top();
				if (timer_queue.top().wakeup_time > high_resolution_clock::now()) {
					timer_lock.LeaveWriteLock();
					break;
				}
				event_type ev = timer_queue.top();
				timer_queue.pop();
				timer_lock.LeaveWriteLock();
				switch (ev.event_id) {
				case OP_RANDOM_MOVE:
				case OP_RUN:
					EXOVER* over = new EXOVER;
					over->op = ev.event_id;
					over->p_id = ev.target_id;
					PostQueuedCompletionStatus(g_iocp, 1, ev.obj_id, &over->over);
				}
			}
		}
	}
};