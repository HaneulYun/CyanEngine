myid = 9999
is_runAfter = false
runCount = 0

function set_uid(x)
myid = x
end

function event_player_move(p_id)
	if API_get_x(p_id) == API_get_x(myid) then
		if API_get_y(p_id) == API_get_y(myid) then
			API_send_message(myid, player, "HELLO")
			is_runAfter = true
			runCount = 0
			API_add_timer_run(myid)
		end
	end
end

function event_run()
	if is_runAfter == true then
		runCount = runCount + 1
		if runCount < 3 then
			API_add_timer_run(myid)
		else
			API_run_finished(myid)
		end
	end
end

function event_run_finished()
	API_send_message(myid, player, "BYE")
	is_runAfter = false
end