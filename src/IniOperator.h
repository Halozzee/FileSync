#pragma once
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace INI_WORKS
{
	//Represents base that allows to format file separators standard quickly
	class INI_formater
	{
	public:
		wchar_t _INI_STARTSECTION_SEPARATOR = '[';
		wchar_t _INI_ENDSECTION_SEPARATOR = ']';
		wchar_t _INI_VALUE_SEPARATOR = '=';
		wchar_t _INI_MULTI_VALUE_SEPARATOR = ';';
	};

	//Represents base class for all of operators
	class INI_baseOperator
	{

	protected:
		//path to ini file
		std::wstring path_ = L"";
		wchar_t _INI_STARTSECTION_SEPARATOR;
		wchar_t _INI_ENDSECTION_SEPARATOR;
		//for more then one value
		wchar_t _INI_MULTI_VALUE_SEPARATOR;
		//means : yadayada = 123
		wchar_t _INI_VALUE_SEPARATOR;

	public:
		void remove_spaces(std::wstring &str);
		INI_baseOperator(std::wstring, wchar_t _INI_STARTSECTION_SEPARATOR = '[', wchar_t _INI_ENDSECTION_SEPARATOR = ']', wchar_t _INI_VALUE_SEPARATOR = '=', wchar_t _INI_MULTI_VALUE_SEPARATOR = ';');
		INI_baseOperator(std::wstring p, INI_formater I_V_S);
		void reformat(INI_formater I_V_S);
	};

	//Class that represents work with reading values and sections
	class INI_ReadOperator : public INI_baseOperator
	{
	public:
		INI_ReadOperator(std::wstring, wchar_t _INI_STARTSECTION_SEPARATOR = '[', wchar_t _INI_ENDSECTION_SEPARATOR = ']', wchar_t _INI_VALUE_SEPARATOR = '=', wchar_t _INI_MULTI_VALUE_SEPARATOR = ';');
		INI_ReadOperator(std::wstring p, INI_formater I_V_S);
		std::wstring get_value(std::wstring _SectionName, std::wstring _ValueName);
		int get_values_count(std::wstring _SectionName);
		std::wstring get_all_lines();
		std::vector<std::wstring> get_multi_values(std::wstring _SectionName, std::wstring _ValueName);
	private:
		std::wstring get_section(std::wstring _SectionName);
		int get_section_count();
	};

	//Base class that represents setting std::wstring function for INI_CreateOperator and INI_ChangeOperator
	class INI_WriteOperator : public INI_baseOperator
	{
	public:
		bool update_ini_file(std::wstring Lines);
		INI_WriteOperator(std::wstring p, wchar_t _INI_STARTSECTION_SEPARATOR = '[', wchar_t _INI_ENDSECTION_SEPARATOR = ']', wchar_t _INI_VALUE_SEPARATOR = '=', wchar_t _INI_MULTI_VALUE_SEPARATOR = ';');
		INI_WriteOperator(std::wstring p, INI_formater I_V_S);
	protected:
		std::wstring set_string(std::wstring _string_to_set, std::wstring _SectionName = L"[begin]");
	};

	//Class that represents work with creating values and section names
	class INI_CreateOperator : public INI_WriteOperator
	{
	public:
		INI_CreateOperator(std::wstring, wchar_t _INI_STARTSECTION_SEPARATOR = '[', wchar_t _INI_ENDSECTION_SEPARATOR = ']', wchar_t _INI_VALUE_SEPARATOR = '=', wchar_t _INI_MULTI_VALUE_SEPARATOR = ';');
		INI_CreateOperator(std::wstring p, INI_formater I_V_S);
		std::wstring create_value(std::wstring _SectionName, std::wstring _ValueName, std::wstring _Value);
		std::wstring create_multi_values(std::wstring _SectionName, std::wstring _ValueName, std::vector<std::wstring> _Values);
		std::wstring create_section(std::wstring _SectionName, std::wstring _SectionName_Above = L"begin");
		bool create_ini_File();
		bool create_ini_File(std::wstring Lines);
	};

	//Class that represents work with changing values and section names
	class INI_ChangeOperator : public INI_WriteOperator
	{
	public:
		INI_ChangeOperator(std::wstring, wchar_t _INI_STARTSECTION_SEPARATOR = '[', wchar_t _INI_ENDSECTION_SEPARATOR = ']', wchar_t _INI_VALUE_SEPARATOR = '=', wchar_t _INI_MULTI_VALUE_SEPARATOR = ';');
		INI_ChangeOperator(std::wstring p, INI_formater I_V_S);
		std::wstring change_value(std::wstring _SectionName, std::wstring _ValueName, std::wstring _Value_toSet);
		std::wstring change_multi_values(std::wstring _SectionName, std::wstring _ValueName, std::vector<std::wstring> _Values_toSet);
		std::wstring change_section(std::wstring _SectionName, std::wstring _SectionName_toChange);
	};

	//Class that represents work with commentaries in the file
	class Commentary_Handler
	{
		std::vector<std::wstring> comments;
		std::vector<int> index_mass;

		std::wstring _INI_COMMENT_BEGIN;
		std::wstring _INI_COMMENT_END;

	public:
		std::wstring cut_comments(std::wstring All_content);
		std::wstring set_comments(std::wstring All_content);
		std::wstring create_comment(std::wstring All_content, std::wstring Comment, int index);
		std::wstring delete_comment(std::wstring All_content, int index);
		Commentary_Handler(std::wstring comment_begin = L"/*", std::wstring comment_end = L"*/");
		std::vector<std::wstring> get_cutten_comments();
		int get_cutten_comment_index(std::wstring comment);
	};
}