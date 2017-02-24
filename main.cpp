#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <atomic>
#include <mutex>
#include "CTPL/ctpl.h"

typedef std::chrono::steady_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::nanoseconds ns;
typedef std::chrono::duration<double> fsec;
typedef unsigned char type;
unsigned int size = 1024;
unsigned int number_of_threads = 1;
unsigned long number_of_packets = 12;
bool is_tp = false;
bool is_mb = false;
using namespace std;

void parse(const std::vector<std::string> & args) {
	auto it = std::find(args.begin(), args.end(), "-j");
	if(it != args.end())
		number_of_threads = atoi((++it)->c_str());

	it = std::find(args.begin(), args.end(), "-s");
	if(it != args.end())
		size = atoi((++it)->c_str());

	it = std::find(args.begin(), args.end(), "-n");
	if(it != args.end())
		number_of_packets = atoi((++it)->c_str());

	it = std::find(args.begin(), args.end(), "-tp");
	if(it != args.end())
		is_tp = true;

	it = std::find(args.begin(), args.end(), "-mb");
	if(it != args.end())
		is_mb = true;
}

int main(int argc, char **argv) {
	vector <string> args(argc);
	for (int i=0; i<argc; ++i) args.push_back(argv[i]);
	parse(args);
	auto xor_test = [](type* tab, unsigned int begin, unsigned int end) {asm volatile("" : : "g"(tab) : "memory"); for(int i=begin; i<end; i++) tab[i] ^= i;};
	auto xor_test2 = [](int id, type* tab, unsigned int begin, unsigned int end) {asm volatile("" : : "g"(tab) : "memory"); for(int i=begin; i<end; i++) tab[i] ^= i;};

	unsigned long iterations = is_mb ? number_of_packets/number_of_threads : number_of_packets;

	type *tab = new type[size];

	type **tab2 = new type*[number_of_threads];

	for(int j=0; j<number_of_threads; j++)
		tab2[j] = new type[size];

	for(int j=0; j<number_of_threads; j++)
		for(int i=0; i<size; i++)
			tab2[j][i] = ((7+j)*i) % (sizeof(type)*256);

	for(int i=0; i<size; i++)
		tab[i] = (7*i) % (sizeof(type)*256);

	vector<thread*> threads(number_of_threads);

	auto start_time = Time::now();

	if( is_tp )
	{
		ctpl::thread_pool p(number_of_threads);
		for(int j = 0; j<iterations; j++)
		{
			for(int i=0; i<number_of_threads; i++)
				if(is_mb)
					p.push(xor_test2, tab2[i], 0, size);
				else
					p.push(xor_test2, tab, (size/number_of_threads)*i, (size/number_of_threads)*(i+1));
		}
	}
	else
	{
		for(int j = 0; j<iterations; j++)
		{
			for(int i=0; i<number_of_threads; i++)
				if(is_mb)
					threads.at(i) = new thread(xor_test, tab2[i], 0, size);
				else
					threads.at(i) = new thread(xor_test, tab, (size/number_of_threads)*i, (size/number_of_threads)*(i+1));

			for(int i=0; i<number_of_threads; i++)
			{
				threads.at(i)->join();
				delete threads.at(i);
			}
		}
	}

	auto end_time = Time::now();
	fsec time = end_time - start_time;
	ms m = std::chrono::duration_cast<ms>(time);
	ns n = std::chrono::duration_cast<ns>(time);
	cout << "Thread pool: " << is_tp	<< endl;
	cout << "Multi buffers: " << is_mb	<< endl;
	cout << "Number of threads: " << number_of_threads << endl;
	cout << "number_of_packets: " << number_of_packets << endl;
	cout << "tab size: " << size << endl;
	cout << "Time: " << n.count() << " ns" << endl;
	cout << "Time: " << m.count() << " ms" << endl;
//	cout << ((static_cast<double>(size) * static_cast<double>(sizeof(type)) * 8.) / (1024. * 1024.)) / time.count() << " Mbps" << endl;

	//for(int i=0; i<number_of_threads; i++)
	//        delete threads[i];

	delete[] tab;

	for(int j=0; j<number_of_threads; j++)
		delete[] tab2[j];

	delete tab2;

	return 0;
}

void foo(){}

int main2(int argc, char **argv) {
	//int a = 1;
	thread t([](){volatile int a =5;});
	//thread t([](){std::this_thread::sleep_for(std::chrono::seconds(2));});
	//thread t(foo);
	//thread t([&](){a=10;});
	//cout << a << endl;
	t.join();
	return 0;
}
