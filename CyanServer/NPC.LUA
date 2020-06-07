myid = 9999

function set_uid(x)
    myid = x
end

function event_player_move(p_id)
    if API_get_x(p_id) == API_get_x(myid) and API_get_y(p_id) == API_get_y(myid) then
        API_send_message(myid, player, "HELLO")
        API_push_queue_bye_message(myid, player)
    end
end

function event_npc_move(o_id)
    API_random_move(o_id)
end