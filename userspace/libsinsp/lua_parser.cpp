#include <iostream>
#include <fstream>
#include "sinsp.h"
#include "filter.h"
#include "sinsp_int.h"

#include "lua_parser.h"
#include "lua_parser_api.h"


extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

const static struct luaL_reg ll_filter [] =
{
	{"rel_expr", &lua_parser_cbacks::rel_expr},
	{"bool_op", &lua_parser_cbacks::bool_op},
	{"nest", &lua_parser_cbacks::nest},
	{"unnest", &lua_parser_cbacks::unnest},
	{NULL,NULL}
};

lua_parser::lua_parser(lua_filter_factory *factory, lua_State *ls, const char *lua_global_binding)
{
	m_factory = factory;

	m_ls = ls;
	reset();

	// Register our c++ defined functions
	luaL_openlib(m_ls, lua_global_binding, ll_filter, 0);

	lua_pushlightuserdata(m_ls, this);
	lua_setglobal(m_ls, "siparser");

}

void lua_parser::reset()
{
	m_have_rel_expr = false;
	m_last_boolop = BO_NONE;
	m_nest_level = 0;

	m_filter = m_factory->new_filter();
}

gen_event_filter* lua_parser::get_filter(bool reset_filter)
{
	if (m_nest_level != 0)
	{
		throw sinsp_exception("Error in configured filter: unbalanced nesting");
	}

	gen_event_filter *ret = m_filter;

	if (reset_filter)
	{
		reset();
	}

	return ret;
}
lua_parser::~lua_parser()
{
	// The lua state is not considered owned by this object, so
	// not freeing it.

	delete m_filter;
}


