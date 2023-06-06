#pragma once
#ifndef NANO_LOG_HEADER_GUARD
#define NANO_LOG_HEADER_GUARD

#include <cstdint>
#include <memory>
#include <string>
#include <iosfwd>
#include <type_traits>

namespace nanolog {
	enum class LogLevel : uint8_t { INFO, WARN, CRIT };

	class NanoLogLine {
	public:
		NanoLogLine(LogLevel level, char const* file, char const* function, uint32_t line);
		~NanoLogLine();

		NanoLogLine(NanoLogLine&&) = default;
		NanoLogLine& operator=(NanoLogLine&&) = default;

		void stringify(std::ostream& os);

		NanoLogLine& operator<<(char arg);
		NanoLogLine& operator<<(uint32_t arg);
		NanoLogLine& operator<<(int32_t arg);
		NanoLogLine& operator<<(uint64_t arg);
		NanoLogLine& operator<<(int64_t arg);
		NanoLogLine& operator<<(double arg);
		NanoLogLine& operator<<(std::string const& arg);

		template <size_t N>
		NanoLogLine& operator<<(const char(&arg)[N]) {
			encode(string_literal_t(arg));
			return *this;
		}

		template <typename Arg>
		typename std::enable_if<std::is_same<Arg, char const* >::value, NanoLogLine&>::type//ֻ�е� Arg �������� char const *���� C ����ַ�����ʱ����ʹ��������ڱ���ʱ��Ч��
		operator<<(Arg const& arg) {
			encode(arg);
			return *this;
		}

		template <typename Arg>
		typename std::enable_if<std::is_same<Arg, char* >::value, NanoLogLine& >::type
		operator<<(Arg const& arg) {
			encode(arg);
			return *this;
		}

		//�ļ�������
		struct string_literal_t {
			explicit string_literal_t(char const* s) : m_s(s) {}
			char const* m_s;
		};

	private:
		char* buffer();

		template < typename Arg >
		void encode(Arg arg);

		template < typename Arg >
		void encode(Arg arg, uint8_t type_id);

		void encode(char* ananorg);
		void encode(char const* arg);
		void encode(string_literal_t arg);
		void encode_c_string(char const* arg, size_t length);
		void resize_buffer_if_needed(size_t additional_bytes);
		void stringify(std::ostream& os, char* start, char const* const end);

	private:
		size_t m_bytes_used;
		size_t m_buffer_size;
		std::unique_ptr<char[]> m_heap_buffer;
		//��������Ĺ��캯�����Ա������ʹ�ù̶���С��������������Ҫ�������������ֶ��ͷ��ڴ棬��Ϊ���ǵ��ڴ�����ɶ���Ĵ��������ٸ���
		//ʹ����new��̬�������Ҫ�ֶ��ͷ�
		char m_stack_buffer[256 - 2 * sizeof(size_t) - sizeof(decltype(m_heap_buffer)) - 8];
	};

	struct NanoLog {
		bool operator==(NanoLogLine&);//����һ����־
	};

	void set_log_level(LogLevel level); /*{
		loglevel.store(static_cast<unsigned int>(level), std::memory_order_release);
	}*/

	//�жϸ���־����־�����Ƿ���ڵ��ڵ�ǰϵͳ���õ���־���𣬴��ڵ��ڲż�¼
	bool is_logged(LogLevel level); /*{
		return static_cast<unsigned int>(level) >= loglevel.load(std::memory_order_relaxed);
	}*/

	struct NonGuaranteedLogger {
		NonGuaranteedLogger(uint32_t ring_buffer_size_mb_) : ring_buffer_size_mb(ring_buffer_size_mb_) {}
		uint32_t ring_buffer_size_mb;
	};

	struct GuaranteedLogger{
	};

	void initialize(GuaranteedLogger gl, std::string const& log_directory, std::string const& log_file_name, uint32_t log_file_roll_size_mb);
	/*{
		nanologger.reset(new NanoLogger(gl, log_directory, log_file_name, log_file_roll_size_mb));
		atomic_nanologger.store(nanologger.get(), std::memory_order_seq_cst);
	}*/

	void initialize(NonGuaranteedLogger ngl, std::string const& log_directory, std::string const& log_file_name, uint32_t log_file_roll_size_mb);
	/*{
		nanologger.reset(new NanoLogger(ngl, log_directory, log_file_name, log_file_roll_size_mb));
		atomic_nanologger.store(nanologger.get(), std::memory_order_seq_cst);
	}*/
}

//����һ����־
#define NANO_LOG(LEVEL) nanolog::NanoLog() == nanolog::NanoLogLine(LEVEL, __FILE__, __func__, __LINE__)
//�жϸ���־�����Ƿ���ڵ���ϵͳ������־���𣬴��ڵ�����д�뻺����
#define LOG_INFO nanolog::is_logged(nanolog::LogLevel::INFO) && NANO_LOG(nanolog::LogLevel::INFO)
#define LOG_WARN nanolog::is_logged(nanolog::LogLevel::WARN) && NANO_LOG(nanolog::LogLevel::WARN)
#define LOG_CRIT nanolog::is_logged(nanolog::LogLevel::CRIT) && NANO_LOG(nanolog::LogLevel::CRIT)

#endif