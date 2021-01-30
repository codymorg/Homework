#include "Log.h"     //class def, streams
#include <iostream>  //cout
#include <string>    //string
#include <cassert>   //assert

Log::Log(const std::string& logFileName, Log::LogLevel level, Log::LogStream stream) : m_level(level), m_stream(stream)
{

  //ensure that the log level is valid
  assert(m_level > Log::LevelInvalid && m_level <= Log::LevelOff);

#ifdef _DEBUG
  m_level = Log::LogLevel::LevelDebug;
#endif

  if(stream == LogStream::StreamFile || stream == LogStream::StreamAll)
  {
    m_outFile.open(logFileName);

    if(!m_outFile.is_open())
    {
      std::cout << "Unable to open log file" << std::endl;
    }
  }
}

Log::~Log()
{
  if(m_outFile.is_open())
  {
    m_outFile.close();
  }
}

void Log::SetLevel(Log::LogLevel level)
{
  m_level = level;
}

void Log::SetStreams(Log::LogStream stream, const std::string& logFileName)
{
  m_stream = stream;

  if(m_stream == LogStream::StreamFile || m_stream == LogStream::StreamAll && !m_outFile.is_open())
  {
    m_outFile.open(logFileName);
  }

  if(m_stream == LogStream::StreamConsole) //close the output file if ostream is the only stream
  {
    if(m_outFile.is_open())
    {
      m_outFile.close();
    }
  }
}

void Log::Debug(const std::string& message, const std::string& file, const std::string& function, int line)
{
  if(m_level == LevelDebug)
    Write("--[DEBUG]--\t" + message, file, function, line);
}

void Log::Error(const std::string& message, const std::string& file, const std::string& function, int line)
{
  if(m_level <= LevelError)
    Write("--[ERROR]--\t" + message, file, function, line);
}

void Log::Warn(const std::string& message, const std::string& file, const std::string& function, int line)
{
  if(m_level <= LevelWarning)
    Write("--[WARNING]--\t" + message, file, function, line);
}

void Log::Info(const std::string& message, const std::string& file, const std::string& function, int line)
{
  if(m_level <= LevelInfo)
    Write("--[INFO]--\t" + message, file, function, line);
}

void Log::Write(const std::string& message, const std::string& file, const std::string& function, int line)
{

  std::string fileName(file.substr(file.rfind("\\") + 1));

  m_outStringStream << message << "\t\t\t" << fileName << "\t\t\t" << function << "\t\t\t"
                    << "line " << line << "\n";

  if(m_stream == StreamConsole)
  {
    std::cout << m_outStringStream.rdbuf();
  }
  else if(m_stream == StreamFile)
  {
    m_outFile << m_outStringStream.rdbuf();
  }
  else if(m_stream == StreamAll)
  {
    std::string output = m_outStringStream.str();
    
    m_outFile << output;
    std::cout << output;
  }

  m_outStringStream.str(std::string()); //clear the string stream buffer: More info at https://stackoverflow.com/questions/20731/how-do-you-clear-a-stringstream-variable
}
