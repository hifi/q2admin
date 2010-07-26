local server = { }
server[27910] = "aq2.pelitutka.fi:27920"
server[27920] = "aq2.pelitutka.fi:27910"

-- simple hack to tell in ClientBegin if this is a new connecting client
local is_connecting = { }
function ClientConnect(client)
    is_connecting[client] = true
end

function ClientDisconnect(client)
    is_connecting[client] = nil
end

function ClientBegin(client)
	local plr = players[client]
        local numplrs = 0
        local maxclients = tonumber(gi.cvar("maxclients"))
        local port = tonumber(gi.cvar("port"))

        for i=1,maxclients do
            if players[i].inuse then
                numplrs = numplrs + 1
            end
        end

        if numplrs == maxclients and is_connecting[client] == true and type(server[port]) == "string" then
            gi.cprintf(client, PRINT_HIGH, "This server is full, forwarding to "..server[port]..".\n")
            stuffcmd(client, "connect "..server[port].."\n")
        end

        is_connecting[client] = nil
end
