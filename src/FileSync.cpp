#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include <iostream>
#include <fstream>
#include <filesystem>
#include <locale.h>
#include <io.h>
#include <fcntl.h>

#include "Hasher.h"
#include "Filer.h"
#include "Synchronizer.h"
#include "Interface.h"
#include "Initializer.h"

namespace fs = std::filesystem;

int main()
{  
    //hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // To be able to write UTF-16 to stdout.
    _setmode(_fileno(stdout), _O_WTEXT);
    // To be able to read  UTF-16 from stdin.
    _setmode(_fileno(stdin),  _O_WTEXT);

    Initializer i;
    Synchronizer s;
    Pather p = i.make_pather();
    s.ask_for_global_priorities();

    if (s.use_global_priorities)
        s.ask_priorities();

    while (!p.is_end())
    {
        if (!s.use_global_priorities)
            s.ask_priorities();
        Filer f(p.next_from());
        Filer t(p.next_to());
        s.file_share(f, t);
        s.file_compliance(f, t);
    }

    system("pause");
}