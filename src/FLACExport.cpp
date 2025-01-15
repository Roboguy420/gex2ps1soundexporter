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

#include <FLAC++/encoder.h>
#include <FLAC/stream_encoder.h>
#include <filesystem>
#include <format>

#include "FLACExport.h"
#include "Constants.h"
#include "Globals.h"

int exportToFLAC()
{
  size_t iterator = 0;
  for (PCMData flacData : samplePCMList)
  {
    std::filesystem::path dirName = outputFolder;
    std::filesystem::path fileName = std::format("{}.flac", iterator);
    std::filesystem::path fullWritePath = dirName / fileName;

    FLAC::Encoder::File flacFile;
    
    FLAC__int32 buffers32bit[flacData.size];

    for (size_t i = 0; i < flacData.size; i++)
    {
      buffers32bit[i] = (FLAC__int32)flacData.waveform[i];
    }
    
    flacFile.set_sample_rate(GEX2_SAMPLE_RATE);
    flacFile.set_channels(1);
    flacFile.set_bits_per_sample(GEX2_BITS_PER_SAMPLE);
    flacFile.init(fullWritePath);
    flacFile.process_interleaved(buffers32bit, flacData.size);

    FLAC::Encoder::Stream::State thisState = flacFile.get_state();

    flacFile.finish();
    iterator++;
  }

  return 0;
}

