#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <codecvt>

namespace fs = std::filesystem;

std::string utf16_to_utf8(const std::wstring& wstr)
{
    std::string retStr;
    if (!wstr.empty())
    {
        int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

        if (sizeRequired > 0)
        {
            std::vector<char> utf8String(sizeRequired);
            int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
                -1, &utf8String[0], utf8String.size(), NULL,
                NULL);
            if (bytesConverted != 0)
            {
                retStr = &utf8String[0];
            }
            else
            {
                std::stringstream err;
                err << __FUNCTION__
                    << " std::string WstrToUtf8Str failed to convert wstring '"
                    << wstr.c_str() << L"'";
                throw std::runtime_error(err.str());
            }
        }
    }
    return retStr;
}

std::vector<std::wstring> utf16_to_utf8_vector(std::vector<std::wstring> v)
{
    std::vector<std::wstring> rtn;
    for (size_t i = 0; i < v.size(); i++)
    {
        std::string buf;
        buf = utf16_to_utf8(v[i]);
        rtn.push_back(std::wstring(buf.begin(), buf.end()));
    }
    return rtn;
}

//конвертация кодировки utf8 в utf16 для wstring
std::wstring utf8_to_utf16(const std::wstring& utf8)
{
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size())
    {
        unsigned long uni;
        size_t todo;
        bool error = false;
        unsigned char ch = utf8[i++];
        if (ch <= 0x7F)
        {
            uni = ch;
            todo = 0;
        }
        else if (ch <= 0xBF)
        {
            throw std::logic_error("not a UTF-8 string");
        }
        else if (ch <= 0xDF)
        {
            uni = ch & 0x1F;
            todo = 1;
        }
        else if (ch <= 0xEF)
        {
            uni = ch & 0x0F;
            todo = 2;
        }
        else if (ch <= 0xF7)
        {
            uni = ch & 0x07;
            todo = 3;
        }
        else
        {
            throw std::logic_error("not a UTF-8 string");
        }
        for (size_t j = 0; j < todo; ++j)
        {
            if (i == utf8.size())
                throw std::logic_error("not a UTF-8 string");
            unsigned char ch = utf8[i++];
            if (ch < 0x80 || ch > 0xBF)
                throw std::logic_error("not a UTF-8 string");
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            throw std::logic_error("not a UTF-8 string");
        if (uni > 0x10FFFF)
            throw std::logic_error("not a UTF-8 string");
        unicode.push_back(uni);
    }
    std::wstring utf16;
    for (size_t i = 0; i < unicode.size(); ++i)
    {
        unsigned long uni = unicode[i];
        if (uni <= 0xFFFF)
        {
            utf16 += (wchar_t)uni;
        }
        else
        {
            uni -= 0x10000;
            utf16 += (wchar_t)((uni >> 10) + 0xD800);
            utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
        }
    }
    return utf16;
}

//Функция возвращающая промежуток в виде папок от корневой до имени файла
std::wstring get_dirs_between(fs::path root, fs::path file)
{
    std::wstring root_s = root.wstring();
    std::wstring file_s = file.wstring();
    std::wstring filename = file.filename().wstring();

    size_t r_pos = root.string().size();
    size_t fn_pos = file_s.find(filename);

    file_s.erase(file_s.begin() + fn_pos, file_s.end());
    file_s.erase(file_s.begin(), file_s.begin() + r_pos);

    if (file_s == L"\\")
        file_s = L"";

    return file_s;
}

//Функция для вставки папок в путь
fs::path set_dirs_between(fs::path root, fs::path file, std::wstring dirs_btwn)
{
    std::wstring root_s = root.wstring();
    std::wstring file_s = file.wstring();
    std::wstring filename = file.filename().wstring();

    return fs::path(root_s + dirs_btwn + filename);
}

//получение времени записи файлов
std::time_t get_file_write_time(const std::filesystem::path& filename)
{
#if defined ( _WIN32 )
    {
        struct _stat64 fileInfo;
        if (_wstati64(filename.wstring().c_str(), &fileInfo) != 0)
        {
            throw std::runtime_error("Failed to get last write time.");
        }
        return fileInfo.st_mtime;
    }
#else
    {
        auto fsTime = std::filesystem::last_write_time(filename);
        return decltype (fsTime)::clock::to_time_t(fsTime);
    }
#endif
}

//получение информации о файле
std::pair<std::wstring, std::wstring> get_file_info(fs::path file)
{
    uintmax_t size = file_size(file);
    time_t wt = get_file_write_time(file);

    char* write_time = std::asctime(std::localtime(&wt));

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;

    std::wstring wstring_write_time = convert.from_bytes(std::string(write_time));
    wstring_write_time.erase(wstring_write_time.begin() + wstring_write_time.size() - 1);
    std::wstring wstring_size = std::to_wstring(size);

    return std::make_pair(wstring_size, wstring_write_time);
}