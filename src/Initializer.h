#pragma once

#include "Enums.h"
#include "Interface.h"

class Initializer
{
public:
	Pather make_pather()
	{
		fs::path ini_path;
		Pather p;

		ini_path = fs::path(Interface::ask_ini_path());
		p.read_pathes_from_ini(ini_path);

		sync_action sa = Interface::ask_sync_action();

		switch (sa)
		{
		case update:
			break;
		case upload:
			p.swap_from_to();
			break;
		default:
			break;
		}

		return p;
	}

private:
};