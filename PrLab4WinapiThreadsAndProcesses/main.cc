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

DWORD WINAPI doStuff(LPVOID arg) {
	auto iters = 2'000'000'000ul;
	const auto sleepTime = 2s;
	const auto start = clck::now();
	this_thread::sleep_for(sleepTime);
	Sleep(2000);
	while (iters--) auto div = iters / 2.0;
	const auto time = duration_cast<milliseconds>(clck::now() - start - sleepTime).count();
	cout << int(arg) << " T: " << time << "\tP: " << GetThreadPriority(GetCurrentThread()) << endl;
	return 0;
}
void measureProcessesTime() {
	auto iters = 2'000'000'000ul;
	cout << "Testing processes" << endl;
	auto procs = vector<Process>{};
	auto testProgram = R"(D:\app.exe)"s;
	procs.emplace_back(testProgram, testProgram + " " + to_string(iters) + " " + to_string(1), REALTIME_PRIORITY_CLASS);
	procs.emplace_back(testProgram, testProgram + " " + to_string(iters) + " " + to_string(2), HIGH_PRIORITY_CLASS);
	procs.emplace_back(testProgram, testProgram + " " + to_string(iters) + " " + to_string(3), NORMAL_PRIORITY_CLASS);
	procs.emplace_back(testProgram, testProgram + " " + to_string(iters) + " " + to_string(4), IDLE_PRIORITY_CLASS);
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
	// FORKBOMB
	//Process{ argv[0], argv[0], IDLE_PRIORITY_CLASS }.run();
	auto th = Thread {
		[](LPVOID arg)->DWORD {cout << "I am in a new thread with argument: " << int(arg) << endl; Sleep(2000); return 222; },
		LPVOID(12)
	};
	th.run();
	cout << "Thread has returned: " <<  th.waitAndGet() << endl;
	measureProcessesTime();
	measureThreadsTime();
	system("pause");
	
	return 0;
}
