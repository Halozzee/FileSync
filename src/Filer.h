#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <locale.h>

namespace fs = std::filesystem;

class Filer
{
public:
	Filer(fs::path root);
	~Filer();

	fs::path root() 
	{
		return r;
	}
	fs::path next() 
	{
		if (is_end())
			return fs::path();
		return pathes[counter++];
	}
	std::wstring next_cutted()
	{
		if (is_end())
			return std::wstring();
		return cutted_pathes[counter++];
	}
	bool is_end() 
	{
		if (counter == pathes.size())
			return true;
		return false;
	}
	void reset()
	{
		counter = 0;
	}
	bool cutted_exists(std::wstring p)
	{
		return std::find(cutted_pathes.begin(), cutted_pathes.end(), p) == cutted_pathes.end() ? false : true;
	}
	std::wstring cutted_from_notcutted(fs::path p) 
	{
		for (size_t i = 0; i < pathes.size(); i++)
			if (pathes[i] == p)
				return cutted_pathes[i];
		return L"";
	}
	fs::path notcutted_from_cutted(std::wstring p)
	{
		for (size_t i = 0; i < pathes.size(); i++)
			if (cutted_pathes[i] == p)
				return pathes[i];
		return fs::path();
	}
	fs::path compute_file_path(std::wstring p)
	{
		return fs::path(r.wstring() + p);
	}
private:
	// Счетчик для возврата следующего файла
	size_t counter = 0;
	// Путь к root папке
	fs::path r;
	// Список полных путей к файлам
	std::vector<fs::path> pathes;
	// Список путей к файлам относительно папки синхронизации
	std::vector<std::wstring> cutted_pathes;
	// Рекурсивная функция получения всех файлов по пути
	std::vector<fs::path> collect_files(fs::path dir_path)
	{
		std::vector<fs::path> file_pathes;

		for (auto& shrd : fs::directory_iterator(dir_path))
		{
			if (!shrd.is_directory())
				file_pathes.push_back(shrd.path());
			else
			{
				std::vector<fs::path> val = collect_files(shrd);

				for (size_t i = 0; i < val.size(); i++)
					file_pathes.push_back(val[i]);
			}
		}

		return file_pathes;
	};
	// Заполнить список путей к файлам относительно папки синхронизации
	void cut_pathes()
	{
		for (size_t i = 0; i < pathes.size(); i++)
		{
			std::wstring pp = get_dirs_between(r, pathes[i]) + L"\\" + pathes[i].filename().wstring();
			cutted_pathes.push_back(pp);
		}
	}
};

Filer::Filer(fs::path root)
{
	r = root;
	pathes = collect_files(r);
	cut_pathes();
}

Filer::~Filer()
{
}