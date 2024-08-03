/*  gex2ps1soundexporter: Command line program for exporting Gex 2 PS1 sounds
    Copyright (C) 2024  Roboguy420

    gex2ps1soundexporter is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gex2ps1soundexporter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with gex2ps1soundexporter.  If not, see <https://www.gnu.org/licenses/>.  */

#pragma once

#include <fstream>
#include <string>
#include <list>

#include "PCMStructs.h"

// Uninitialised (.bss)

extern std::string inputFile;
extern std::string outputFolder;

extern std::ifstream readerSmp;

extern std::list<PCMData> samplePCMList;

// Initialised (.data)

extern const std::string usageMsg;
extern const char posADPCMTable[5];
extern const char negADPCMTable[5];

