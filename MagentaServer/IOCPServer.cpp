#include "IOCPServer.h"

IOCPServer::IOCPServer()
{
	ifstream in{ "colliders.txt" };
	istream_iterator<Point> c_i(in);
	while (c_i != istream_iterator<Point>())
	{
		collidePoints.emplace_back(*c_i);
		*c_i++;
	}

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	cout << "NPC Initialize start.\n";
	init_npc();
	cout << "NPC Initialize finished.\n";

	l_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN s_address;
	memset(&s_address, 0, sizeof(s_address));
	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(SERVER_PORT);
	s_address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(l_socket, reinterpret_cast<sockaddr*>(&s_address), sizeof(s_address));

	listen(l_socket, SOMAXCONN);

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	initialize_clients();

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_socket), g_iocp, 999, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	EXOVER accept_over;
	ZeroMemory(&accept_over.over, sizeof(accept_over.over));
	accept_over.op = OP_ACCEPT;
	accept_over.c_socket = c_socket;
	AcceptEx(l_socket, c_socket, accept_over.io_buf, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.over);	// Ŭ���̾�Ʈ ���ӿ� ����� ������ �̸� �������� ��

	vector <thread> worker_threads;
	for (int i = 0; i < 4; ++i) worker_threads.emplace_back(&IOCPServer::worker_thread, this);
	timer.init_timer(g_iocp);

	for (auto& th : worker_threads)th.join();
}

IOCPServer::~IOCPServer()
{
	closesocket(l_socket);
	CloseHandle(g_iocp);
	WSACleanup();
}

bool IOCPServer::is_player(int id)
{
	return id < NPC_ID_START;
}

bool IOCPServer::is_near(int a, int b)
{
	if (abs(g_clients[a].m_inform.x - g_clients[b].m_inform.x) > VIEW_RADIUS)	return false;
	if (abs(g_clients[a].m_inform.y - g_clients[b].m_inform.y) > VIEW_RADIUS)	return false;
	return true;
}

bool IOCPServer::is_collide(int x, int y)
{
	int sectX = x % SECTOR_WIDTH;
	int sectY = y % SECTOR_WIDTH;
	if(sectX == x && sectY == y)
		if (collidePoints[0].x1 <= sectX && sectX <= collidePoints[0].x2
			&& collidePoints[0].y1 <= sectY && sectY <= collidePoints[0].y2)
			return true;
	for (int i = 1; i < collidePoints.size(); ++i)
	{
		if (collidePoints[i].x1 <= sectX && sectX <= collidePoints[i].x2
			&& collidePoints[i].y1 <= sectY && sectY <= collidePoints[i].y2)
			return true;
	}

	return false;
}

void IOCPServer::init_npc()
{
	for (int i = NPC_ID_START; i < NPC_ID_START + NUM_NPC; ++i) {
		g_clients[i].m_s = 0;
		g_clients[i].m_id = i;
		sprintf_s(g_clients[i].m_inform.m_name, "NPC%d", i);
		g_clients[i].m_status = ST_SLEEP;
		g_clients[i].m_inform.x = rand() % WORLD_WIDTH;
		g_clients[i].m_inform.y = rand() % WORLD_HEIGHT;
		if (i < NUM_NPC / 4 + NPC_ID_START)
			g_clients[i].m_otype = O_EEVEE;
		else if (i < NUM_NPC / 2 + NPC_ID_START)
			g_clients[i].m_otype = O_JOLTEON;
		else if (i < NUM_NPC * 3 / 4 + NPC_ID_START)
			g_clients[i].m_otype = O_FLAREON;
		else 
			g_clients[i].m_otype = O_VAPOREON;
		g_clients[i].m_inform.hp = 10 * (g_clients[i].m_otype);
		g_SectorLock[g_clients[i].m_inform.y / SECTOR_WIDTH][g_clients[i].m_inform.x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[g_clients[i].m_inform.y / SECTOR_WIDTH][g_clients[i].m_inform.x / SECTOR_WIDTH].insert(g_clients[i].m_id);
		g_SectorLock[g_clients[i].m_inform.y / SECTOR_WIDTH][g_clients[i].m_inform.x / SECTOR_WIDTH].LeaveWriteLock();
	
		////g_clients[i].m_last_move_time = high_resolution_clock::now();
		////add_timer(i, OP_RANDOM_MOVE, 1000);
		//lua_State* L = g_clients[i].L = luaL_newstate();
		//
		//luaL_openlibs(L);
		//luaL_loadfile(L, "NPC.LUA");
		//int error = lua_pcall(L, 0, 0, 0);
		//if (error) cout << lua_tostring(L, -1);
		//lua_getglobal(L, "set_uid");
		//lua_pushnumber(L, i);
		//error = lua_pcall(L, 1, 0, 0);
		//if (error) cout << lua_tostring(L, -1);
		////lua_pop(L, 1);
		//
		//lua_register(L, "API_send_message", IOCPServer::API_SendMessage);
		//lua_register(L, "API_get_x", API_get_x);
		//lua_register(L, "API_get_y", API_get_y);
		//lua_register(L, "API_add_timer_run", API_add_timer_run);
		//lua_register(L, "API_run_finished", API_run_finished);
	}
}

void IOCPServer::activate_npc(int id)
{
	C_STATUS old_state = ST_SLEEP;
	if (true == atomic_compare_exchange_strong(&g_clients[id].m_status, &old_state, ST_ACTIVE))
		timer.add_timer(id, OP_RANDOM_MOVE, 1000, 0);
}

void IOCPServer::random_move_npc(int id)
{
	if (g_clients[id].m_status != ST_ACTIVE)
		return;

	int x = g_clients[id].m_inform.x;
	int y = g_clients[id].m_inform.y;
	switch (rand() % 4) {
	case 0:	if (x < (WORLD_WIDTH - 1))	x++; break;
	case 1:	if (x > 0)	x--; break;
	case 2:	if (y < (WORLD_HEIGHT - 1))	y++; break;
	case 3:	if (y > 0)	y--; break;
	}

	if (g_clients[id].m_inform.x / SECTOR_WIDTH != x / SECTOR_WIDTH || g_clients[id].m_inform.y / SECTOR_WIDTH != y / SECTOR_WIDTH)
	{
		g_SectorLock[g_clients[id].m_inform.y / SECTOR_WIDTH][g_clients[id].m_inform.x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[g_clients[id].m_inform.y / SECTOR_WIDTH][g_clients[id].m_inform.x / SECTOR_WIDTH].erase(id);
		g_SectorLock[g_clients[id].m_inform.y / SECTOR_WIDTH][g_clients[id].m_inform.x / SECTOR_WIDTH].LeaveWriteLock();
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[y / SECTOR_WIDTH][x / SECTOR_WIDTH].insert(id);
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].LeaveWriteLock();
	}

	g_clients[id].m_inform.x = x;
	g_clients[id].m_inform.y = y;

	for (int i = g_clients[id].m_inform.y / SECTOR_WIDTH - 1; i <= g_clients[id].m_inform.y / SECTOR_WIDTH + 1; ++i) {
		if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1) continue;
		for (int j = g_clients[id].m_inform.x / SECTOR_WIDTH - 1; j <= g_clients[id].m_inform.x / SECTOR_WIDTH + 1; ++j) {
			if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1) continue;
			g_SectorLock[i][j].EnterReadLock();
			for (auto nearObj : g_ObjectListSector[i][j]) {
				if (ST_ACTIVE != g_clients[nearObj].m_status)	continue;
				if (false == is_player(nearObj))	continue;
				if (true == is_near(nearObj, id)) {
					g_clients[nearObj].m_cl.EnterReadLock();
					if (0 != g_clients[nearObj].view_list.count(id)) {
						g_clients[nearObj].m_cl.LeaveReadLock();
						send_move_packet(nearObj, id);
					}
					else {
						g_clients[nearObj].m_cl.LeaveReadLock();
						send_enter_packet(nearObj, id);
					}
				}
				else {
					g_clients[nearObj].m_cl.EnterReadLock();
					if (0 != g_clients[nearObj].view_list.count(id)) {
						g_clients[nearObj].m_cl.LeaveReadLock();
						send_leave_packet(nearObj, id);
					}
					else
						g_clients[nearObj].m_cl.LeaveReadLock();
				}
			}
			g_SectorLock[i][j].LeaveReadLock();
		}
	}
}

void IOCPServer::respawn_npc(int id)
{
	bool flag = false;
	if (g_clients[id].m_status != ST_ACTIVE)
		g_clients[id].m_status = ST_SLEEP;
	g_clients[id].m_inform.hp = 10 * g_clients[id].m_otype;

	for (int i = g_clients[id].m_inform.y / SECTOR_WIDTH - 1; i <= g_clients[id].m_inform.y / SECTOR_WIDTH + 1; ++i) {
		if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1) continue;
		for (int j = g_clients[id].m_inform.x / SECTOR_WIDTH - 1; j <= g_clients[id].m_inform.x / SECTOR_WIDTH + 1; ++j) {
			if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1) continue;
			g_SectorLock[i][j].EnterReadLock();
			for (auto nearObj : g_ObjectListSector[i][j]) {
				if (ST_ACTIVE != g_clients[nearObj].m_status)	continue;
				if (false == is_player(nearObj))	continue;
				if (true == is_near(nearObj, id)) {
					flag = true;
					g_clients[nearObj].m_cl.EnterReadLock();
					if (0 != g_clients[nearObj].view_list.count(id)) {
						g_clients[nearObj].m_cl.LeaveReadLock();
						send_move_packet(nearObj, id);
					}
					else {
						g_clients[nearObj].m_cl.LeaveReadLock();
						send_enter_packet(nearObj, id);
					}
				}
				else {
					g_clients[nearObj].m_cl.EnterReadLock();
					if (0 != g_clients[nearObj].view_list.count(id)) {
						g_clients[nearObj].m_cl.LeaveReadLock();
						send_leave_packet(nearObj, id);
					}
					else
						g_clients[nearObj].m_cl.LeaveReadLock();
				}
			}
			g_SectorLock[i][j].LeaveReadLock();
		}
	}

	if (flag)
		activate_npc(id);
}

void IOCPServer::initialize_clients()
{
	for (int i = 0; i < NPC_ID_START; ++i){
		g_clients[i].m_id = i;
		g_clients[i].m_status = ST_FREE;
	}
}

void IOCPServer::process_packet(int user_id, char* buf)
{
	switch (buf[1]) {
	case C2S_LOGIN:
	{
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(buf);
		Inform p = database.find_name_in_database(packet->name);
		bool canEnter = true;
		for (int i = 0; i < NPC_ID_START; ++i)
		{
			if (i == user_id)
				continue;
			if (g_clients[i].m_status == ST_ACTIVE)
			{
				string cmp1 = g_clients[i].m_inform.m_name;
				string cmp2 = packet->name;
				if (cmp1 == cmp2)
				{
					canEnter = false;
					break;
				}
			}
		}
		if (p.x != -1 && canEnter)
		{
			g_clients[user_id].m_inform = p;
			enter_game(user_id, packet->name);
		}
		else if(!canEnter)
		{
			send_login_fail_packet(user_id);
			disconnect(user_id);
		}
		else
		{
			p = database.add_player_in_database(packet->name);
			g_clients[user_id].m_inform = p;
			enter_game(user_id, packet->name);
		}
	}
	break;
	case C2S_MOVE:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
		g_clients[user_id].m_move_time = packet->move_time;
		do_move(user_id, packet->direction);
	}
	break;
	case C2S_ATTACK:
	{
		cs_packet_attack* packet = reinterpret_cast<cs_packet_attack*>(buf);
		do_attack(user_id);
	}
	break;
	case C2S_CHAT:
	{
		cs_packet_chat* packet = reinterpret_cast<cs_packet_chat*>(buf);
		chatting(user_id, packet->message);
	}
	break;
	case C2S_LOGOUT:
	{
		cs_packet_logout* packet = reinterpret_cast<cs_packet_logout*>(buf);
	}
	break;
	default:
		cout << "Unknown Packet Type Error!\n";
		DebugBreak();
		//exit(-1);
		break;
	}
}

void IOCPServer::recv_packet_construct(int user_id, int io_byte)
{
	Client& cu = g_clients[user_id];
	EXOVER& r_o = cu.m_recv_over;

	// ������ ����Ʈ
	int rest_byte = io_byte;
	// ó���� �������� ������
	char* p = r_o.io_buf;
	int packet_size = 0;
	// �̸� �޾Ƴ� �� �ִٸ�?
	if (0 != cu.m_prev_size)	packet_size = cu.m_packet_buf[0];
	// ó���� �����Ͱ� �����ִٸ�?
	while (rest_byte > 0)
	{
		// �츮�� ó���ؾ� �Ǵ� ��Ŷ�� ���� ó���غ� ���� ���� ��
		// == ��Ŷ�� ���� �κ��� �ִ�
		if (0 == packet_size)	packet_size = (unsigned char)*p;
		// ��Ŷ�� �ϼ��� �� �ִ�
		if (packet_size <= rest_byte + cu.m_prev_size) {
			memcpy(cu.m_packet_buf + cu.m_prev_size, p, packet_size - cu.m_prev_size);
			p += packet_size - cu.m_prev_size;
			rest_byte -= packet_size - cu.m_prev_size;
			packet_size = 0;	// �� ��Ŷ�� ó���Ǿ���
			process_packet(user_id, cu.m_packet_buf);
			cu.m_prev_size = 0;
		}
		else // �ϼ��� �� ����
		{
			// �����Ͽ� ���� ���� ������ ����� ��
			memcpy(cu.m_packet_buf + cu.m_prev_size, p, rest_byte);
			cu.m_prev_size += rest_byte;
			rest_byte = 0;
			p += rest_byte;
		}
	}
}

void IOCPServer::worker_thread()
{
	while (true) {

		DWORD io_byte;
		ULONG_PTR key;
		WSAOVERLAPPED* over;
		GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE);

		EXOVER* exover = reinterpret_cast<EXOVER*>(over);
		int user_id = static_cast<int>(key);
		Client& cl = g_clients[user_id];

		switch (exover->op) {
		case OP_RECV:
			// send�� recv�� ��쿡�� �� ó���� ����� ��
			if (0 == io_byte)
				disconnect(user_id);
			else {
				recv_packet_construct(user_id, io_byte);
				ZeroMemory(&cl.m_recv_over.over, sizeof(cl.m_recv_over.over));
				DWORD flags = 0;
				WSARecv(cl.m_s, &cl.m_recv_over.wsabuf, 1, NULL, &flags, &cl.m_recv_over.over, NULL);
			}
			break;
		case OP_SEND:
			// send�� recv�� ��쿡�� �� ó���� ����� ��
			if (0 == io_byte)
				disconnect(user_id);
			delete exover;
			break;
		case OP_ACCEPT:
		{
			// ���ο��� �񵿱� ACCEPT�� �ϰ�, ��Ŀ �����忡�� �Ϸ��ϰ�,
			// �Ϸ��� �����忡�� ACEEPT�� �� ������ �� �ڵ�� �̱� ������� ���ư���.
			// �׷��� ���� �� �ʿ䰡 ����.
			// �׷��� ���ÿ� ���� Ŭ���̾�Ʈ�� �����ϴ� ���(������ �ٿ�Ǽ� �ٽ� �����ϴ� ���)
			// �׷� ��쿡�� ���� �����忡�� ���ÿ� ACCEPT�� �� �� �ֱ� ������ ���� �ɾ�� �Ѵ�.
			int user_id = -1;
			for (int i = 0; i < NPC_ID_START; ++i) {
				// lock_guard�� ��ϵ� ����� �������� �� unlock�� ����
				// ������ �������� ���� ��ϵǰ�, ������� ���� �ݺ��ϰ� ��
				// break �Ӹ� �ƴ϶� return, continue ��� unlock�� ����
				if (ST_FREE == g_clients[i].m_status) {
					g_clients[i].m_status = ST_ALLOC;
					user_id = i;
					break;
				}
			}

			SOCKET c_socket = exover->c_socket;
			if (-1 == user_id)
				closesocket(c_socket);
			else {
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_iocp, user_id, 0);

				Client& nc = g_clients[user_id];
				// �Ⱦ��̴� �� ��󳻴� �͵� �ؾ������� ���߿� ����
				nc.m_prev_size = 0;
				nc.m_recv_over.op = OP_RECV;
				ZeroMemory(&nc.m_recv_over.over, sizeof(nc.m_recv_over.over));
				nc.m_recv_over.wsabuf.buf = nc.m_recv_over.io_buf;
				nc.m_recv_over.wsabuf.len = MAX_BUF_SIZE;
				nc.m_s = c_socket;
				nc.view_list.clear();
				nc.m_inform.x = rand() % WORLD_WIDTH;
				nc.m_inform.y = rand() % WORLD_HEIGHT;
				nc.m_inform.exp = 0;
				nc.m_inform.hp = 100;
				nc.m_inform.level = 1;
				nc.m_otype = O_HUMAN;
				DWORD flags = 0;
				WSARecv(c_socket, &nc.m_recv_over.wsabuf, 1, NULL, &flags, &nc.m_recv_over.over, NULL);
			}
			// Accept ���� ����� ����
			c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			exover->c_socket = c_socket;
			ZeroMemory(&exover->over, sizeof(exover->over));
			AcceptEx(l_socket, c_socket, exover->io_buf, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &exover->over);
		}
		break;
		case OP_RANDOM_MOVE:
		{
			random_move_npc(user_id);
			bool keep_alive = false;
			for (int i = 0; i < NPC_ID_START; ++i) {
				if (true == is_near(user_id, i))
					if (ST_ACTIVE == g_clients[i].m_status) {
						keep_alive = true;
						break;
					}
			}
			if (true == keep_alive) timer.add_timer(user_id, OP_RANDOM_MOVE, 1000, 0);
			else if (g_clients[user_id].m_status == ST_DEAD);
			else
				g_clients[user_id].m_status = ST_SLEEP;
			delete exover;
		}
		break;
		case OP_PLAYER_MOVE:
		{
			//g_clients[user_id].lua_l.EnterWriteLock();
			//lua_State* L = g_clients[user_id].L;
			//lua_getglobal(L, "event_player_move");
			//lua_pushnumber(L, exover->p_id);
			//lua_pcall(L, 1, 0, 0);
			////lua_pop(L, 1);
			//g_clients[user_id].lua_l.LeaveWriteLock();
			//delete exover;
		}
		break;
		case OP_RUN:
		{
			//g_clients[user_id].lua_l.EnterWriteLock();
			//lua_State* L = g_clients[user_id].L;
			//lua_getglobal(L, "event_run");
			//lua_pushnumber(L, exover->p_id);
			//lua_pcall(L, 1, 0, 0);
			////lua_pop(L, 1);
			//g_clients[user_id].lua_l.LeaveWriteLock();
			//delete exover;
		}
		break;
		case OP_RUN_FINISH:
		{
			//g_clients[user_id].lua_l.EnterWriteLock();
			//lua_State* L = g_clients[user_id].L;
			//lua_getglobal(L, "event_run_finished");
			//lua_pushnumber(L, exover->p_id);
			//lua_pcall(L, 1, 0, 0);
			//g_clients[user_id].lua_l.LeaveWriteLock();
			//delete exover;
		}
		break;
		case OP_HEAL:
		{
			g_clients[user_id].m_cl.EnterWriteLock();
			g_clients[user_id].heal_player();
			g_clients[user_id].m_cl.LeaveWriteLock();
			send_stat_change_packet(user_id);
			if (g_clients[user_id].m_inform.hp < 98 + pow(2, g_clients[user_id].m_inform.level))
				timer.add_timer(user_id, OP_HEAL, 5000, 0);
			delete exover;
		}
		break;
		case OP_RESPAWN:
		{
			respawn_npc(user_id);
			delete exover;
		}
		break;
		default:
			cout << "Unknown Operation in worker_thread!!\n";
			while (true);
		}
	}
}

void IOCPServer::enter_game(int user_id, char name[])
{
	g_clients[user_id].m_cl.EnterWriteLock();
	strcpy_s(g_clients[user_id].m_inform.m_name, name);
	g_clients[user_id].m_inform.m_name[MAX_ID_LEN] = NULL;
	send_login_ok_packet(user_id);
	g_clients[user_id].m_status = ST_ACTIVE;
	g_clients[user_id].m_cl.LeaveWriteLock();

	g_SectorLock[g_clients[user_id].m_inform.y / SECTOR_WIDTH][g_clients[user_id].m_inform.x / SECTOR_WIDTH].EnterWriteLock();
	g_ObjectListSector[g_clients[user_id].m_inform.y / SECTOR_WIDTH][g_clients[user_id].m_inform.x / SECTOR_WIDTH].insert(user_id);
	g_SectorLock[g_clients[user_id].m_inform.y / SECTOR_WIDTH][g_clients[user_id].m_inform.x / SECTOR_WIDTH].LeaveWriteLock();

	for (int i = g_clients[user_id].m_inform.y / SECTOR_WIDTH - 1; i <= g_clients[user_id].m_inform.y / SECTOR_WIDTH + 1; ++i) {
		if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1) continue;
		for (int j = g_clients[user_id].m_inform.x / SECTOR_WIDTH - 1; j <= g_clients[user_id].m_inform.x / SECTOR_WIDTH + 1; ++j) {
			if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1) continue;
			g_SectorLock[i][j].EnterReadLock();
			for (auto nearObj : g_ObjectListSector[i][j]) {
				if (nearObj == user_id) continue;
				if (true == is_near(user_id, nearObj)) {
					if (ST_SLEEP == g_clients[nearObj].m_status) {
						activate_npc(nearObj);
					}
					if (ST_ACTIVE == g_clients[nearObj].m_status) {
						send_enter_packet(user_id, nearObj);
						if (true == is_player(nearObj))
							send_enter_packet(nearObj, user_id);
					}
				}
			}
			g_SectorLock[i][j].LeaveReadLock();
		}
	}

	if (g_clients[user_id].m_inform.hp < 98 + pow(2, g_clients[user_id].m_inform.level))
		timer.add_timer(user_id, OP_HEAL, 5000, 0);
}

void IOCPServer::do_move(int user_id, int direction)
{
	Client& u = g_clients[user_id];
	//if (high_resolution_clock::now() - u.m_last_move_time < 1s)
	//	return;

	int x = u.m_inform.x;
	int y = u.m_inform.y;
	switch (direction)
	{
	case D_UP:	if (y > 0)	y--;	break;
	case D_DOWN:if (y < (WORLD_HEIGHT - 1))	y++;	break;
	case D_LEFT:if (x > 0)	x--;	break;
	case D_RIGHT:if (x < (WORLD_WIDTH - 1)) x++;	break;
	default:
		cout << "Unknown Direction from Client move packet!\n";
		DebugBreak();
		//exit(-1);
	}

	if (is_collide(x, y))
		return;

	if (u.m_inform.x / SECTOR_WIDTH != x / SECTOR_WIDTH || u.m_inform.y / SECTOR_WIDTH != y / SECTOR_WIDTH)
	{
		g_SectorLock[u.m_inform.y / SECTOR_WIDTH][u.m_inform.x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[u.m_inform.y / SECTOR_WIDTH][u.m_inform.x / SECTOR_WIDTH].erase(user_id);
		g_SectorLock[u.m_inform.y / SECTOR_WIDTH][u.m_inform.x / SECTOR_WIDTH].LeaveWriteLock();
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[y / SECTOR_WIDTH][x / SECTOR_WIDTH].insert(user_id);
		g_SectorLock[y / SECTOR_WIDTH][x / SECTOR_WIDTH].LeaveWriteLock();
	}

	u.m_inform.x = x;
	u.m_inform.y = y;
	u.m_last_move_time = high_resolution_clock::now();

	g_clients[user_id].m_cl.EnterReadLock();
	unordered_set<int> old_vl = g_clients[user_id].view_list;
	g_clients[user_id].m_cl.LeaveReadLock();
	unordered_set<int> new_vl;

	for (int i = u.m_inform.y / SECTOR_WIDTH - 1; i <= u.m_inform.y / SECTOR_WIDTH + 1; ++i) {
		if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1) continue;
		for (int j = u.m_inform.x / SECTOR_WIDTH - 1; j <= u.m_inform.x / SECTOR_WIDTH + 1; ++j) {
			if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1) continue;
			g_SectorLock[i][j].EnterReadLock();
			for (auto nearObj : g_ObjectListSector[i][j]) {
				if (false == is_near(nearObj, user_id))	continue;
				if (ST_SLEEP == g_clients[nearObj].m_status) activate_npc(nearObj);
				if (ST_ACTIVE != g_clients[nearObj].m_status)continue;
				if (nearObj == user_id)continue;
				if (false == is_player(nearObj)) {
					EXOVER* over = new EXOVER;
					over->op = OP_PLAYER_MOVE;
					over->p_id = user_id;
					PostQueuedCompletionStatus(g_iocp, 1, nearObj, &over->over);
				}
				new_vl.insert(nearObj);
			}
			g_SectorLock[i][j].LeaveReadLock();
		}
	}

	send_move_packet(user_id, user_id);

	for (auto np : new_vl) {
		if (false == is_player(np))
			if (ST_ACTIVE == g_clients[np].m_status)
				if (u.m_inform.x == g_clients[np].m_inform.x
					&& u.m_inform.y == g_clients[np].m_inform.y)
					player_damaged(user_id, np);

		if (0 == old_vl.count(np)) {	// Object�� ���� �þ߿� ������ ��
			send_enter_packet(user_id, np);
			if (false == is_player(np))
				continue;
			g_clients[np].m_cl.EnterReadLock();
			if (0 == g_clients[np].view_list.count(user_id)) {
				g_clients[np].m_cl.LeaveReadLock();
				send_enter_packet(np, user_id);
			}
			else {
				g_clients[np].m_cl.LeaveReadLock();
				send_move_packet(np, user_id);
			}
		}
		else {	// ��� �þ߿� �����ϰ� ���� ��
			if (false == is_player(np))	continue;
			g_clients[np].m_cl.EnterReadLock();
			if (0 != g_clients[np].view_list.count(user_id)) {
				g_clients[np].m_cl.LeaveReadLock();
				send_move_packet(np, user_id);
			}
			else {
				g_clients[np].m_cl.LeaveReadLock();
				send_enter_packet(np, user_id);
			}
		}
	}

	for (auto old_p : old_vl) {		// Object�� �þ߿��� ����� ��
		if (0 == new_vl.count(old_p)) {
			send_leave_packet(user_id, old_p);
			if (false == is_player(old_p))	continue;
			g_clients[old_p].m_cl.EnterReadLock();
			if (0 != g_clients[old_p].view_list.count(user_id)) {
				g_clients[old_p].m_cl.LeaveReadLock();
				send_leave_packet(old_p, user_id);
			}
			else {
				g_clients[old_p].m_cl.LeaveReadLock();
			}
		}
	}
}

void IOCPServer::do_attack(int user_id)
{
	Client& u = g_clients[user_id];
	u.m_cl.EnterReadLock();
	unordered_set<int> vl = u.view_list;
	int x = u.m_inform.x, y = u.m_inform.y;
	u.m_cl.LeaveReadLock();

	for (auto cl : vl)
	{
		if (true == is_player(cl))
			continue;
		g_clients[cl].m_cl.EnterReadLock();
		if ((x == g_clients[cl].m_inform.x && y + 1 == g_clients[cl].m_inform.y)
			|| (x == g_clients[cl].m_inform.x && y - 1 == g_clients[cl].m_inform.y)
			|| (x + 1 == g_clients[cl].m_inform.x && y == g_clients[cl].m_inform.y)
			|| (x - 1 == g_clients[cl].m_inform.x && y == g_clients[cl].m_inform.y))
		{
			g_clients[cl].m_cl.LeaveReadLock();
			monster_damaged(user_id, cl);
		}
		else
			g_clients[cl].m_cl.LeaveReadLock();
	}
}

void IOCPServer::player_damaged(int user_id, int monster_id)
{
	Client& u = g_clients[user_id];
	u.m_inform.hp -= 15;

	if (u.m_inform.hp > 0)
	{
		wchar_t msg[MAX_STR_LEN];
		wsprintf(msg, L"�� �������� %d�� �������� �Ծ����ϴ�.", 15);
		send_stat_change_packet(user_id);
		send_chat_packet(user_id, monster_id, msg);
		timer.add_timer(user_id, OP_HEAL, 5000, 0);
		return;
	}
	else
	{
		int respawnX = 20;
		int respawnY = 5;
		g_SectorLock[u.m_inform.y / SECTOR_WIDTH][u.m_inform.x / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[u.m_inform.y / SECTOR_WIDTH][u.m_inform.x / SECTOR_WIDTH].erase(user_id);
		g_SectorLock[u.m_inform.y / SECTOR_WIDTH][u.m_inform.x / SECTOR_WIDTH].LeaveWriteLock();
		g_SectorLock[respawnY / SECTOR_WIDTH][respawnX / SECTOR_WIDTH].EnterWriteLock();
		g_ObjectListSector[respawnY / SECTOR_WIDTH][respawnX / SECTOR_WIDTH].insert(user_id);
		g_SectorLock[respawnY / SECTOR_WIDTH][respawnX / SECTOR_WIDTH].LeaveWriteLock();

		u.m_inform.hp = 98 + pow(2, u.m_inform.level);
		u.m_inform.exp /= 2;
		u.m_inform.x = 20;
		u.m_inform.y = 5;

		wchar_t msg[MAX_STR_LEN] = L"�� �������� ����Ͽ����ϴ�.";
		send_chat_packet(user_id, monster_id, msg);
		send_stat_change_packet(user_id);

		u.m_last_move_time = high_resolution_clock::now();

		u.m_cl.EnterReadLock();
		unordered_set<int> old_vl = u.view_list;
		u.m_cl.LeaveReadLock();
		unordered_set<int> new_vl;

		for (int i = u.m_inform.y / SECTOR_WIDTH - 1; i <= u.m_inform.y / SECTOR_WIDTH + 1; ++i) {
			if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1) continue;
			for (int j = u.m_inform.x / SECTOR_WIDTH - 1; j <= u.m_inform.x / SECTOR_WIDTH + 1; ++j) {
				if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1) continue;
				g_SectorLock[i][j].EnterReadLock();
				for (auto nearObj : g_ObjectListSector[i][j]) {
					if (false == is_near(nearObj, user_id))	continue;
					if (ST_SLEEP == g_clients[nearObj].m_status) activate_npc(nearObj);
					if (ST_ACTIVE != g_clients[nearObj].m_status)continue;
					if (nearObj == user_id)continue;
					if (false == is_player(nearObj)) {
						EXOVER* over = new EXOVER;
						over->op = OP_PLAYER_MOVE;
						over->p_id = user_id;
						PostQueuedCompletionStatus(g_iocp, 1, nearObj, &over->over);
					}
					new_vl.insert(nearObj);
				}
				g_SectorLock[i][j].LeaveReadLock();
			}
		}

		send_move_packet(user_id, user_id);

		for (auto np : new_vl) {
			if (0 == old_vl.count(np)) {	// Object�� ���� �þ߿� ������ ��
				send_enter_packet(user_id, np);
				if (false == is_player(np))
					continue;
				g_clients[np].m_cl.EnterReadLock();
				if (0 == g_clients[np].view_list.count(user_id)) {
					g_clients[np].m_cl.LeaveReadLock();
					send_enter_packet(np, user_id);
				}
				else {
					g_clients[np].m_cl.LeaveReadLock();
					send_move_packet(np, user_id);
				}
			}
			else {	// ��� �þ߿� �����ϰ� ���� ��
				if (false == is_player(np))	continue;
				g_clients[np].m_cl.EnterReadLock();
				if (0 != g_clients[np].view_list.count(user_id)) {
					g_clients[np].m_cl.LeaveReadLock();
					send_move_packet(np, user_id);
				}
				else {
					g_clients[np].m_cl.LeaveReadLock();
					send_enter_packet(np, user_id);
				}
			}
		}

		for (auto old_p : old_vl) {		// Object�� �þ߿��� ����� ��
			if (0 == new_vl.count(old_p)) {
				send_leave_packet(user_id, old_p);
				if (false == is_player(old_p))	continue;
				g_clients[old_p].m_cl.EnterReadLock();
				if (0 != g_clients[old_p].view_list.count(user_id)) {
					g_clients[old_p].m_cl.LeaveReadLock();
					send_leave_packet(old_p, user_id);
				}
				else {
					g_clients[old_p].m_cl.LeaveReadLock();
				}
			}
		}
	}
}

void IOCPServer::monster_damaged(int user_id, int monster_id)
{
	Client& u = g_clients[user_id];
	Client& m = g_clients[monster_id];

	m.m_inform.hp -= u.m_inform.level * 5 + 10;
	
	if (m.m_inform.hp > 0)
	{
		wchar_t msg[MAX_STR_LEN];
		wsprintf(msg, L"�� %s�� ������ %d�� �������� �������ϴ�.", m.m_inform.m_name, u.m_inform.level * 5 + 10);
		send_chat_packet(user_id, user_id, msg);
		return;
	}
	else
	{
		u.exp_plus(m.m_otype * 5);
		send_stat_change_packet(user_id);
		wchar_t msg[MAX_STR_LEN];
		wsprintf(msg, L"�� %s�� ���񷯼� %d�� ����ġ�� ������ϴ�.", m.m_inform.m_name, m.m_otype * 5);
		send_chat_packet(user_id, user_id, msg);

		for (int i = m.m_inform.y / SECTOR_WIDTH - 1; i <= m.m_inform.y / SECTOR_WIDTH + 1; ++i) {
			if (i < 0 || i > WORLD_HEIGHT / SECTOR_WIDTH - 1) continue;
			for (int j = m.m_inform.x / SECTOR_WIDTH - 1; j <= m.m_inform.x / SECTOR_WIDTH + 1; ++j) {
				if (j < 0 || j > WORLD_WIDTH / SECTOR_WIDTH - 1) continue;
				g_SectorLock[i][j].EnterReadLock();
				for (auto nearObj : g_ObjectListSector[i][j]) {
					if (false == is_near(nearObj, monster_id))	continue;
					if (ST_ACTIVE != g_clients[nearObj].m_status)continue;
					if (nearObj == monster_id)continue;
					if (true == is_player(nearObj)) {
						if (g_clients[nearObj].view_list.count(monster_id) != 0)
							send_leave_packet(nearObj, monster_id);

					}
				}
				g_SectorLock[i][j].LeaveReadLock();
			}
		}

		m.m_status = ST_DEAD;
		timer.add_timer(monster_id, OP_RESPAWN, 30000, 0);
	}

}

void IOCPServer::chatting(int user_id, wchar_t mess[])
{
	for (int i = 0; i < NPC_ID_START; ++i)
	{
		Client& cl = g_clients[i];
		if (ST_ACTIVE == cl.m_status)
			send_chat_packet(cl.m_id, user_id, mess);
	}
}

void IOCPServer::disconnect(int user_id)
{
	Inform& inform = g_clients[user_id].m_inform;
	database.set_data_in_database(inform.m_name, inform.x, inform.y, inform.level, inform.exp, inform.hp);

	g_SectorLock[g_clients[user_id].m_inform.y / SECTOR_WIDTH][g_clients[user_id].m_inform.x / SECTOR_WIDTH].EnterWriteLock();
	if (g_ObjectListSector[g_clients[user_id].m_inform.y / SECTOR_WIDTH][g_clients[user_id].m_inform.x / SECTOR_WIDTH].count(user_id) != 0)
		g_ObjectListSector[g_clients[user_id].m_inform.y / SECTOR_WIDTH][g_clients[user_id].m_inform.x / SECTOR_WIDTH].erase(user_id);
	g_SectorLock[g_clients[user_id].m_inform.y / SECTOR_WIDTH][g_clients[user_id].m_inform.x / SECTOR_WIDTH].LeaveWriteLock();

	send_leave_packet(user_id, user_id);

	g_clients[user_id].m_cl.EnterWriteLock();
	g_clients[user_id].m_status = ST_ALLOC;

	closesocket(g_clients[user_id].m_s);
	for (int i = 0; i < NPC_ID_START; ++i)
	{
		Client& cl = g_clients[i];
		if (user_id == cl.m_id) continue;
		if (ST_ACTIVE == cl.m_status)
			send_leave_packet(cl.m_id, user_id);
	}
	g_clients[user_id].m_status = ST_FREE;
	g_clients[user_id].m_cl.LeaveWriteLock();
}

void IOCPServer::send_packet(int user_id, void* p)
{
	unsigned char* buf = reinterpret_cast<unsigned char*>(p);

	Client& u = g_clients[user_id];

	EXOVER* exover = new EXOVER;
	exover->op = OP_SEND;
	ZeroMemory(&exover->over, sizeof(exover->over));
	exover->wsabuf.buf = exover->io_buf;
	exover->wsabuf.len = buf[0];
	memcpy(exover->io_buf, buf, buf[0]);
	// IpBuffers �׸� u�� wsabuf�� �̹� Recv���� ���� �ֱ� ������ ����ϸ� �ȵ�
	WSASend(u.m_s, &exover->wsabuf, 1, NULL, 0, &exover->over, NULL);
}

void IOCPServer::send_login_ok_packet(int user_id)
{
	sc_packet_login_ok p;
	p.size = sizeof(p);
	p.type = S2C_LOGIN_OK;
	p.id = user_id;
	p.x = g_clients[user_id].m_inform.x;
	p.y = g_clients[user_id].m_inform.y;
	p.hp = g_clients[user_id].m_inform.hp;
	p.level = g_clients[user_id].m_inform.level;
	p.exp = g_clients[user_id].m_inform.exp;

	send_packet(user_id, &p);
}

void IOCPServer::send_login_fail_packet(int user_id) 
{
	sc_packet_login_fail p;
	p.size = sizeof(p);
	p.type = S2C_LOGIN_FAIL;

	send_packet(user_id, &p);
}

void IOCPServer::send_enter_packet(int user_id, int o_id)
{
	sc_packet_enter p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_ENTER;
	p.x = g_clients[o_id].m_inform.x;
	p.y = g_clients[o_id].m_inform.y;
	strcpy_s(p.name, g_clients[o_id].m_inform.m_name);
	p.o_type = g_clients[o_id].m_otype;

	g_clients[user_id].m_cl.EnterWriteLock();
	g_clients[user_id].view_list.insert(o_id);
	g_clients[user_id].m_cl.LeaveWriteLock();

	send_packet(user_id, &p);
}

void IOCPServer::send_move_packet(int user_id, int mover)
{
	sc_packet_move p;
	p.id = mover;
	p.size = sizeof(p);
	p.type = S2C_MOVE;
	p.x = g_clients[mover].m_inform.x;
	p.y = g_clients[mover].m_inform.y;
	p.move_time = g_clients[mover].m_move_time;

	send_packet(user_id, &p);
}

void IOCPServer::send_leave_packet(int user_id, int o_id)
{
	sc_packet_leave p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = S2C_LEAVE;

	g_clients[user_id].m_cl.EnterWriteLock();
	g_clients[user_id].view_list.erase(o_id);
	g_clients[user_id].m_cl.LeaveWriteLock();

	send_packet(user_id, &p);
}

void IOCPServer::send_chat_packet(int user_id, int chatter, wchar_t mess[])
{
	sc_packet_chat p;
	p.id = chatter;
	p.size = sizeof(p);
	p.type = S2C_CHAT;
	wcscpy_s(p.mess, mess);

	send_packet(user_id, &p);
}

void IOCPServer::send_stat_change_packet(int user_id)
{
	sc_packet_stat_change p;
	p.size = sizeof(p);
	p.type = S2C_STAT_CHANGE;
	p.hp = g_clients[user_id].m_inform.hp;
	p.level = g_clients[user_id].m_inform.level;
	p.exp = g_clients[user_id].m_inform.exp;

	send_packet(user_id, &p);
}

int IOCPServer::API_SendMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	wchar_t* mess = (wchar_t*)lua_tostring(L, -1);

	send_chat_packet(user_id, my_id, mess);
	lua_pop(L, 3);
	return 0;
}

int IOCPServer::API_get_x(lua_State* L)
{
	int obj_id = (int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	int x = g_clients[obj_id].m_inform.x;
	lua_pushnumber(L, x);
	return 1;
}

int IOCPServer::API_get_y(lua_State* L)
{
	int obj_id = (int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	int y = g_clients[obj_id].m_inform.y;
	lua_pushnumber(L, y);
	return 1;
}

int IOCPServer::API_add_timer_run(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -2);
	int user_id = (int)lua_tointeger(L, -1);
	timer.add_timer(my_id, OP_RUN, 1000, user_id);
	lua_pop(L, 2);
	return 0;
}

int IOCPServer::API_run_finished(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -2);
	int user_id = (int)lua_tointeger(L, -1);

	EXOVER* over = new EXOVER;
	over->op = OP_RUN_FINISH;
	over->p_id = user_id;

	PostQueuedCompletionStatus(g_iocp, 1, my_id, &over->over);
	lua_pop(L, 2);
	return 0;
}
