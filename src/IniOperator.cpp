#include "IniOperator.h"
#include <codecvt>

namespace INI_WORKS
{
	void INI_baseOperator::remove_spaces(std::wstring& str)
	{

		std::wstring::iterator result = str.begin();
		std::wstring buffer;

		while (result != str.end())
		{
			//For making no errors on hieroglyphs and other shiete
			try
			{
				if (*result == ' ')
				{
					*result = NULL;
					result++;
				}
				else
				{
					buffer += *result;
					result++;
				}
			}
			catch (const std::exception&)
			{
			}
		}

		str = buffer;

	}
	INI_baseOperator::INI_baseOperator(std::wstring p, wchar_t STARTSECTION_SEPARATOR, wchar_t ENDSECTION_SEPARATOR, wchar_t VALUE_SEPARATOR, wchar_t MULTI_VALUE_SEPARATOR)
	{
		if (fs::exists(p))
		{
			path_ = p;
			_INI_STARTSECTION_SEPARATOR = STARTSECTION_SEPARATOR;
			_INI_ENDSECTION_SEPARATOR = ENDSECTION_SEPARATOR;
			_INI_VALUE_SEPARATOR = VALUE_SEPARATOR;
			_INI_MULTI_VALUE_SEPARATOR = MULTI_VALUE_SEPARATOR;
		}
		else
		{
			throw std::logic_error("No file found by this path!");
		}
	}

	INI_baseOperator::INI_baseOperator(std::wstring p, INI_formater I_V_S)
	{
		if (fs::exists(p))
		{
			path_ = p;
			_INI_STARTSECTION_SEPARATOR = I_V_S._INI_STARTSECTION_SEPARATOR;
			_INI_ENDSECTION_SEPARATOR = I_V_S._INI_ENDSECTION_SEPARATOR;
			_INI_VALUE_SEPARATOR = I_V_S._INI_ENDSECTION_SEPARATOR;
			_INI_MULTI_VALUE_SEPARATOR = I_V_S._INI_MULTI_VALUE_SEPARATOR;
		}
		else
		{
			throw std::logic_error("No file found by this path!");
		}
	}

	void INI_baseOperator::reformat(INI_formater I_V_S)
	{
		_INI_STARTSECTION_SEPARATOR = I_V_S._INI_STARTSECTION_SEPARATOR;
		_INI_ENDSECTION_SEPARATOR = I_V_S._INI_ENDSECTION_SEPARATOR;
		_INI_VALUE_SEPARATOR = I_V_S._INI_ENDSECTION_SEPARATOR;
		_INI_MULTI_VALUE_SEPARATOR = I_V_S._INI_MULTI_VALUE_SEPARATOR;
	}

	INI_ReadOperator::INI_ReadOperator(std::wstring p, wchar_t STARTSECTION_SEPARATOR, wchar_t ENDSECTION_SEPARATOR, wchar_t VALUE_SEPARATOR, wchar_t MULTI_VALUE_SEPARATOR) : INI_baseOperator(p, STARTSECTION_SEPARATOR, ENDSECTION_SEPARATOR, VALUE_SEPARATOR, MULTI_VALUE_SEPARATOR) {}
	INI_ReadOperator::INI_ReadOperator(std::wstring p, INI_formater I_V_S) : INI_baseOperator(p, I_V_S) {}

	//Getting section out of ini file
	std::wstring INI_ReadOperator::get_section(std::wstring _SectionName)
	{
		std::wifstream ifs;
		std::wstring readingStr;
		std::wstring outputStr;

		ifs.open(path_);

		if (ifs.good())
		{
			do
			{
				getline(ifs, readingStr);

				if (readingStr.find(']') != std::wstring::npos)
				{
					remove(readingStr.begin(), readingStr.end(), _INI_STARTSECTION_SEPARATOR);
					remove(readingStr.begin(), readingStr.end(), _INI_ENDSECTION_SEPARATOR);
					readingStr.erase(readingStr.end() - 1);
					readingStr.erase(readingStr.end() - 1);
				}
				else
					continue;

			} while (readingStr != _SectionName && !ifs.eof());

			if (!ifs.eof())
			{
				outputStr = L"";
				readingStr = L"";

				bool quitCond = false;
				do
				{
					getline(ifs, readingStr);

					if (readingStr.find(']') == std::wstring::npos)
						outputStr += readingStr + L"\n";
					else
						quitCond = true;

				} while (!quitCond);
			}
		}
		else
		{
			throw "Reading stream is not opened!";
		}

		ifs.clear();
		ifs.close();

		return outputStr;
	}

	//Getting value out of section
	std::wstring INI_ReadOperator::get_value(std::wstring _SectionName, std::wstring _ValueName)
	{
		//Section text
		std::wstring _Section = get_section(_SectionName);
		//std::vector with a std::wstring
		std::vector<std::wstring> NewStrings;
		std::wstring buffer = L"";      //buffer std::wstring

		for (size_t i = 0; i < _Section.size(); i++)
		{
			if (_Section[i] != *(const_cast<wchar_t*>(L"\n")) && i + 1 != _Section.size()) // "\n" splitter
			{
				buffer += _Section[i];
			}
			else
			{
				//taking the buffered std::wstring in std::vector
				NewStrings.push_back(buffer);
				buffer = L"";
			}
		}

		if (buffer != L"")
			NewStrings.push_back(buffer);

		int size = NewStrings.size();
		for (int i = 0; i < size; i++)
		{
			int fnd = NewStrings[i].find(_INI_VALUE_SEPARATOR);
			buffer = NewStrings[i];
			buffer = buffer.erase(fnd, NewStrings[i].size() - 1);

			remove_spaces(buffer);//removing all free spaces
			remove_spaces(_ValueName);//removing all free spaces
			
			//checking if the std::wstring contains the name we searching for
			if (buffer == _ValueName)
			{
				buffer = NewStrings[i].erase(0, NewStrings[i].find(_INI_VALUE_SEPARATOR) + 1);
				//Remove_spaces(buffer);
				if (buffer[0] == ' ')
					buffer.erase(0,1);

				return buffer;
			}
		}
		return L"NoSuchValueFound";
	}

	int INI_ReadOperator::get_values_count(std::wstring _SectionName)
	{
		int counter = 0;

		std::wstring _Section = get_section(_SectionName);
		std::wstring buffer;

		for (size_t i = 0; i < _Section.size(); i++)
		{
			if (_Section[i] != *(const_cast<wchar_t*>(L"\n")) && _Section[i] != _Section.size()) // "\n" splitter
				buffer += _Section[i];
			else
			{
				remove_spaces(buffer);

				if (buffer != L"" && buffer != L"\n")
					counter++;

				buffer = L"";
			}
		}

		remove_spaces(buffer);

		if (buffer != L"" && buffer != L"\n")
			counter++;

		return counter;
	}

	std::wstring INI_ReadOperator::get_all_lines()
	{
		std::wifstream ifs(path_);
		std::wstring reading;
		std::wstring All_Lines;
		do
		{
			getline(ifs, reading);

			All_Lines += reading + L"\n";
		} while (!ifs.eof());

		return All_Lines;
	}

	std::vector<std::wstring> INI_ReadOperator::get_multi_values(std::wstring _SectionName, std::wstring _ValueName)
	{
		std::wstring value_return = this->get_value(_SectionName, _ValueName);

		std::vector<std::wstring> mass_values;

		int last_delim_index = -1;
		int value_counter = 0;

		std::wstring bfr;

		for (size_t i = 0; i < value_return.size(); i++)
			if (value_return[i] == _INI_MULTI_VALUE_SEPARATOR)
			{
				for (int t = last_delim_index + 1; t < i; t++)
					bfr += value_return[t];

				mass_values.push_back(bfr);
				value_counter++;
				last_delim_index = i;

				bfr = L"";
			}

		return mass_values;
	}

	int INI_ReadOperator::get_section_count()
	{
		return 0;
	}

	INI_WriteOperator::INI_WriteOperator(std::wstring p, wchar_t STARTSECTION_SEPARATOR, wchar_t ENDSECTION_SEPARATOR, wchar_t VALUE_SEPARATOR, wchar_t  MULTI_VALUE_SEPARATOR) : INI_baseOperator(p, STARTSECTION_SEPARATOR, ENDSECTION_SEPARATOR, VALUE_SEPARATOR, MULTI_VALUE_SEPARATOR) {}

	INI_WriteOperator::INI_WriteOperator(std::wstring p, INI_formater I_V_S) : INI_baseOperator(p, I_V_S) {}

	bool INI_WriteOperator::update_ini_file(std::wstring Lines)
	{
		try
		{
			std::wifstream chkStrm(path_);

			if (chkStrm.good())
			{
				chkStrm.clear();
				chkStrm.close();

				std::wofstream ofs;
				ofs.open(path_);

				ofs << Lines;
				ofs.close();

				return true;
			}
			else
				return false;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	INI_CreateOperator::INI_CreateOperator(std::wstring p, wchar_t STARTSECTION_SEPARATOR, wchar_t ENDSECTION_SEPARATOR, wchar_t VALUE_SEPARATOR, wchar_t MULTI_VALUE_SEPARATOR) : INI_WriteOperator(p, STARTSECTION_SEPARATOR, ENDSECTION_SEPARATOR, VALUE_SEPARATOR, MULTI_VALUE_SEPARATOR) {}

	INI_CreateOperator::INI_CreateOperator(std::wstring p, INI_formater I_V_S) : INI_WriteOperator(p, I_V_S) {}

	std::wstring INI_WriteOperator::set_string(std::wstring _string_to_set, std::wstring _SectionName)
	{
		std::wstring All_content;

		//Represents all the content above section to insert
		std::wstring Above_Content;
		std::wstring Below_Content;

		std::wstring readingStr;

		std::wifstream ifs;

		ifs.open(path_);

		bool Section_found = false;

		//looking for _SectionName_Above section
		do
		{
			getline(ifs, readingStr);
			Above_Content += readingStr + L"\n";

			if (readingStr.find(']') != std::wstring::npos)
			{
				remove(readingStr.begin(), readingStr.end(), _INI_STARTSECTION_SEPARATOR);
				remove(readingStr.begin(), readingStr.end(), _INI_ENDSECTION_SEPARATOR);

				readingStr.erase(readingStr.end() - 1);
				readingStr.erase(readingStr.end() - 1);

				if (readingStr == _SectionName)
					Section_found = true;
			}
			else
				continue;

		} while (!Section_found && !ifs.eof());

		//looking for the end of _SectionName_Above section
		if (Section_found)
		{
			do
			{
				getline(ifs, readingStr);

				if (readingStr.find(']') != std::wstring::npos)
				{
					//getting the name of next section
					Below_Content += readingStr + L"\n";

					break;
				}
				else
				{
					Above_Content += readingStr + L"\n";
					continue;
				}

			} while (!ifs.eof());

			if (!ifs.eof())
				do
				{
					getline(ifs, readingStr);
					Below_Content += readingStr + L"\n";

				} while (!ifs.eof());

				ifs.clear();
				ifs.close();

				All_content = Above_Content + _string_to_set + L"\n" + Below_Content;
		}

		return All_content;
	}

	std::wstring INI_CreateOperator::create_value(std::wstring _SectionName, std::wstring _ValueName, std::wstring _Value)
	{
		return set_string(_ValueName + L" " + _INI_VALUE_SEPARATOR + L" " + _Value, _SectionName);
	}

	std::wstring INI_CreateOperator::create_multi_values(std::wstring _SectionName, std::wstring _ValueName, std::vector<std::wstring> _Values)
	{
		std::wstring params;

		for (size_t i = 0; i < _Values.size(); i++)
		{
			params += _Values[i];
			params += L";";
		}

		return create_value(_SectionName, _ValueName, params);
	}

	std::wstring INI_CreateOperator::create_section(std::wstring _SectionName, std::wstring _SectionName_Above)
	{
		return set_string(L"[" + _SectionName + L"]", _SectionName_Above);
	}

	bool INI_CreateOperator::create_ini_File()
	{
		std::wifstream chkStrm(path_);

		if (!chkStrm.good())
		{
			chkStrm.clear();
			chkStrm.close();

			std::wofstream ofs(path_);

			ofs << L"[begin]" << std::endl << L"[end]";
		}
		else
			throw L"File Already Exists!";
	}

	bool INI_CreateOperator::create_ini_File(std::wstring Lines)
	{
		std::wifstream chkStrm(path_);

		if (!chkStrm.good())
		{
			chkStrm.clear();
			chkStrm.close();

			std::wofstream ofs(path_);

			ofs << "[begin]" << std::endl << Lines << std::endl << "[end]";
		}
		else
			throw "File Already Exists!";
	}

	INI_ChangeOperator::INI_ChangeOperator(std::wstring p, wchar_t STARTSECTION_SEPARATOR, wchar_t ENDSECTION_SEPARATOR, wchar_t VALUE_SEPARATOR, wchar_t MULTI_VALUE_SEPARATOR) : INI_WriteOperator(p, STARTSECTION_SEPARATOR, ENDSECTION_SEPARATOR, VALUE_SEPARATOR, MULTI_VALUE_SEPARATOR) {}

	INI_ChangeOperator::INI_ChangeOperator(std::wstring p, INI_formater I_V_S) : INI_WriteOperator(p, I_V_S) {}

	std::wstring INI_ChangeOperator::change_value(std::wstring _SectionName, std::wstring _ValueName, std::wstring _Value_toSet)
	{
		std::wstring All_content;

		//Represents all the content above section to insert
		std::wstring Above_Content;
		std::wstring Below_Content;

		std::wstring readingStr;

		std::wifstream ifs;

		ifs.open(path_);

		bool Section_found = false;

		//looking for _SectionName_Above section
		do
		{
			getline(ifs, readingStr);
			Above_Content += readingStr + L"\n";

			if (readingStr.find(']') != std::wstring::npos)
			{
				remove(readingStr.begin(), readingStr.end(), _INI_STARTSECTION_SEPARATOR);
				remove(readingStr.begin(), readingStr.end(), _INI_ENDSECTION_SEPARATOR);

				readingStr.erase(readingStr.end() - 1);
				readingStr.erase(readingStr.end() - 1);

				remove_spaces(readingStr);
				remove_spaces(_SectionName);

				if (readingStr == _SectionName)
					Section_found = true;
			}
			else
				continue;

		} while (!Section_found && !ifs.eof());

		//looking for the end of _SectionName_Above section
		if (Section_found)
		{
			do
			{
				getline(ifs, readingStr);

				if (readingStr.find(_ValueName) != std::wstring::npos)
				{
					remove_spaces(readingStr);

					readingStr.erase(readingStr.find(_INI_VALUE_SEPARATOR), readingStr.size() - 1);

					readingStr += L" ";
					readingStr += _INI_VALUE_SEPARATOR;
					readingStr += L" ";
					readingStr += _Value_toSet;

					//getting the name of next section
					Below_Content += readingStr + L"\n";

					break;
				}
				else
				{
					Above_Content += readingStr + L"\n";
					continue;
				}

			} while (!ifs.eof());

			do
			{
				getline(ifs, readingStr);
				Below_Content += readingStr + L"\n";

			} while (!ifs.eof());

			ifs.clear();
			ifs.close();
		}

		ifs.clear();
		ifs.close();

		All_content = Above_Content + Below_Content;

		return All_content;
	}

	std::wstring INI_ChangeOperator::change_multi_values(std::wstring _SectionName, std::wstring _ValueName, std::vector<std::wstring> _Values_toSet)
	{
		std::wstring params;

		for (size_t i = 0; i < _Values_toSet.size(); i++)
		{
			params += _Values_toSet[i];
			params += L";";
		}

		return change_value(_SectionName, _ValueName, params);
	}

	std::wstring INI_ChangeOperator::change_section(std::wstring _SectionName, std::wstring _SectionName_toChange)
	{
		std::wstring All_content;

		//Represents all the content above section to insert
		std::wstring Above_Content;
		std::wstring Below_Content;
		std::wstring buffer;

		std::wstring readingStr;

		std::wifstream ifs;

		ifs.open(path_);

		bool Section_found = false;

		//looking for _SectionName_Above section
		do
		{
			getline(ifs, readingStr);
			buffer = readingStr + L"\n";

			remove_spaces(readingStr);
			remove_spaces(_SectionName);

			if (readingStr.find(']') != std::wstring::npos)
			{
				if (readingStr.find(L"_SectionName"))

					remove(readingStr.begin(), readingStr.end(), _INI_STARTSECTION_SEPARATOR);
				remove(readingStr.begin(), readingStr.end(), _INI_ENDSECTION_SEPARATOR);

				readingStr.erase(readingStr.end() - 1);
				readingStr.erase(readingStr.end() - 1);

				if (readingStr == _SectionName)
				{
					Section_found = true;
					Above_Content += _INI_STARTSECTION_SEPARATOR;
					Above_Content += _SectionName_toChange;
					Above_Content += _INI_ENDSECTION_SEPARATOR;
					Above_Content += L"\n";
				}
				else { Above_Content += buffer; }
			}
			else
			{
				Above_Content += buffer;
				continue;
			}

		} while (!Section_found && !ifs.eof());

		do
		{
			getline(ifs, readingStr);
			Below_Content += readingStr + L"\n";

		} while (!ifs.eof());

		ifs.clear();
		ifs.close();

		All_content = Above_Content + Below_Content;

		return All_content;
	}

	std::wstring Commentary_Handler::cut_comments(std::wstring All_content)
	{

		int iter_counter = 0;

		std::wstring All_content_cutted;

		int token_found = -1;
		int LT_tokey_found = 0;

		while (token_found != 0)
		{
			token_found = All_content.find(L"\n");
			std::wstring splitted = All_content.substr(LT_tokey_found, token_found - LT_tokey_found);

			if (token_found == -1)
				break;

			LT_tokey_found = token_found;

			All_content.erase(LT_tokey_found, token_found - LT_tokey_found + 1);
			/*getline(ifs, reading);*/
			std::wstring to_all_content = splitted;

			if (splitted.find(_INI_COMMENT_BEGIN) != std::wstring::npos && splitted.find(_INI_COMMENT_END) != std::wstring::npos)
			{
				to_all_content.erase(splitted.find(_INI_COMMENT_BEGIN), splitted.find(_INI_COMMENT_END) - splitted.find(_INI_COMMENT_BEGIN) + 2);

				std::wstring comment = splitted.erase(0, splitted.find(_INI_COMMENT_BEGIN)).erase(splitted.find(_INI_COMMENT_END), splitted.size() - splitted.find(_INI_COMMENT_END) - 2);

				/*index_mass = (int*)realloc(index_mass, sizeof(int) * (count));
				index_mass[count - 1] = iter_counter;*/

				index_mass.push_back(iter_counter);

				comments.push_back(comment);
			}

			All_content_cutted += to_all_content;
			All_content_cutted += L"\n";

			iter_counter++;
		}

		return All_content_cutted;
	}

	std::wstring Commentary_Handler::set_comments(std::wstring All_content)
	{
		int iter_counter = 0;
		int i_c = 0;
		std::wstring All_content_set;
		bool isFound = false;
		int token_found = -1;
		int LT_tokey_found = 0;

		while (iter_counter != 100)
		{
			token_found = All_content.find(L"\n");
			std::wstring splitted = All_content.substr(LT_tokey_found, token_found - LT_tokey_found);

			if (token_found == -1)
				break;

			LT_tokey_found = token_found;

			All_content.erase(LT_tokey_found, token_found - LT_tokey_found + 1);

			std::wstring to_all_content = splitted;

			for (size_t i = 0; i < index_mass.size(); i++)
			{
				if (iter_counter == index_mass[i])
				{
					isFound = true;
					i_c++;
					break;
				}
				else
				{
					isFound = false;
				}
			}

			All_content_set += to_all_content;

			if (isFound)
			{
				All_content_set += L" " + comments[i_c - 1];
			}
			All_content_set += L"\n";

			iter_counter++;
		}

		comments.clear();
		index_mass.clear();

		return All_content_set;
	}

	std::wstring Commentary_Handler::create_comment(std::wstring All_content, std::wstring Comment, int index)
	{
		Comment = _INI_COMMENT_BEGIN + Comment + _INI_COMMENT_END;
		comments.push_back(Comment);
		index_mass.push_back(index);

		std::wstring rtrn = set_comments(All_content);
		return rtrn;
	}

	std::wstring Commentary_Handler::delete_comment(std::wstring All_content, int index)
	{
		All_content = cut_comments(All_content);
		comments.erase(comments.begin()+index);
		(comments).swap(comments);

		index_mass.erase(index_mass.begin() + index);
		(index_mass).swap(index_mass);
		std::wstring ret = set_comments(All_content);
		return ret;
	}

	Commentary_Handler::Commentary_Handler(std::wstring comment_begin, std::wstring comment_end)
	{
		_INI_COMMENT_BEGIN = comment_begin;
		_INI_COMMENT_END = comment_end;
	}
	std::vector<std::wstring> Commentary_Handler::get_cutten_comments()
	{
		if (comments.size() > 0)
			return comments;
		else
			throw std::logic_error("No comments were cutted!");
	}
	int Commentary_Handler::get_cutten_comment_index(std::wstring com)
	{
		for (size_t i = 0; i < comments.size(); i++)
			if (comments[i] == com)
				return index_mass[i];
			else
				return -1;
	}
}