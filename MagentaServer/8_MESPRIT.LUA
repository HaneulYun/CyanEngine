myid = 9999
myname = "Mesprit"
mylevel = 8
myhp = 80
myfirstX = 9999
myfirstY = 9999

function set_uid(x)
myid = x
end

function set_firstX(x)
myfirstX = x
end

function set_firstY(y)
myfirstY = y
end

function event_respawn()
	API_set_x(myid, myfirstX)
	API_set_y(myid, myfirstY)
	API_set_hp(myid, myhp)
end

function event_player_move(p_id)
	if API_get_x(p_id) == API_get_x(myid) then
		if API_get_y(p_id) == API_get_y(myid) then
			API_player_damaged(p_id, myid)
		end
	end
end

function event_random_move()
	API_type_random_move(myid, myfirstX, myfirstY)
end

function event_attacked(p_id)
	API_pathFind(myid, p_id, myfirstX, myfirstY)
end