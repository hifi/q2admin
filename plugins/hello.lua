function q2a_load()
	gi.dprintf("hello.lua: q2a_load()\n")
end

function q2a_unload()
	gi.dprintf("hello.lua: q2a_unload()\n")
end

function ClientConnect(client)
	local plr = players[client]
	gi.dprintf("hello.lua: ClientConnect("..client..")\n")
	gi.dprintf("hello.lua: "..plr.name.."@"..plr.ip_str.." is connecting\n")
	return true
end

function ClientBegin(client)
	local plr = players[client]
	gi.dprintf("hello.lua: ClientBegin("..client..")\n")
	gi.dprintf("hello.lua: "..plr.name.."@"..plr.ip_str.." joined the game\n")
end

function ClientDisconnect(client)
	local plr = players[client]
	gi.dprintf("hello.lua: ClientDisconnect("..client..")\n")
	gi.dprintf("hello.lua: "..plr.name.."@"..plr.ip_str.." disconnected\n")
end

function ClientNameChanged(client, new_name)
	gi.dprintf("hello.lua: ClientNameChanged("..client..", "..new_name..")\n")
end

function ClientSkinChanged(client, new_skin)
	gi.dprintf("hello.lua: ClientSkinChanged("..client..", "..new_skin..")\n")
end

function RunFrame()
	--gi.dprintf("hello.lua: RunFrame()\n")
end

function ClientThink(client)
	--gi.dprintf("hello.lua: ClientThink()\n")
end

function ClientCommand(client, cmd)
	gi.dprintf("hello.lua: ClientCommand("..client..", "..cmd..")\n")

	if cmd == "hello" then
		if gi.argc() > 1 then
			local name = gi.argv(1)
			gi.cprintf(client, PRINT_HIGH, "hello.lua: Hello, "..name.."!\n")
		else
			gi.cprintf(client, PRINT_HIGH, "hello.lua: usage: !hello <name>\n")
		end

		return true
	end

	if cmd == "ping" then
		gi.centerprintf(client, "Pong!")
		return true
	end

	if cmd == "kickme" then
		gi.cprintf(client, PRINT_HIGH, "Kick requested.\n")
		gi.AddCommandString("\nkick "..client.."\n")
		return true
	end

	return false
end

function ServerCommand(cmd)
	gi.dprintf("hello.lua: ServerCommand("..cmd..")\n")

	if cmd == "hello" then
		if gi.argc() > 2 then
			local name = gi.argv(2)
			gi.dprintf("hello.lua: Hello, "..name.."!\n")
		else
			gi.dprintf("hello.lua: usage: !hello <name>\n")
		end

		return true
	end

	return false
end

gi.dprintf("hello.lua: plugin loaded\n")
