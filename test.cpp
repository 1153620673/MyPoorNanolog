#include"NanoLog.h"
#include<chrono>
#include<vector>
#include<thread>
#include<atomic>
#include<cstdio>

//int main() {
//	nanolog::initialize(nanolog::GuaranteedLogger(), "C:/Users/Armand/Desktop/NanoLog/Project1/test/", "nanolog", 1);
//
//	for (int i = 0; i < 5; i++) {
//		LOG_INFO << "Sample NanoLog : " << i;
//	}
//
//	nanolog::set_log_level(nanolog::LogLevel::CRIT);
//	LOG_WARN << "This log line will not be logged since we are at loglevel = CRIT";
//	return 0;
//}

uint64_t timestamps_now() {
	return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::microseconds(1);
}

void nanolog_benchmark() {
	int const iterations = 100000;
	char const* const benchmark = "benchmark";
	uint64_t begin = timestamps_now();
	for (int i = 0; i < iterations; i++) {
		LOG_INFO << "Logging" << benchmark << i << 0 << 'K' << -42.42;
	}
	uint64_t end = timestamps_now();
	long int avg_latency = (end - begin) * 1000 / iterations;
	printf("\tAverage NanoLog Latency =  %ld nanoseconds\n", avg_latency);
}

template <typename Function>
void run_benchmark(Function&& f, int thread_count) {
	printf("Thread count: %d\n", thread_count);
	std::vector<std::thread> threads;
	for (int i = 0; i < thread_count; i++) {
		threads.emplace_back(f);
	}
	for (int i = 0; i < thread_count; i++) {
		threads[i].join();
	}
}

int main() {
	nanolog::initialize(nanolog::NonGuaranteedLogger(10), "C:/Users/Armand/Desktop/NanoLog/Project1/test/", "0nanolog", 1);
	for (auto threads : { 1, 2, 3, 4, 5 }) {
		run_benchmark(nanolog_benchmark, threads);
	}
	nanolog::set_log_level(nanolog::LogLevel::CRIT);
	LOG_WARN << "This log line will not be logged since we are at loglevel = CRIT";
	return 0;
}