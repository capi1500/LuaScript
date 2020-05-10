/******************************************************************************
* Copyright (C) 2020 Kacper Chętkowski.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifndef LUASCRIPT_SCRIPT_HPP
#define LUASCRIPT_SCRIPT_HPP

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <string>

typedef int (* Lua_callback_function)(lua_State* L);

class Script{
	private:
		lua_State* L;
		
		template<typename T> void pushToLua(const T& var){
			if constexpr (std::is_same_v<T, int> or std::is_same_v<T, long> or std::is_same_v<T, long long> or std::is_same_v<T, unsigned> or std::is_same_v<T, unsigned long> or std::is_same_v<T, unsigned long long>)
				lua_pushinteger(L, var);
			else if constexpr (std::is_same_v<T, float> or std::is_same_v<T, double> or std::is_same_v<T, long double>)
				lua_pushnumber(L, var);
			else if constexpr (std::is_same_v<T, bool>)
				lua_pushboolean(L, var);
			else if constexpr (std::is_same_v<T, std::string>)
				lua_pushstring(L, var.c_str());
		}
		
		template<typename T> T getFromLua(const std::string& var){
			if constexpr (std::is_same_v<T, int> or std::is_same_v<T, long> or std::is_same_v<T, long long> or std::is_same_v<T, unsigned> or std::is_same_v<T, unsigned long> or std::is_same_v<T, unsigned long long>){
				if(!lua_isnumber(L, -1))
					err(var, "not a number");
				return lua_tointeger(L, -1);
			} else if constexpr (std::is_same_v<T, float> or std::is_same_v<T, double> or std::is_same_v<T, long double>){
				if(!lua_isnumber(L, -1))
					err(var, "not a number");
				return lua_tonumber(L, -1);
			} else if constexpr (std::is_same_v<T, bool>){
				return lua_toboolean(L, -1);
			} else if constexpr (std::is_same_v<T, std::string>){
				return lua_tostring(L, -1);
			}
		}
		
		void err(const std::string& name, const std::string& reason);
		void getStack(const std::string& name);
		void cleanStack();
		void preCall(const std::string& name);
		void postCall(int res);
		
		bool m_funFlag;
		
		int level;
		int deltaLevel;
	public:
		Script(const std::string& script);
		Script();
		~Script();
		void load(const std::string& script);
		
		template<typename T> void addVar(const T& var, const std::string& name){
			if(not L)
				err(name, "script is not loaded");
			pushToLua(var);
			lua_setglobal(L, name.c_str());
		}
		
		void addFunction(Lua_callback_function function, const std::string& name);
		
		template<typename R> R callFunction(const std::string& name){
			preCall(name);
			postCall(lua_pcall(L, 0, 1, 0));
			R r = getFromLua<R>("");
			lua_pop(L, 1);
			return r;
		}
		
		template<typename R, unsigned size, class T> R callFunction(const std::string& name, T arg){
			preCall(name);
			pushToLua(arg);
			postCall(lua_pcall(L, size, 1, 0));
			R r = getFromLua<R>("");
			lua_pop(L, 1);
			return r;
		}
		
		template<typename R, unsigned size, class T, class... Ts> R callFunction(const std::string& name, T arg, Ts... args){
			preCall(name);
			pushToLua(arg);
			return callFunction<R, size>(name, args...);
		}
		
		void callFunction(const std::string& name){
			preCall(name);
			postCall(lua_pcall(L, 0, 0, 0));
		}
		
		template<unsigned size, class T> void callFunction(const std::string& name, T arg){
			preCall(name);
			pushToLua(arg);
			postCall(lua_pcall(L, size, 0, 0));
		}
		
		template<unsigned size, class T, class... Ts> void callFunction(const std::string& name, T arg, Ts... args){
			preCall(name);
			pushToLua(arg);
			callFunction<size>(name, args...);
		}
		
		void openTable(const std::string& name);
		void openTable(const int id);
		void closeTable(const int layers = 1);
		
		template<typename T> T get(const std::string& name){
			T out = T();
			if(not L){
				err(name, "script is not loaded");
				return out;
			}
			getStack(name);
			out = getFromLua<T>(name);
			lua_pop(L, deltaLevel + 1);
			level -= deltaLevel;
			deltaLevel = 0;
			return out;
		}
		
		template<typename T> T get(const int id){
			return get<T>("[" + std::to_string(id) + "]");
		}
		
		template<typename T> T get(const std::string& table, const int id){
			return get<T>(table + "[" + std::to_string(id) + "]");
		}
};

#endif //LUASCRIPT_SCRIPT_HPP
