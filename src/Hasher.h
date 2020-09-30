#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <boost/asio.hpp>
#include <boost/functional/hash.hpp>
#include <openssl/sha.h>
#include "Functions.h"
#include "Filer.h"

namespace fs = std::filesystem;

class Hasher
{
public:
    Hasher(Filer f) 
    {
        while (!f.is_end())
            add_file(f.root(), f.next());
    }
	void add_file(fs::path r, fs::path p);
    std::string get_file_hash(std::wstring p);

private:
	// Содержит следующий путь: папки, в которых лежит файл относительно указанной для синхронизации т.е.
	// Если для синхронизации папка C:/abs/ и она содержит папки aa, bb и cc, то файл лежащий в папке bb будет записан как
	//			bb/filename.fileextention (Следить за слешами)
	std::vector<std::wstring> pathes;
	std::vector<std::string>  SHA256HASHES;
    std::string SHA256(fs::path s_path);
};

// Внести файл вычислить его хэш и записать в массивы
void Hasher::add_file(fs::path r, fs::path p)
{
    std::wstring pp = get_dirs_between(r, p) + L"\\" + p.filename().wstring();

    if (std::find(pathes.begin(), pathes.end(), pp) != pathes.end())
        return;

    pathes.push_back(pp);
    SHA256HASHES.push_back(SHA256(p));
}

// Получение хэша файла по его пути относительно папки синхронизации
std::string Hasher::get_file_hash(std::wstring p)
{
    for (size_t i = 0; i < pathes.size(); i++)
        if (pathes[i] == p)
            return SHA256HASHES[i];
    return "";
}

std::string Hasher::SHA256(fs::path s_path)
{
    std::ifstream fp(s_path, std::ios::in | std::ios::binary);

    if (!fp.good())
    {
        std::ostringstream os;
        os << "Cannot open \"" << s_path << "\": " << std::strerror(errno) << ".";
        throw std::runtime_error(os.str());
    }

    constexpr const std::size_t buffer_size{ 1 << 12 };
    char buffer[buffer_size];

    unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    while (fp.good())
    {
        fp.read(buffer, buffer_size);
        SHA256_Update(&ctx, buffer, fp.gcount());
    }

    SHA256_Final(hash, &ctx);
    fp.close();

    std::ostringstream os;
    os << std::hex << std::setfill('0');

    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        os << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }

    return os.str();
}