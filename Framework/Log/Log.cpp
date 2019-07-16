#include "Framework.h"
#include "Log.h"
#include "ILogger.h"

std::string Log::caller_name;
bool Log::is_log_to_file = true;

std::weak_ptr<class ILogger> Log::logger;
std::ofstream Log::fout;
std::mutex Log::log_mutex;
std::string Log::log_file_name = "log.txt";
std::vector<LogCommand> Log::log_buffer;
bool Log::is_first_log = true;


void Log::SetLogger(const std::weak_ptr<ILogger>& i_logger)
{
	//this->logger = i_logger; 불가능. static이기 때문에 Log가 생성되지 않아도 공간이 잡힘.
	//정작 Log는 생성되지 않았기 때문에 이가 만들어질지 모름. 따라서 this->사용불가

	logger = i_logger;
}

void Log::Write(const char * text, const LogType & type) //*라 null체크 필요함
{
	if (!text) {
		LOG_ERROR("Invalid Parameter!!");
		return;
	}

	std::lock_guard<std::mutex> guard(log_mutex);

	const auto log_to_file = logger.expired() || is_log_to_file; //weak_ptr주의점. 만료되었는지 확인해야 함
	const auto formatted_text = caller_name.empty() ? std::string(text) : caller_name + " : " + std::string(text);

	if (log_to_file) {
		log_buffer.emplace_back(formatted_text, text);
		log_buffer.emplace_back(formatted_text.c_str(), type);
	}
	else { 
		//Editor에서 engine이 만들어진 후 log를 만들면 engine의 log가 파일로만 빠짐.
		//화면에는 출력되지 않으므로 확인이 어려운데 이를 해결하기 위해 FlushBuffer()함수를 만듦
		FlushBuffer();
		log_buffer.emplace_back(formatted_text.c_str(), type);
	}

	caller_name.clear();
}

void Log::Write(const std::string & text, const LogType & type) //참조라서 null체크 필요 없음
{
	Write(text.c_str(), type);
}

void Log::Info_f(const char * text, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, sizeof(buffer), text, args);
	va_end(args);

	Write(buffer, LogType::Info);
}

void Log::Warning_f(const char * text, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, sizeof(buffer), text, args);
	va_end(args);

	Write(buffer, LogType::Warning);
}

void Log::Error_f(const char * text, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, sizeof(buffer), text, args);
	va_end(args);

	Write(buffer, LogType::Error);
}

void Log::FlushBuffer()
{
	if (logger.expired() || log_buffer.empty()) return;

	for (const auto &log : log_buffer) LogToLogger(log.text.c_str(), log.type);

	log_buffer.clear();
	log_buffer.shrink_to_fit();
} 

void Log::LogToLogger(const char * text, const LogType & type)
{
	if (!text) {
		LOG_ERROR("Invalid Parameter!!");
		return;
	}

	logger.lock()->Log(std::string(text), static_cast<uint>(type)); //lock = 약한참조 -> shared_ptr

}

void Log::LogToFile(const char * text, const LogType & type)
{
	if (!text) {
		LOG_ERROR("Invalid Parameter!!");
		return;
	}

	const std::string prefix = (type == LogType::Info) ? "Info : " : (type == LogType::Warning) ? "Warning : " : "Error : ";
	const auto final_text = prefix + text;

	if (is_first_log)
	{
		FileSystem::Delete_File(log_file_name);
		is_first_log = false;
	}

	fout.open(log_file_name, std::ofstream::out | std::ofstream::app); //append. 마지막으로 쓴 커서 위치에 이어서 씀

	if (fout.is_open())
	{
		fout << final_text << std::endl;
		fout.close();
	}
}
