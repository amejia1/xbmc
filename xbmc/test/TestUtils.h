/*
 *      Copyright (C) 2005-2012 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */
#pragma once

#include "utils/StdString.h"

namespace XFILE
{
  class CFile;
}

class CXBMCTestUtils
{
public:
  static CXBMCTestUtils &Instance();

  /* ReferenceFilePath() is used to prepend a path with the location to the
   * xbmc-test binary. It's assumed the test suite program will only be run
   * with xbmc-test residing in the source tree.
   */
  CStdString ReferenceFilePath(CStdString const& path);

  /* Function to set the reference file base path. */
  bool SetReferenceFileBasePath();

  /* Function used in creating a temporary file. It accepts a parameter
   * 'suffix' to append to the end of the tempfile path. The temporary
   * file is return as a XFILE::CFile object.
   */
  XFILE::CFile *CreateTempFile(CStdString const& suffix);

  /* Function used to close and delete a temporary file previously created
   * using CreateTempFile().
   */
  bool DeleteTempFile(XFILE::CFile *tempfile);

  /* Function to get path of a tempfile */
  CStdString TempFilePath(XFILE::CFile const* const tempfile);

  /* Functions to get variables used in the TestDownloadQueue tests. */
  std::vector<CStdString> &getTestDownloadQueueUrls();

  /* Functions to get variables used in the TestFileFactory tests. */
  std::vector<CStdString> &getTestFileFactoryUrls();

  /* Function to get advanced settings files. */
  std::vector<CStdString> &getAdvancedSettingsFiles();

  /* Function to get GUI settings files. */
  std::vector<CStdString> &getGUISettingsFiles();

  /* Function to parse command line options */
  void ParseArgs(int argc, char **argv);
private:
  CXBMCTestUtils();
  CXBMCTestUtils(CXBMCTestUtils const&);
  void operator=(CXBMCTestUtils const&);

  std::vector<CStdString> TestDownloadQueueUrls;
  std::vector<CStdString> TestFileFactoryUrls;

  std::vector<CStdString> AdvancedSettingsFiles;
  std::vector<CStdString> GUISettingsFiles;
};

#define XBMC_REF_FILE_PATH(s) CXBMCTestUtils::Instance().ReferenceFilePath(s)
#define XBMC_CREATETEMPFILE(a) CXBMCTestUtils::Instance().CreateTempFile(a)
#define XBMC_DELETETEMPFILE(a) CXBMCTestUtils::Instance().DeleteTempFile(a)
#define XBMC_TEMPFILEPATH(a) CXBMCTestUtils::Instance().TempFilePath(a)
