#pragma once
#include <Windows.h>
#include <stdarg.h>
#include "Functions.h"
#include "Defines.h"
#include "Pather.h"
#include "Enums.h"
#include "Prioritizer.h"
#include <iomanip>

struct colored_words
{
	colored_words(size_t c, std::wstring w) : color(c), words(w)
	{

	}

	size_t color = 7;
	std::wstring words;
};
static class Interface
{
public:
	Interface();
	~Interface();
	static const int tab_size = 60;
	static void set_color(size_t attribute = 7);
	static void print_colored_text(int num, ...);
	static conflict_action ask_conflict(std::wstring from, std::wstring to, fs::path fp, fs::path tp)
	{
		auto fpair = get_file_info(fp);
		auto tpair = get_file_info(tp);
		colored_words header(HEADER_COLOR, L"\n###File:\t" + from + L"\n");
		colored_words fdata(FROM_COLOR, L"###FROM size:\t" + fpair.first + L"\twrite time:\t" + fpair.second + L"\n");
		colored_words tdata(TO_COLOR, L"###TO size:\t"   + tpair.first + L"\twrite time:\t" + tpair.second + L"\n");
		print_colored_text(3, header, fdata, tdata);
		std::wcout << std::endl << L"Choose option(";
		print_colored_text(8, colored_words(FROM_COLOR, L"Share FROM file(0)"), colored_words(BASE_COLOR, L"/"),
			colored_words(TO_COLOR, L"Share TO file(1)"), colored_words(BASE_COLOR, L"/"), colored_words(COPY_COLOR, L"Make copy(2)"), colored_words(BASE_COLOR, L"/"), colored_words(HEADER_COLOR, L"Skip(3)"), colored_words(BASE_COLOR, L"): "));

		std::wstring ws;
		std::getline(std::wcin, ws);

		if (ws == L"0")
			return share_from;
		else if (ws == L"1")
			return share_to;
		else if (ws == L"2")
			return copy;
		else if (ws == L"3")
			return skip;
	}
	static conflict_action ask_compliance(std::wstring from, std::wstring to, fs::path fp, fs::path tp)
	{
		colored_words header(HEADER_COLOR, L"\n###File that doesnt exist in FROM:\t" + from + L"\n");
		print_colored_text(1, header);
		std::wcout << std::endl << L"Choose option(";
		print_colored_text(6, colored_words(FROM_COLOR, L"Share File(0)"), colored_words(BASE_COLOR, L"/"),
			colored_words(ERROR_COLOR, L"Remove file(1)"), colored_words(BASE_COLOR, L"/"),
			 colored_words(SKIP_COLOR, L"Skip(2)"), colored_words(BASE_COLOR, L"): "));

		std::wstring ws;
		std::getline(std::wcin, ws);

		if (ws == L"0")
			return share_from;
		else if (ws == L"1")
			return share_to;
		else if (ws == L"2")
			return skip;
	}
	static void show_share_header(fs::path from, fs::path to) 
	{
		std::wcout << std::setw(tab_size) << std::setfill(L'=') << std::endl;
		std::wcout << L"Syncronizing: \n";
		print_colored_text(2, colored_words(FROM_COLOR, L"FROM: " + from.wstring() + L"\n"), colored_words(TO_COLOR, L"TO: " + to.wstring() + L"\n"));
		std::wcout << std::setw(tab_size) << std::setfill(L'=') << " " << std::endl;
		std::wcout << std::setfill(L' ');
	}
	static void show_compliance_header(fs::path from, fs::path to)
	{
		std::wcout << std::setw(tab_size) << std::setfill(L'=') << std::endl;
		std::wcout << L"Comparing files: \n";
		print_colored_text(2, colored_words(FROM_COLOR, L"FROM: " + from.wstring() + L"\n"), colored_words(TO_COLOR, L"TO: " + to.wstring() + L"\n"));
		std::wcout << std::setw(tab_size) << std::setfill(L'=') << " " << std::endl;
		std::wcout << std::setfill(L' ');

	}
	static void up_to_date(std::wstring from)
	{
		print_file_action_status(from, L"is up to date!", OK_COLOR);
	}
	static void added(std::wstring from)
	{
		print_file_action_status(from, L"added!", UPDATE_COLOR);
	}
	static void updated(std::wstring from)
	{
		print_file_action_status(from, L"Updated!", UPDATE_COLOR);
	}
	static void skipped(std::wstring from)
	{
		print_file_action_status(from, L"Skipped!", SKIP_COLOR);
	}
	static void copied(std::wstring from)
	{
		print_file_action_status(from, L"Copied!", COPY_COLOR);
	}
	static void deleted(std::wstring from)
	{
		print_file_action_status(from, L"Deleted!", ERROR_COLOR);
	}
	static void print_file_action_status(std::wstring from, std::wstring status, int color)
	{
		set_color(color);
		std::wcout << std::left << std::setw(tab_size) << from << std::right << status << std::endl;
		set_color(BASE_COLOR);
	}
	static load_from ask_loading()
	{
		std::wcout << "Load from History(0)/Ini file(1): ";
		std::wstring ws;
		std::getline(std::wcin, ws);

		if (ws == L"0")
		{
			std::wcout << "\nUsing History\n";
			return history;
		}
		else if (ws == L"1")
		{
			std::wcout << "\nUsing Ini file\n";
			return ini_file;
		}
	}
	static std::wstring ask_ini_path()
	{
		bool found = 0;
		std::wstring ws;
		while (!found)
		{
			std::wcout << "Write a path to an ini file: ";
			std::getline(std::wcin, ws);

			if (fs::exists(fs::path(ws)))
			{
				found = true;
				print_colored_text(1, colored_words(OK_COLOR, L"Ini file found!\n"));
			}
			else
				print_colored_text(1, colored_words(ERROR_COLOR, L"Ini file not found!\n"));
		}
		return ws;
	}
	static void show_history_sessions(Pather &p) 
	{
		auto hp = p.get_history();

		for (size_t i = 0; i < p.history_size; i++)
		{
			print_colored_text(1, colored_words(19, L"=================SESSION #" + std::to_wstring(i) + L"=================\n"));

			for (size_t j = 0; j < hp.first.size(); j++)
			{
				print_colored_text(1, colored_words(21, L"PAIR #" + std::to_wstring(j) + L":\n"));
				print_colored_text(1, colored_words(FROM_COLOR, L"FROM:\t" + hp.first[i][j].wstring() + L":\n"));
				print_colored_text(1, colored_words(TO_COLOR, L"TO:\t" + hp.second[i][j].wstring() + L":\n"));
			}
		}

		std::wcout << std::endl;
	}
	static size_t ask_num_history_session() 
	{
		std::wcout << L"Choose history session: ";
		std::wstring ws;
		std::getline(std::wcin, ws);
		return std::stoi(ws);
	}
	static sync_action ask_sync_action() 
	{
		std::wstring ws;
		std::wcout << "Make an Get to TO(0)/Get to FROM(1): ";
		std::getline(std::wcin, ws);

		if (ws == L"0")
			return update;
		else if (ws == L"1")
			return upload;
	}
	static Prioritizer ask_priorities()
	{
		conflict_action share = none;
		conflict_action compliance = none;

		std::wcout << std::endl;
		std::wcout << L"Share priority: ";
		print_colored_text(9, colored_words(FROM_COLOR, L"Share FROM(0)"),
			colored_words(BASE_COLOR, L"/"), colored_words(TO_COLOR, L"Share TO(1)"),
			colored_words(BASE_COLOR, L"/"), colored_words(COPY_COLOR, L"Make copy(2)"),
			colored_words(BASE_COLOR, L"/"), colored_words(SKIP_COLOR, L"Skip(3)"),
			colored_words(BASE_COLOR, L"/"), colored_words(BASE_COLOR, L"Ask(4):"));

		std::wstring ws;
		std::getline(std::wcin, ws);

		std::wcout << std::endl;

		switch (std::stoi(ws))
		{
		case 0:
			share = share_from;
			break;
		case 1:
			share = share_to;
			break;
		case 2:
			share = copy;
			break;
		case 3:
			share = skip;
			break;
		case 4:
			share = none;
			break;
		default:
			break;
		}

		std::wcout << L"Compliance priority ";
		print_colored_text(5, colored_words(FROM_COLOR, L"Share(0)"),
			colored_words(BASE_COLOR, L"/"), colored_words(SKIP_COLOR, L"Skip(1)"),
			colored_words(BASE_COLOR, L"/"), colored_words(BASE_COLOR, L"Ask(2):"));

		std::getline(std::wcin, ws);
		switch (std::stoi(ws))
		{
		case 0:
			compliance = share_from;
			break;
		case 1:
			compliance = skip;
			break;
		case 2:
			compliance = none;
			break;
		default:
			break;
		}

		std::wcout << std::endl;

		return Prioritizer(share, compliance);
	};
	static bool ask_for_global_priorities_use() 
	{
		std::wcout << "Use priority settings for all paths?(0/1): ";
		std::wstring ws;
		std::getline(std::wcin, ws);
		std::wcout << std::endl;

		return ws == L"0" ? false : true;
	}
private:
};

Interface::Interface()
{
}

Interface::~Interface()
{
}
//установка цвета с указанным параметром
void Interface::set_color(size_t attribute)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attribute);
}

// Вывод текста с цветом передается число colored_words для вывода и colored_words через запятую
void Interface::print_colored_text(int num, ...)
{
	va_list argptr;
	va_start(argptr, num);

	for (size_t i = 0; i < num; i++)
	{
		colored_words t = va_arg(argptr, colored_words);
		set_color(t.color);
		std::wcout << t.words;
	}
	set_color(BASE_COLOR);
	va_end(argptr);
}