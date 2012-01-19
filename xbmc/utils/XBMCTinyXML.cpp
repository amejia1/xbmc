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

CXBMCTinyXML::Entity CXBMCTinyXML::entity[CXBMCTinyXML::NUM_ENTITY] =
{
  { "&amp;",  5, '&' },
  { "&lt;",   4, '<' },
  { "&gt;",   4, '>' },
  { "&quot;", 6, '\"' },
  { "&apos;", 6, '\'' }
};

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

void CXBMCTinyXML::ConvertUTF32ToUTF8(unsigned long input, char* output, int* length)
{
  const unsigned long BYTE_MASK = 0xBF;
  const unsigned long BYTE_MARK = 0x80;
  const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

  if (input < 0x80)
    *length = 1;
  else if ( input < 0x800 )
    *length = 2;
  else if ( input < 0x10000 )
    *length = 3;
  else if ( input < 0x200000 )
    *length = 4;
  else
    { *length = 0; return; }  // This code won't covert this correctly anyway.

  output += *length;

  // Scary scary fall throughs.
  switch (*length)
  {
    case 4:
      --output;
      *output = (char)((input | BYTE_MARK) & BYTE_MASK);
      input >>= 6;
    case 3:
      --output;
      *output = (char)((input | BYTE_MARK) & BYTE_MASK);
      input >>= 6;
    case 2:
      --output;
      *output = (char)((input | BYTE_MARK) & BYTE_MASK);
      input >>= 6;
    case 1:
      --output;
      *output = (char)(input | FIRST_BYTE_MARK[*length]);
  }
}

// Provide TiXmlBase::GetEntity symbol within XBMC to allow unrecognized
// entities to be accepted.
const char* TiXmlBase::GetEntity(const char* p, char* value, int* length, TiXmlEncoding encoding)
{
  // Presume an entity, and pull it out.
  TIXML_STRING ent;
  int i;
  *length = 0;

  if ( *(p+1) && *(p+1) == '#' && *(p+2) )
  {
    unsigned long ucs = 0;
    std::ptrdiff_t delta = 0;
    unsigned mult = 1;

    if ( *(p+2) == 'x' )
    {
      // Hexadecimal.
      if ( !*(p+3) ) return 0;

      const char* q = p+3;
      q = strchr( q, ';' );

      if ( !q || !*q ) return 0;

      delta = q-p;
      --q;

      while ( *q != 'x' )
      {
        if ( *q >= '0' && *q <= '9' )
          ucs += mult * (*q - '0');
        else if ( *q >= 'a' && *q <= 'f' )
          ucs += mult * (*q - 'a' + 10);
        else if ( *q >= 'A' && *q <= 'F' )
          ucs += mult * (*q - 'A' + 10 );
        else
          return 0;
        mult *= 16;
        --q;
      }
    }
    else
    {
      // Decimal.
      if ( !*(p+2) ) return 0;

      const char* q = p+2;
      q = strchr( q, ';' );

      if ( !q || !*q ) return 0;

      delta = q-p;
      --q;

      while ( *q != '#' )
      {
        if ( *q >= '0' && *q <= '9' )
          ucs += mult * (*q - '0');
        else
          return 0;
        mult *= 10;
        --q;
      }
    }
    if ( encoding == TIXML_ENCODING_UTF8 )
    {
      // convert the UCS to UTF-8
      CXBMCTinyXML::ConvertUTF32ToUTF8(ucs, value, length);
    }
    else
    {
      *value = (char)ucs;
      *length = 1;
    }
    return p + delta + 1;
  }

  // Now try to match it.
  for( i=0; i<CXBMCTinyXML::NUM_ENTITY; ++i )
  {
    if ( strncmp( CXBMCTinyXML::entity[i].str, p, CXBMCTinyXML::entity[i].strLength ) == 0 )
    {
      assert( strlen( CXBMCTinyXML::entity[i].str ) == CXBMCTinyXML::entity[i].strLength );
      *value = CXBMCTinyXML::entity[i].chr;
      *length = 1;
      return ( p + CXBMCTinyXML::entity[i].strLength );
    }
  }

  // So it wasn't an entity, its unrecognized, or something like that.
  *value = *p;  // Don't put back the last one, since we return it!
  *length = 1;  // Leave unrecognized entities - this doesn't really work.
          // Just writes strange XML.
  return p+1;
}
