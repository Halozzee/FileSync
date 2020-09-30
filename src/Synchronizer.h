#pragma once
#include <fstream>
#include <filesystem>
#include "Hasher.h"
#include "Filer.h"
#include "Interface.h"
#include "Prioritizer.h"

namespace fs = std::filesystem;

class Synchronizer
{
public:
	conflict_action file_share_priority;
	conflict_action file_compliance_priority;

	bool use_global_priorities = false;

	void ask_for_global_priorities() 
	{
		use_global_priorities = Interface::ask_for_global_priorities_use();
	}

	void ask_priorities() 
	{
		set_prioritizer(Interface::ask_priorities());
	}

	void set_prioritizer(Prioritizer p) 
	{
		auto priors = p.get_priority();
		file_share_priority = priors.first;
		file_compliance_priority = priors.second;
	}

	Synchronizer();
	~Synchronizer();

	void file_compliance(Filer& from, Filer& to)
	{
		Interface::show_compliance_header(from.root(), to.root());

		from.reset();
		to.reset();
		while (!to.is_end())
		{
			std::wstring crnt = to.next_cutted();
			if (!from.cutted_exists(crnt))
			{
				conflict_action act;
				if (file_compliance_priority == none)
					act = Interface::ask_compliance(crnt, crnt, from.notcutted_from_cutted(crnt), to.notcutted_from_cutted(crnt));
				else
					act = file_compliance_priority;
				solve_compliance(fs::path(from.root().wstring() + crnt), fs::path(to.root().wstring() + crnt), act, crnt);
			}
		}
	}

	void file_share(Filer &from, Filer &to) 
	{
		Interface::show_share_header(from.root(), to.root());

		from.reset();
		to.reset();
		Hasher from_hash(from);
		Hasher to_hash(to);
		from.reset();
		to.reset();

		while (!from.is_end())
		{
			std::wstring crnt = from.next_cutted();
			if (to.cutted_exists(crnt))
			{
				if (to_hash.get_file_hash(crnt) != from_hash.get_file_hash(crnt))
				{
					conflict_action act;
					if (file_share_priority == none)
						act = Interface::ask_conflict(crnt, crnt, from.notcutted_from_cutted(crnt), to.notcutted_from_cutted(crnt));
					else
						act = file_share_priority;

					solve_conflict(from.compute_file_path(crnt), to.compute_file_path(crnt), act, crnt);
				}
				else
					Interface::up_to_date(crnt);
			}
			else
				add_file(from.notcutted_from_cutted(crnt), to.compute_file_path(crnt), crnt);
		}
	}

private:
	void add_file(fs::path file, fs::path end)
	{
		std::wstring file_dir = end.wstring();
		file_dir.erase(file_dir.begin() + file_dir.find(end.filename().wstring()), file_dir.end());
		if (!fs::exists(fs::path(file_dir)))
			fs::create_directories(fs::path(file_dir));
		fs::copy(file, end);
	}
	void add_file(fs::path file, fs::path end, std::wstring crnt)
	{
		add_file(file, end);
		Interface::added(crnt);
	} 
	void delete_file(fs::path file, std::wstring crnt)
	{
		fs::remove(file);
		Interface::deleted(crnt);
	}
	void make_file_copy(fs::path from, fs::path to, std::wstring f)
	{
		fs::path iter_path = to;
		size_t counter = 0;
		while (fs::exists(iter_path))
		{
			std::wstring ws = iter_path.wstring();
			std::wstring n = iter_path.filename().wstring();
			n.erase(n.begin() + n.find(iter_path.extension().wstring()), n.end());
			size_t pos = ws.find(n) + n.size();
			std::wstring wc = std::to_wstring(counter);

			ws.insert(ws.begin() + pos, L')');

			for (size_t i = 0; i < wc.size(); i++)
				ws.insert(ws.begin() + pos, wc[wc.size() - 1 - i]);

			ws.insert(ws.begin() + pos, L'(');

			iter_path = fs::path(ws);
		}
		add_file(from, iter_path);
		Interface::copied(f);
	}
	void solve_conflict(fs::path from, fs::path to, conflict_action act, std::wstring ws)
	{
		switch (act)
		{
		case share_from:
			delete_file(to, ws);
			add_file(from, to);
			Interface::updated(ws);
			break;
		case share_to:
			delete_file(from, ws);
			add_file(to, from);
			Interface::updated(ws);
			break;
		case copy:
			make_file_copy(from, to, ws);
			break;
		case skip:
			Interface::skipped(ws);
			break;
		default:
			break;
		}
	}
	void solve_compliance(fs::path from, fs::path to, conflict_action act, std::wstring ws)
	{
		switch (act)
		{
		case share_from:
			add_file(to, from);
			Interface::updated(ws);
			break;
		case share_to:
			delete_file(to, ws);
			break;
		case skip:
			Interface::skipped(ws);
			break;
		default:
			break;
		}
	}
};

Synchronizer::Synchronizer()
{
	file_share_priority			= none;
	file_compliance_priority	= none;
}

Synchronizer::~Synchronizer()
{
}