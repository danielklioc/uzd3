// g++ -std=c++17 pvz.cpp -pthread -lstdc++fs
#include <condition_variable> 
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <experimental/filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;

std::mutex g_mutex;
std::condition_variable queueEmpty;
vector<string> buffer;
bool read_file = false;

string path = "rand_files/file1.txt";

struct result
{
  int min;
  int max;
};

vector<result> res;

bool g_ready = false;
int g_data = 0;

std::vector<string> open_file(const string& file_name)
{
  string new_value;
  vector<string> file_contents;
  ifstream file(file_name, ios::in);

  while(!file.eof())
  {
    file >> new_value;
    file_contents.push_back(new_value);
  }
  return file_contents;
}

void put_string(const string& str)
{

    buffer.push_back(str);
}

void producer() {
  while (true) {
    vector<string> consumer_data = open_file(path);
    {
      std::unique_lock<std::mutex> ul(g_mutex);
      if( false == read_file)
      {
        buffer = consumer_data;
        read_file = true;
      }
      else
      {

      }

      queueEmpty.notify_all();
    }
  }

}

void consumeData(int data) { std::cout << "receive data: " << data << "\n"; }

void consumer() {
  while (true) {
    std::string message;
    {
      std::unique_lock<std::mutex> ul(g_mutex);

        queueEmpty.wait(ul, [](){
          return !buffer.empty(); 
        });

      message = buffer.at(buffer.size()-1);
      buffer.pop_back();

      for (auto i = buffer.begin(); i != buffer.end(); ++i)
        cout << *i << " ";
        buffer.pop_back();
    }
    
    // parse prime numbers and find min max 
    // cout<<message<<endl;
  }
}

void primeNumber(string i,int n)
{
  int myint1 = stoi(i);
  bool isPrime = true;

    cout << "Enter a positive integer: ";
    cin >> n;

    // 0 and 1 are not prime numbers
    if (n == 0 || n == 1) {
        isPrime = false;
    }
    else {
        for (i = 2; i <= n / 2; ++i) {
            if (n % i == 0) {
                isPrime = false;
                break;
            }
        }
    }
    if (isPrime)
        cout << n << " is a prime number";
    else
        cout << n << " is not a prime number";
}


void consumerThread() { consumer(); }

void producerThread() { producer(); }

int main() {

  fs::path pathToShow(/* ... */);

  std::thread t1(consumerThread);
  std::thread t2(producerThread);
  t1.join();
  t2.join();
  return 0;
}
