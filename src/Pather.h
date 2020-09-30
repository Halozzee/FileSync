#pragma once
#include "Interface.h"
#include "IniOperator.h"

class Pather
{
private:
	fs::path history_file = "history.data";
	fs::path ini_file;

	std::vector<std::vector<fs::path>> history_from;
	std::vector<std::vector<fs::path>> history_to;

	std::vector<fs::path> from;
	std::vector<fs::path> to;

	size_t from_cntr = 0;
	size_t to_cntr = 0;

public:
	size_t history_size = 0;
	Pather();
	~Pather();
	fs::path next_from()
	{
		if (!is_end())
			return from[from_cntr++]; 
		else
			return fs::path();
	}
	fs::path next_to()
	{
		if (!is_end())
			return to[to_cntr++];
		else
			return fs::path();
	}
	std::vector<std::wstring> path_vector_to_wstring_vector(std::vector<fs::path> v)
	{
		std::vector<std::wstring> r;
		for (size_t i = 0; i < v.size(); i++)
			r.push_back(v[i].wstring());
		return r;
	}

	std::wstring ExePath() 
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
		return std::wstring(buffer).substr(0, pos);
	}

	void store_session() 
	{
		/*INI_WORKS::INI_CreateOperator co(ExePath() + L"\\" + history_file.wstring());
		std::wstring file;
		file = co.create_section(L"SESSION #" + std::to_wstring(history_size + 1));
		co.update_ini_file(file);
		file = co.create_multi_values(L"SESSION #" + std::to_wstring(history_size + 1), L"FROM", path_vector_to_wstring_vector(from));
		co.update_ini_file(file);
		file = co.create_multi_values(L"SESSION #" + std::to_wstring(history_size + 1), L"TO", path_vector_to_wstring_vector(to));
		co.update_ini_file(file);*/
	}

	bool is_end() 
	{
		return from_cntr == from.size() && to_cntr == to.size();
	}

	void update_history_metadata() 
	{
		/*INI_WORKS::INI_ReadOperator ro(ExePath() + L"\\" + history_file.wstring());

		std::wstring ws = ro.get_all_lines();
		size_t history_count = 0;

		while (ws.find(L"[SESSION #") != std::wstring::npos)
		{
			history_count++;
			ws.erase(ws.begin() + ws.find(L"[SESSION #"), ws.begin() + ws.find(L"[SESSION #") + std::wstring(L"[SESSION #").size());
		}

		history_size = history_count;*/
	}

	bool read_pathes_from_ini(fs::path ini)
	{
		update_history_metadata();
		ini_file = ini;
		INI_WORKS::INI_ReadOperator ro(ini_file);

		std::vector<std::wstring> v = ro.get_multi_values(L"FROM DIR", L"PATHES");
		for (size_t i = 0; i < v.size(); i++)
			from.push_back(fs::path(utf8_to_utf16(v[i])));

		v = ro.get_multi_values(L"TO DIR", L"PATHES");
		for (size_t i = 0; i < v.size(); i++)
			to.push_back(fs::path(utf8_to_utf16(v[i])));

		if (to.size() > 0 && from.size() > 0)
			return true;
		else 
			return false;
	}
	bool read_history() 
	{
		/*INI_WORKS::INI_ReadOperator ro(ExePath() + L"\\" + history_file.wstring());

		update_history_metadata();

		history_from.resize(history_size);
		history_to.resize(history_size);

		for (size_t j = 1; j <= history_size; j++)
		{
			std::vector<std::wstring> v = ro.get_multi_values(L"SESSION #" + std::to_wstring(j), L"FROM");
			for (size_t i = 0; i < v.size(); i++)
				history_from[i].push_back(fs::path(utf8_to_utf16(v[i])));

			v = ro.get_multi_values(L"SESSION #" + std::to_wstring(j), L"TO");
			for (size_t i = 0; i < v.size(); i++)
				history_to[i].push_back(fs::path(utf8_to_utf16(v[i])));
		}
		return true;*/
	}

	bool use_history_session(size_t i) 
	{/*
		if (i > history_size) 
			return false;

		if (history_from.size() == 0 || history_to.size() == 0)
			return false;

		from = history_from[i];
		to = history_to[i];*/
	}

	void swap_from_to() 
	{
		std::swap(from, to);
	}

	std::pair<std::vector<std::vector<fs::path>>, std::vector<std::vector<fs::path>>> get_history() 
	{
		/*return std::make_pair(history_from, history_to);*/
	}

	void reset() 
	{
		from_cntr = 0;
		to_cntr = 0;
	}
};

Pather::Pather()
{
}

Pather::~Pather()
{
}