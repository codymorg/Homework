#include "Serializer.h"
#include <iostream>
#include <fstream>

static const unsigned int MEMBUF_SIZE = 1024;

void Serializer::readFile(const char * filename, char* memBuf)
{
  //open file
  std::ifstream file;
  file.open(filename, std::ios::ate | std::ios::in);
  std::streamoff filesize = file.tellg();
  file.seekg(0);
  file.read(memBuf, filesize);

  //close file
  file.close();
}

Serializer::Serializer()
{}


Serializer::~Serializer()
{}


bool Serializer::EnterValue(rapidjson::Value & value, void * object)
{
  //query the type and load it
  if (value.IsNumber())
  {
    if (value.IsInt()) *(int*)object = value.GetInt();
    if (value.IsDouble()) *(double*)object = value.GetDouble();
    if (value.IsFloat()) *(float*)object = value.GetFloat();
  }
  else if (value.IsBool()) *(bool*)object = value.GetBool();
  else if (value.IsString()) *(std::string*)object = value.GetString();
  else if (value.IsArray())
    {
      //test for different sizes, save the size into typesize
      std::size_t typeSize = sizeof(void *);
      if (value[0].IsInt())    typeSize = sizeof(int);
      if (value[0].IsDouble()) typeSize = sizeof(double);
      if (value[0].IsFloat())  typeSize = sizeof(float);
      if (value[0].IsBool())   typeSize = sizeof(bool);
      if (value[0].IsString()) typeSize = sizeof(const char *);

      for (unsigned int i = 0; i < value.Size(); ++i)
      {
        //put the value into the index of the array
        EnterValue(value[i], (char*)object + (i * typeSize));
      }
    }
  else
  {
    ++levelsDeep;
    return false;
  }
  levelsDeep = 0;
  return true;
}

void Serializer::Initialize(const char * filename)
{
  //load file into memory buffer
  char memBuf[MEMBUF_SIZE] = "";
  readFile(filename, memBuf);

  //parse file memory into doc
  doc.Parse(memBuf);
  levelsDeep = 0;
}

void Serializer::readVal(std::string fullname, void* object)
{
  static bool firstLevel = true; //bool to check if this is the first time through this function
  std::string name = fullname; //first part of the string
  rapidjson::Value value;


  //this does not work yet for nested Functions
  size_t seperator = name.find_first_of("/\\");
  //cut the name off at the seperator
  name = name.substr(0, seperator);
  //store the value to query
  if (levelsDeep == 0)
  {
    value = doc[name.c_str()];
  }
  //else
  //{
  //  value = ;
  //}


  //enter the type until a basic type is found and laoded into an object
  if (!EnterValue(value, object))
  {
    //get the next part of the object path by removing name from fullname
    fullname.replace(0, seperator + 1, "");
    readVal(fullname, object);
  }
}
