--
-- a very limited rcon by hifi <3
--

local quit_on_empty = true

local cvars = {
    -- server
    "password", "maxclients", "timelimit", "dmflags", "sv_gravity", "sv_iplimit", "fraglimit",
    "sv_anticheat_required",

    -- mod
    "teamplay", "ctf", "matchmode", "roundtimelimit", "tgren", "limchasecam", "forcedteamtalk",
    "mm_forceteamtalk", "ir", "wp_flags", "itm_flags", "hc_single", "use_punch",  "darkmatch",
    "allitem", "allweapon", "use_3teams"
}

local claimer = nil
local claimer_store = nil

function q2a_load()
    claimer_store = gi.cvar("lua_q2a_lrcon_storage")
end

function q2a_unload()
    if claimer then
        gi.cvar_set("lua_q2a_lrcon_storage", players[claimer].name..players[claimer].ip)
    else
        gi.cvar_set("lua_q2a_lrcon_storage", "")
    end
end

function ClientCommand(client)
    if(gi.argv(0) == 'lrcon') then
        if gi.argc() == 1 then
            gi.cprintf(client, PRINT_HIGH, "Usage: lrcon <command> [parameters]\n")
            gi.cprintf(client, PRINT_HIGH, "Type \"lrcon help\" for more information.\n")
        else
            local plr = players[client]
            local ip = string.match(plr.ip, '^([^:]+)')
            local now = os.time()
            local cmd = gi.argv(1)

            if cmd == 'claim' then
                if claimer == nil then
                    claimer = client
                    gi.cprintf(client, PRINT_HIGH, "You have claimed the server! Type lrcon <command> to set appropriate settings.\n")
                    gi.bprintf(PRINT_HIGH, players[client].name.." claimed the server\n")
                else
                    gi.cprintf(client, PRINT_HIGH, "This server has already been claimed by "..players[claimer].name..".\n")
                end
                return true
            elseif cmd == 'release' then
                if client == claimer then
                    gi.cprintf(client, PRINT_HIGH, "You released the server.\n")
                    gi.bprintf(PRINT_HIGH, players[client].name.." relesed the server, use \"lrcon claim\" to re-claim\n")
                else
                    gi.cprintf(client, PRINT_HIGH, "You have not claimed this server.\n")
                end
                return true
            elseif cmd == 'help' then
                gi.cprintf(client, PRINT_HIGH, "Limited rcon usage:\n")
                gi.cprintf(client, PRINT_HIGH, " lrcon claim             - claim the server\n")
                gi.cprintf(client, PRINT_HIGH, " lrcon release           - release the server to be re-claimed\n")
                gi.cprintf(client, PRINT_HIGH, " lrcon <cvar>            - query cvar value\n")
                gi.cprintf(client, PRINT_HIGH, " lrcon <cvar> <value>    - set cvar value\n")
                gi.cprintf(client, PRINT_HIGH, " lrcon status            - get client status information\n")
                gi.cprintf(client, PRINT_HIGH, " lrcon kick <id>         - kick a player\n")
                gi.cprintf(client, PRINT_HIGH, " lrcon map <mapname>     - change map\n")
                gi.cprintf(client, PRINT_HIGH, " lrcon gamemap <mapname> - change map (keeping state)\n")
                return true
            else
                if client == claimer then
                    local cmd = gi.argv(1)
                    local param

                    if gi.argc() > 2 then
                        param = gi.argv(2)
                    end

                    if gi.argv(1) == 'set' and gi.argc() > 2 then
                        cmd = gi.argv(2)
                        param = gi.argv(3)
                    end

                    for k,v in ipairs(cvars) do
                        if v == cmd then
                            if param == nil then
                                gi.cprintf(client, PRINT_HIGH, cmd.." = \""..gi.cvar(cmd).."\"\n")
                            else
                                if(gi.cvar_set(cmd, param)) then
                                    gi.cprintf(client, PRINT_HIGH, cmd.." -> \""..param.."\"\n")
                                else
                                    gi.cprintf(client, PRINT_HIGH, cmd.." -> \""..param.."\" (latched)\n")
                                end
                                gi.bprintf(PRINT_HIGH, players[client].name.." changed server settings: "..cmd.." -> \""..param.."\"\n")
                            end
                            return true
                        end
                    end

                    if cmd == 'status' then
			local maxclients = tonumber(gi.cvar("maxclients"))

			gi.cprintf(client, PRINT_HIGH, "num  name             address\n")
			gi.cprintf(client, PRINT_HIGH, "---  ---------------  ---------------\n")

			for i=1,maxclients do
			    if players[i].inuse then
				gi.cprintf(client, PRINT_HIGH, string.format("%3d  %-15s  %s\n", i-1, players[i].name, string.match(players[i].ip, '^([^:]+)')))
			    end
			end
                        return true
                    end

                    if cmd == 'sv' and gi.argc() > 2 then
                        cmd = gi.argv(2)
                        param = gi.argv(3)

                        if cmd == 'softmap' then
                            if param == nil or string.len(param) < 1 then
                                gi.cprintf(client, PRINT_HIGH, "Usage: sv softmap <mapname>\n")
                            else
                                gi.AddCommandString("sv softmap "..param)
                            end
                            return true
                        end

                        if cmd == 'stuffcmd' then
                            if param == nil or string.len(param) < 1 then
                                gi.cprintf(client, PRINT_HIGH, "Usage: sv stuffcmd <client> <command>\n")
                            else
                                local rest = ''
                                for i=4,gi.argc() do
                                    rest = rest..gi.argv(i).." "
                                end
                                gi.AddCommandString("sv stuffcmd "..param.." "..rest)
                            end
                            return true
                        end
                    end

                    if cmd == 'map' then
                        if param == nil then
                            gi.cprintf(client, PRINT_HIGH, "Usage: map <mapname>\n")
                        else
                            gi.AddCommandString("map "..param)
                        end
                        return true
                    end

                    if cmd == 'gamemap' then
                        if param == nil then
                            gi.cprintf(client, PRINT_HIGH, "Usage: gamemap <mapname>\n")
                        else
                            gi.AddCommandString("gamemap "..param)
                        end
                        return true
                    end

                    if cmd == 'kick' then
			if param == nil then
                            gi.cprintf(client, PRINT_HIGH, "Usage: kick <id>\n")
			else
			    gi.AddCommandString("kick "..tonumber(param))
			end
                        return true
                    end
                else
                    gi.cprintf(client, PRINT_HIGH, "You have not claimed the server yet.\n")
                    return true
                end

                gi.cprintf(client, PRINT_HIGH, "Unknown or disallowed lrcon command: \""..cmd.."\"\n")
                gi.cprintf(client, PRINT_HIGH, "Type \"lrcon help\" for more information.\n")
            end
        end
        return true
    end

    return false
end

function ClientConnect(client)
    local plr = players[client]

    if players[client].name..players[client].ip == claimer_store then
        claimer_store = nil
        claimer = client
    end
end

-- quit server after a match to reset state
function ClientDisconnect(client)
    local numplrs = 0
    local maxclients = tonumber(gi.cvar("maxclients"))

    for i=1,maxclients do
	if players[i].inuse then
	    numplrs = numplrs + 1
	end
    end

    if quit_on_empty and numplrs == 1 then
        gi.AddCommandString("quit")
    end

    if client == claimer then
        claimer = nil
    end
end
