--
-- q2admin
--
-- q2a_lua_plugman.lua
--
-- copyright 2009 Toni Spets
--

--
-- Q2Admin Lua plugin manager
--

-- not a complete deep copy, metatables are ignored
local function copy_table(orig)
	local new = {}

	for k,v in pairs(orig) do
		new[k] = v
	end

	return new
end

local function q2a_plugin_call(plugin, func, ...)
	if type(plugin.env[func]) ~= 'function' then
		return
	end

	success, err = pcall(plugin.env[func], ...)
	if not success then
		gi.dprintf("Q2A Lua: failed to call '"..func.."' in '"..plugin.file.."': "..err.."\n")
		if not plugin.unloading then
			q2a_unload(plugin.file)
		end

		return false
	end

	return err
end

cfg = {}
local globals = {}
local plugins = {}

function q2a_init()
	gi.dprintf("Q2A Lua: Plugin Manager\n")
	gi.dprintf("Q2A Lua: Loading configuration config.lua\n");

	globals = copy_table(_G)
	globals.q2a_init = nil
	globals.q2a_shutdown = nil
	globals.q2a_load = nil
	globals.q2a_unload = nil
	globals.q2a_reload = nil
	globals.q2a_call = nil

	chunk, err = loadfile("config.lua")
	if chunk == nil then
		gi.dprintf("Q2A Lua: failed to load configuration from config.lua (not fatal): "..tostring(err).."\n")
		return
	else
		setfenv(chunk, cfg)
		success, err = pcall(chunk)
		if not success then
			gi.dprintf("Q2A Lua: syntax error in config:"..tostring(err).."\n")
			return
		end
	end

	if type(cfg.plugins) == 'table' then
		for k,v in pairs(cfg.plugins) do
			q2a_load("plugins/"..tostring(v)..".lua")
		end
	end
end

function q2a_shutdown()
	gi.dprintf("Q2A Lua: shutdown in progress...\n")
	for i,plugin in pairs(plugins) do
		q2a_unload(plugin.file)
	end
	gi.dprintf("Q2A Lua: all plugins unloaded!\n")
end

function q2a_load(file)
	local success, err, chunk

	local plugin = {}
	plugin.file = file

	chunk, err = loadfile(plugin.file)
	if chunk == nil then
		gi.dprintf("Q2A Lua: failed to load file "..plugin.file..": "..tostring(err).."\n")
		return false
	end

	plugin.env = copy_table(globals)
	setfenv(chunk, plugin.env)
	success, err = pcall(chunk)

	if not success then
		gi.dprintf("Q2A Lua: failed to compile file "..plugin.file..": "..tostring(err).."\n")
		return false
	end

	gi.dprintf("Q2A Lua: loaded plugin "..plugin.file.."\n")
	q2a_plugin_call(plugin, 'q2a_load')

	table.insert(plugins, plugin)
	return true
end

function q2a_unload(file)
	for i,plugin in pairs(plugins) do
		if plugin.file == file then
			gi.dprintf("Q2A Lua: unloading "..file.."\n")
			plugin.unloading = true
			-- let the plugin know we are unloading it
			q2a_plugin_call(plugin, 'q2a_unload')
			plugins[i] = nil
			return true
		end
	end

	return false
end

function q2a_reload(file)
	q2a_unload(file)
	q2a_load(file)
end

function q2a_call(func, ...)
	for i,plugin in pairs(plugins) do
		q2a_plugin_call(plugin, func, ...)
	end
end

function q2a_call_bool(func, def, ...)
	for i,plugin in pairs(plugins) do
		local ret = q2a_plugin_call(plugin, func, ...)
		if ret ~= def then
			return ret
		end
	end

	return def
end
