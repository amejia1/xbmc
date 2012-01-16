/*
 *      Copyright (C) 2005-2011 Team XBMC
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
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "XBMCTinyXML.h"
#include "filesystem/File.h"

CXBMCTinyXML::CXBMCTinyXML()
{
}

CXBMCTinyXML::CXBMCTinyXML(const char *documentName)
{
  LoadFile(documentName);
}

bool CXBMCTinyXML::LoadFile(const char *_filename, TiXmlEncoding encoding)
{
  // There was a really terrifying little bug here. The code:
  //    value = filename
  // in the STL case, cause the assignment method of the std::string to
  // be called. What is strange, is that the std::string had the same
  // address as it's c_str() method, and so bad things happen. Looks
  // like a bug in the Microsoft STL implementation.
  // Add an extra string to avoid the crash.
  TIXML_STRING filename( _filename );
  value = filename;

  XFILE::CFileStream file;
  if (!file.Open(value))
  {
    SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
    return false;
  }

  // Delete the existing data:
  Clear();
  location.Clear();

  TIXML_STRING data;
  data.reserve(8 * 1000);
  StreamIn(&file, &data);
  file.Close();

  Parse(data.c_str(), 0, encoding);

  if (  Error() )
    return false;
  else
    return true;
}

bool CXBMCTinyXML::SaveFile(const char *filename) const
{
  XFILE::CFile file;
  if (file.OpenForWrite(filename, true))
  {
    TiXmlPrinter printer;
    Accept(&printer);
    file.Write(printer.CStr(), printer.Size());
    return true;
  }
  return false;
}
