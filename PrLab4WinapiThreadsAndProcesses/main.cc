#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "Thread.hh"
#include "Process.hh"
#include <thread>
using namespace std;
using namespace std::string_literals;
using namespace std::chrono;
using namespace chrono_literals;
using clck = chrono::high_resolution_clock;

constexpr auto ITERS = 4'000'000'000ul;
DWORD WINAPI doStuff(LPVOID arg) {
	auto iters = ITERS;
	const auto sleepTime = 2s;
	const auto start = clck::now();
	this_thread::sleep_for(sleepTime);
	auto tmp = 0ull;
	while (iters--) tmp = 7 * iters + tmp;
	const auto time = duration_cast<milliseconds>(clck::now() - start - sleepTime).count();
	cout << int(arg) << " T: " << time << "\tP: " << GetThreadPriority(GetCurrentThread()) << endl;
	return 0;
}
void measureProcessesTime() {
	cout << "Testing processes" << endl;
	auto procs = vector<Process>{};
	auto testProgram = R"(C:\Users\kamil\Desktop\app.exe)"s;
	procs.emplace_back(testProgram, testProgram + " " + to_string(ITERS) + " " + to_string(1), REALTIME_PRIORITY_CLASS);
	procs.emplace_back(testProgram, testProgram + " " + to_string(ITERS) + " " + to_string(2), HIGH_PRIORITY_CLASS);
	procs.emplace_back(testProgram, testProgram + " " + to_string(ITERS) + " " + to_string(3), NORMAL_PRIORITY_CLASS);
	procs.emplace_back(testProgram, testProgram + " " + to_string(ITERS) + " " + to_string(4), IDLE_PRIORITY_CLASS);


	for (auto&p : procs) p.run();
	for (auto&p : procs) p.waitAndGet();
	cout << "Done" << '\n' << string(20, '*') << '\n' << endl;
}

void measureThreadsTime() {
	cout << "Testing threads" << endl;
	auto threads = vector<Thread>{};
	threads.emplace_back(doStuff, LPVOID(1), THREAD_PRIORITY_ABOVE_NORMAL);
	threads.emplace_back(doStuff, LPVOID(2), THREAD_PRIORITY_BELOW_NORMAL);
	threads.emplace_back(doStuff, LPVOID(3), THREAD_PRIORITY_HIGHEST);
	threads.emplace_back(doStuff, LPVOID(4), THREAD_PRIORITY_IDLE);
	threads.emplace_back(doStuff, LPVOID(5), THREAD_PRIORITY_NORMAL);
	threads.emplace_back(doStuff, LPVOID(6), THREAD_PRIORITY_TIME_CRITICAL);
	for (auto &t : threads) t.run();
	for (auto &t : threads) t.waitAndGet();
	cout << "Done" << '\n' << string(20, '*') << '\n' << endl;
}



int main(int argc, char* argv[]) {
	//Process{ argv[0], argv[0], REALTIME_PRIORITY_CLASS }.run();
	auto notepadExe = R"(C:\windows\system32\notepad.exe)"s;
	auto notepadProcess = Process{ notepadExe, notepadExe };
	notepadProcess.run();
	this_thread::sleep_for(3s);
	notepadProcess.terminate();
	auto th = Thread{
		[](LPVOID arg)->DWORD {cout << "I am in a new thread with argument: " << int(arg) << endl; this_thread::sleep_for(2s); return 222; },
		LPVOID(12)
	};
	th.run();
	cout << "Thread has returned: " << th.waitAndGet() << endl;

	auto additionalStressThreadsNumber = 0;
	auto stressThreads = vector<Thread>{};
	for (int i = 0; i < additionalStressThreadsNumber; ++i)
		stressThreads.emplace_back([](LPVOID arg)->DWORD {auto i = 1ull; while (true) i = 7 * i + i / 3; } , nullptr, THREAD_PRIORITY_TIME_CRITICAL);
	for (auto & t : stressThreads) t.run();

	measureProcessesTime();
	measureThreadsTime();

	for (auto & t : stressThreads) t.terminate();
	system("pause");
	return 0;
}
