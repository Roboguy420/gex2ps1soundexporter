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

#include <filesystem>
#include <format>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <list>

#include "SoundExporter.h"
#include "Constants.h"
#include "FLACExport.h"
#include "Globals.h"
#include "PCMStructs.h"

int main (int argc, char *argv[])
{
  // Default output folder to current working dir
  outputFolder = std::filesystem::current_path().string();

  static struct option longopts[] =
  {
    {"out", required_argument, 0, 'o'},
    {0, 0, 0, 0}
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "o", longopts, NULL)) != -1)
  {
    switch(opt)
    {
      case 'o':
        outputFolder = optarg;
        break;
      default:
        std::cerr << usageMsg << std::endl;
        std::cerr << std::format("Error {}: Arguments not formatted properly", EXIT_BAD_ARGS) << std::endl;
        return EXIT_BAD_ARGS;
    }
  }
  
  if (optind < argc)
  {
    inputFile = argv[optind];
  }
  else
  {
    std::cerr << usageMsg << std::endl;
    std::cerr << std::format("Error {}: Need at least the input file to work", EXIT_INSUFFICIENT_ARGS) << std::endl;
    return EXIT_INSUFFICIENT_ARGS;
  }

  if (!std::filesystem::exists(inputFile))
  {
    // Input file doesn't exist
    std::cerr << std::format("Error {}: Input file does not exist", EXIT_INPUT_NOT_FOUND) << std::endl;
    return EXIT_INPUT_NOT_FOUND;
  }

  // Eventually, this program will be able to interpret .snd files, but ATM it only does .smp so the only reader we need for now is for the .smp
  readerSmp.open(inputFile, std::ifstream::binary);
  readerSmp.exceptions(std::ifstream::eofbit);
  
  // Will read return value of this eventually
  readFile();
  
  readerSmp.close();

  std::cout << "Exit Code 0: Successful export with no errors" << std::endl;
  return EXIT_SUCCESSFUL_EXPORT;
}

int readFile()
{
  readSmp();

  exportToFLAC();

  return 0;
}

int readSmp()
{
  bool decodeError = false;
  bool dataIsBeingRead = false;
  short int old = 0;
  short int older = 0;
  std::list<short int*> tempSamplePCM;
  unsigned int endAddr;

  readerSmp.read((char*)&endAddr, sizeof(endAddr));

  readerSmp.seekg(16, readerSmp.cur);
  
  while (readerSmp.tellg() < endAddr)
  {
    short int* chunkOfPCM = new short int[28];
    int loopReturn = decodeBlock(chunkOfPCM, &old, &older);

    if (loopReturn < 0)
    {
      decodeError = true;
      break;
    }
    
    // Haven't implemented every loop code combination
    // SPU-ADPCM's loop flags are:
    // 0x4: Start loop
    // 0x2: Repeat loop
    // 0x1: End loop

    if ((loopReturn & 0x4) == 0x4)
    {
      dataIsBeingRead = true;
    }
    
    if (dataIsBeingRead)
    {
      tempSamplePCM.push_back(chunkOfPCM);
    }

    if ((loopReturn & 0x1) == 0x1)
    {
      dataIsBeingRead = false;

      // Now that the list has been completed and we know its size, we convert it into an array
      size_t numberOf16BitBlocks = tempSamplePCM.size() * 28;
      PCMData sample;
      sample.size = numberOf16BitBlocks;
      sample.waveform = new short int[numberOf16BitBlocks];
      
      for (size_t chunkNo = 0; chunkNo < numberOf16BitBlocks / 28; chunkNo++)
      {
        for (size_t pcmIndex = 0; pcmIndex < 28; pcmIndex++)
        {
          short int pcmShort = tempSamplePCM.front()[pcmIndex];
          sample.waveform[(chunkNo * 28) + pcmIndex] = pcmShort;
        }
        // Free heap allocated PCM chunk array to avoid a memory leak
        delete tempSamplePCM.front();
        tempSamplePCM.pop_front();
      }

      samplePCMList.push_back(sample);
    }
  }

  return 0;
}

// Decodes the 16-byte block from readerSnd's position as an ADPCM sample chunk 
// chunkOfPCM must have a size of 28
int decodeBlock(short int* chunkOfPCM, short int* oldPtr, short int* olderPtr)
{
  char shiftFilter;
  char loopMask;
  short int old = *oldPtr;
  short int older = *olderPtr;

  readerSmp.read((char*)&shiftFilter, sizeof(shiftFilter));
  readerSmp.read((char*)&loopMask, sizeof(loopMask));

  for (int byteIndex = 0; byteIndex < 14; byteIndex++)
  {
    char pcmByte;

    readerSmp.read((char*)&pcmByte, sizeof(pcmByte));

    for (int nibbleIndex = 0; nibbleIndex < 2; nibbleIndex++)
    {
      // Needs to be unsigned so that bitshifts are logical and not arithmetic
      unsigned char nibble = pcmByte;
      if (nibbleIndex == 0)
        nibble &= 0x0F;
      else
      {
        nibble >>= 4;
      }
      short int resultPCM = nibbleToPCM(nibble, shiftFilter, old, older);
      older = old;
      old = resultPCM;

      chunkOfPCM[(byteIndex * 2) + nibbleIndex] = resultPCM;
      short int debugChunk = chunkOfPCM[0];
    }
  }

  *oldPtr = old;
  *olderPtr = older;

  return loopMask;
}

short int nibbleToPCM(unsigned char nibble, unsigned char shiftFilter, short int old, short int older)
{
  short int waveData = nibble;
  unsigned char shift = shiftFilter & 0x0F;
  unsigned char filter = shiftFilter >> 4;

  waveData <<= 12;
  waveData >>= shift;
 
  // Not much to the shift part as you can see.
  // The filter part is a bit tricker, though, and I'm not sure if my implementation is accurate.
  // 
  // Duckstation would do it like so:
  // waveData += (old * posADPCMTable[filter]) >> 6;
  // waveData += (older * negADPCMTable[filter]) >> 6;
  //
  // On problemkaputt.de it would do it like so:
  // waveData += ((old * posADPCMTable[filter]) + (older * negADPCMTable[filter] + 32)) / 64
  //
  // Presumably the SPU does indeed perform an arithmetic right shift rather than a full on division, since this is much faster.
  // I have chosen a combination of both.

  waveData += ( (old * posADPCMTable[filter]) + (older * negADPCMTable[filter] + 32) ) >> 6;

  return waveData;
}
