#pragma once

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include "util.h"
#include "singleton.h"

#define CS_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        CraftServer::LogEventWrap(CraftServer::LogEvent::ptr(new CraftServer::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, CraftServer::GetThreadId(),\
                CraftServer::GetFiberId(), time(0)))).getSS()

#define CS_LOG_DEBUG(logger) CS_LOG_LEVEL(logger, CraftServer::LogLevel::DEBUG)
#define CS_LOG_INFO(logger) CS_LOG_LEVEL(logger, CraftServer::LogLevel::INFO)
#define CS_LOG_WARN(logger) CS_LOG_LEVEL(logger, CraftServer::LogLevel::WARN)
#define CS_LOG_ERROR(logger) CS_LOG_LEVEL(logger, CraftServer::LogLevel::ERROR)
#define CS_LOG_FATAL(logger) CS_LOG_LEVEL(logger, CraftServer::LogLevel::FATAL)

#define CS_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        CraftServer::LogEventWrap(CraftServer::LogEvent::ptr(new CraftServer::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, CraftServer::GetThreadId(),\
                CraftServer::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define CS_LOG_FMT_DEBUG(logger, fmt, ...) CS_LOG_FMT_LEVEL(logger, CraftServer::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define CS_LOG_FMT_INFO(logger, fmt, ...)  CS_LOG_FMT_LEVEL(logger, CraftServer::LogLevel::INFO, fmt, __VA_ARGS__)
#define CS_LOG_FMT_WARN(logger, fmt, ...)  CS_LOG_FMT_LEVEL(logger, CraftServer::LogLevel::WARN, fmt, __VA_ARGS__)
#define CS_LOG_FMT_ERROR(logger, fmt, ...) CS_LOG_FMT_LEVEL(logger, CraftServer::LogLevel::ERROR, fmt, __VA_ARGS__)
#define CS_LOG_FMT_FATAL(logger, fmt, ...) CS_LOG_FMT_LEVEL(logger, CraftServer::LogLevel::FATAL, fmt, __VA_ARGS__)

#define CS_LOG_ROOT() CraftServer::LoggerMgr::GetInstance()->getRoot()
#define CS_LOG_NAME(name) CraftServer::LoggerMgr::GetInstance()->getLogger(name)

namespace CraftServer {

	class Logger;
	class LoggerManager;

	//日志级别
	class LogLevel {
	public:
		enum Level {
			UNKNOW = 0,
			DEBUG = 1,
			INFO = 2,
			WARN = 3,
			ERROR = 4,
			FATAL = 5
		};

		static const char* ToString(LogLevel::Level level);
		static LogLevel::Level FromString(const std::string& str);
	};

	//日志事件
	class LogEvent {
	public:
		typedef std::shared_ptr<LogEvent> ptr;
		LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
			, const char* file, int32_t m_line, uint32_t elapse
			, uint32_t thread_id, uint32_t fiber_id, uint64_t time);

		const char* getFile() const { return m_file; }
		int32_t getLine() const { return m_line; }
		uint32_t getElapse() const { return m_elapse; }
		uint32_t getThreadId() const { return m_threadId; }
		uint32_t getFiberId() const { return m_fiberId; }
		uint64_t getTime() const { return m_time; }
		std::string getContent() const { return m_ss.str(); }
		std::shared_ptr<Logger> getLogger() const { return m_logger; }
		LogLevel::Level getLevel() const { return m_level; }

		std::stringstream& getSS() { return m_ss; }
		void format(const char* fmt, ...);
		void format(const char* fmt, va_list al);
	private:
		const char* m_file = nullptr;  //文件名
		int32_t m_line = 0;            //行号
		uint32_t m_elapse = 0;         //程序启动开始到现在的毫秒数
		uint32_t m_threadId = 0;       //线程id
		uint32_t m_fiberId = 0;        //协程id
		uint64_t m_time = 0;           //时间戳
		std::stringstream m_ss;

		std::shared_ptr<Logger> m_logger;
		LogLevel::Level m_level;
	};

	class LogEventWrap {
	public:
		LogEventWrap(LogEvent::ptr e);
		~LogEventWrap();
		LogEvent::ptr getEvent() const { return m_event; }
		std::stringstream& getSS();
	private:
		LogEvent::ptr m_event;
	};

	//日志格式器
	class LogFormatter {
	public:
		typedef std::shared_ptr<LogFormatter> ptr;
		LogFormatter(const std::string& pattern);

		//%t    %thread_id %m%n
		std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
	public:
		class FormatItem {
		public:
			typedef std::shared_ptr<FormatItem> ptr;
			virtual ~FormatItem() {}
			virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
		};

		void init();

		bool isError() const { return m_error;}
		const std::string getPattern() const { return m_pattern;}
	private:
		std::string m_pattern;
		std::vector<FormatItem::ptr> m_items;
		bool m_error = false;

	};

	//日志输出地
	class LogAppender {
	friend class Logger;
	public:
		typedef std::shared_ptr<LogAppender> ptr;
		virtual ~LogAppender() {}

		virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
		virtual std::string toYamlString() = 0;

		void setFormatter(LogFormatter::ptr val);
		LogFormatter::ptr getFormatter() const { return m_formatter; }

		LogLevel::Level getLevel() const { return m_level; }
		void setLevel(LogLevel::Level val) { m_level = val; }
	protected:
		LogLevel::Level m_level = LogLevel::DEBUG;
		bool m_hasFormatter = false;
		LogFormatter::ptr m_formatter;
	};

	//日志器
	class Logger : public std::enable_shared_from_this<Logger> {
	/*std::enable_shared_from_this 是一个模板类，它定义在 <memory> 头文件中。这个类的作用是允许一个对象（通常是在堆上分配的）在其生命周期中安全地获取指向自身的 std::shared_ptr。通常情况下，当我们想要在对象的成员函数中返回指向自身的 std::shared_ptr 时，我们会遇到一个问题，即无法直接在成员函数中使用 this 指针创建 std::shared_ptr，因为 this 指针不会增加对象的引用计数，可能会导致悬空指针或者内存泄漏。
	通过继承 std::enable_shared_from_this，Logger 类可以通过调用 shared_from_this() 函数来获得指向自身的 std::shared_ptr。这个函数会返回一个 std::shared_ptr 对象，该对象与当前对象共享所有权，从而确保对象在共享指针的引用计数归零之前不会被销毁。

	在使用 std::enable_shared_from_this 时，有两个重要的约束 ：
	1.对象必须通过 std::shared_ptr 进行动态分配，不能直接栈上分配。
	2.使用 shared_from_this() 函数的前提是，已经存在至少一个指向对象的 std::shared_ptr，否则会抛出 std::bad_weak_ptr 异常。

	在 Logger 类中，可能会利用这个特性，例如在某些异步回调函数中需要共享 Logger 对象的情况下，可以安全地获取指向自身的 std::shared_ptr。*/
	friend class LoggerManager;
	public:
		typedef std::shared_ptr<Logger> ptr;

		Logger(const std::string& name = "root");
		void log(LogLevel::Level level, LogEvent::ptr event);

		void debug(LogEvent::ptr event);
		void info(LogEvent::ptr event);
		void warn(LogEvent::ptr event);
		void error(LogEvent::ptr event);
		void fatal(LogEvent::ptr event);

		void addAppender(LogAppender::ptr appender);
		void delAppender(LogAppender::ptr appender);

		void clearAppenders();

		LogLevel::Level getLevel() const { return m_level; }
		void setLevel(LogLevel::Level val) { m_level = val; }

		const std::string& getName() const { return m_name; }

		void setFormatter(LogFormatter::ptr val);
		void setFormatter(const std::string& val);
		LogFormatter::ptr getFormatter();

		std::string toYamlString();
	private:
		std::string m_name;                     //日志名称
		LogLevel::Level m_level;                //日志级别
		std::list<LogAppender::ptr> m_appenders;//Appender集合
		LogFormatter::ptr m_formatter;
		Logger::ptr m_root;
	};

	//输出到控制台的Appender
	class StdoutLogAppender : public LogAppender {
	public:
		typedef std::shared_ptr<StdoutLogAppender> ptr;
		void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
		std::string toYamlString() override;
	};

	//定义输出到文件的Appender
	class FileLogAppender : public LogAppender {
	public:
		typedef std::shared_ptr<FileLogAppender> ptr;
		FileLogAppender(const std::string& filename);
		void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
		std::string toYamlString() override;

		//重新打开文件，文件打开成功返回true
		bool reopen();
	private:
		std::string m_filename;
		std::ofstream m_filestream;
	};

	class LoggerManager {
	public:
		LoggerManager();
		Logger::ptr getLogger(const std::string& name);

		void init();
		Logger::ptr getRoot() const { return m_root; }

		std::string toYamlString();
	private:
		std::map<std::string, Logger::ptr> m_loggers;
		Logger::ptr m_root;
	};

	typedef CraftServer::Singleton<LoggerManager> LoggerMgr;

}