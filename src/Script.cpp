#include "Script.hpp"

void Script::err(const std::string& name, const std::string& reason){
	throw std::string("ERROR: cannot get " + name + " from script: " + reason);
}

void Script::cleanStack(){
	int n = lua_gettop(L);
	lua_pop(L, n);
}

void Script::getStack(const std::string& name){
	std::string var = "";
	for(size_t i = 0; i < name.size(); i++){
		if(name[i] == '.'){
			if(level == 0)
				lua_getglobal(L, var.c_str());
			else
				lua_getfield(L, -1, var.c_str());
			if(lua_isnil(L, -1))
				err(name, "variable is not defined");
			var = "";
			level++;
			deltaLevel++;
		}
		else if(name[i] == '['){
			if(var != ""){
				if(level == 0)
					lua_getglobal(L, var.c_str());
				else
					lua_getfield(L, -1, var.c_str());
				if(not lua_istable(L, -1))
					err(var, "not a table");
				var = "";
				level++;
				deltaLevel++;
			} else if(var == "" and level == 0)
				err(name, "name is ill-formated, '[' at gloval scope");
			
			i++;
			int id = 0;
			while(i < name.size() and name[i] != ']'){
				id *= 10;
				id += name[i] - '0';
				i++;
			}
			if(i == name.size())
				err(name, "name is ill-formated, lack of ']'");
			if(i + 1 < name.size() and name[i + 1] == '.')
				i++;
			lua_rawgeti(L, -1, id);
			if(lua_isnil(L, -1))
				err(name, "indexed variable is not defined");
			if(lua_istable(L, -1)){
				level++;
				deltaLevel++;
			}
		}
		else
			var += name[i];
	}
	if(var != ""){
		if(level == 0)
			lua_getglobal(L, var.c_str());
		else
			lua_getfield(L, -1, var.c_str());
		if(lua_isnil(L, -1))
			err(name, "variable is not defined");
	}
}

void Script::openTable(const std::string& name){
	if(L == nullptr)
		err(name, "script is not loaded");
	std::string var = "";
	for(size_t i = 0; i < name.size(); i++){
		if(name[i] == '.'){
			if(level == 0)
				lua_getglobal(L, var.c_str());
			else
				lua_getfield(L, -1, var.c_str());
			if(not lua_istable(L, -1))
				err(var, "table is not defined");
			var = "";
			level++;
		}
		else if(name[i] == '['){
			if(var != ""){
				if(level == 0)
					lua_getglobal(L, var.c_str());
				else
					lua_getfield(L, -1, var.c_str());
				if(not lua_istable(L, -1))
					err(var, "not a table");
				var = "";
				level++;
			} else if(var == "" and level == 0)
				err(name, "name is ill-formated, '[' at global scope");
			
			i++;
			int id = 0;
			while(i < name.size() and name[i] != ']'){
				id *= 10;
				id += name[i] - '0';
				i++;
			}
			if(i == name.size())
				err(name, "name is ill-formated, lack of ']'");
			if(i + 1 < name.size() and name[i + 1] == '.')
				i++;
			lua_rawgeti(L, -1, id);
			if(not lua_istable(L, -1))
				err(name, "indexed variable is not defined");
			level++;
		}
		else
			var += name[i];
	}
	if(var != ""){
		if(level == 0)
			lua_getglobal(L, var.c_str());
		else
			lua_getfield(L, -1, var.c_str());
		if(not lua_istable(L, -1))
			err(var, "table is not defined");
		level++;
	}
}

void Script::openTable(const int id){
	if(L != nullptr){
		if(level == 0)
			throw std::string("ERROR: cannot open indexed table at global scope");
		lua_rawgeti(L, -1, id);
		if(not lua_istable(L, -1))
			throw std::string("ERROR: cannot open indexed table: not a table");
		level++;
	}
}

void Script::closeTable(const int layers){
	lua_pop(L, layers);
	level -= layers;
}

Script::Script(const std::string& script){
	L = luaL_newstate();
	luaL_openlibs(L);
	m_funFlag = false;
	load(script);
}

void Script::load(const std::string& script){
	if(luaL_loadfile(L, script.c_str()) or lua_pcall(L, 0, 0, 0))
		throw std::string("ERROR: cannot load script " + script + ". No such file.");
	deltaLevel = level = 0;
}

Script::Script(){
	L = luaL_newstate();
	luaL_openlibs(L);
	m_funFlag = false;
}

Script::~Script(){
	lua_close(L);
}

void Script::addFunction(Lua_callback_function function, const std::string& name){
	lua_pushcfunction(L, function);
	lua_setglobal(L, name.c_str());
}

void Script::postCall(int res){
	m_funFlag = false;
	if(res){
		throw std::string(lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

void Script::preCall(const std::string& name){
	if(not m_funFlag){
		if(not L)
			err(name, "script is not loaded");
		if(lua_getglobal(L, name.c_str()) != LUA_TFUNCTION)
			err(name, "not a function");
		m_funFlag = true;
	}
}
