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
	//this->logger = i_logger; �Ұ���. static�̱� ������ Log�� �������� �ʾƵ� ������ ����.
	//���� Log�� �������� �ʾұ� ������ �̰� ��������� ��. ���� this->���Ұ�

	logger = i_logger;
}

void Log::Write(const char * text, const LogType & type) //*�� nullüũ �ʿ���
{
	if (!text) {
		LOG_ERROR("Invalid Parameter!!");
		return;
	}

	std::lock_guard<std::mutex> guard(log_mutex);

	const auto log_to_file = logger.expired() || is_log_to_file; //weak_ptr������. ����Ǿ����� Ȯ���ؾ� ��
	const auto formatted_text = caller_name.empty() ? std::string(text) : caller_name + " : " + std::string(text);

	if (log_to_file) {
		log_buffer.emplace_back(formatted_text, text);
		log_buffer.emplace_back(formatted_text.c_str(), type);
	}
	else { 
		//Editor���� engine�� ������� �� log�� ����� engine�� log�� ���Ϸθ� ����.
		//ȭ�鿡�� ��µ��� �����Ƿ� Ȯ���� ���� �̸� �ذ��ϱ� ���� FlushBuffer()�Լ��� ����
		FlushBuffer();
		log_buffer.emplace_back(formatted_text.c_str(), type);
	}

	caller_name.clear();
}

void Log::Write(const std::string & text, const LogType & type) //������ nullüũ �ʿ� ����
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

	logger.lock()->Log(std::string(text), static_cast<uint>(type)); //lock = �������� -> shared_ptr

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

	fout.open(log_file_name, std::ofstream::out | std::ofstream::app); //append. ���������� �� Ŀ�� ��ġ�� �̾ ��

	if (fout.is_open())
	{
		fout << final_text << std::endl;
		fout.close();
	}
}
