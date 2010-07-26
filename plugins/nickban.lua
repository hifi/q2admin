local banned = { "Player", "nameless jErk" }

function ClientConnect(client)
    local plr = players[client]
    for k,v in pairs(banned) do
        if v == plr.name then
            return false, "This nick is banned, please use another one."
        end
    end

    return true
end
