// g++ -std=c++17 pvz.cpp -pthread -lstdc++fs
#include <condition_variable> 
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

mutex g_mutex;
condition_variable queueEmpty;
vector<string> buffer;

// struct result
// {
//   int min;
//   int max;
// };
// vector<result> res;

int minValue;
int maxValue;
bool initializeMinMax = true;

bool primeNumber(string myint1);
vector<string> open_file(const string& file_name);
void producer();
void consumer();

vector<string> open_file(const string& file_name)
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

bool primeNumber(string myint1)
{
  int n = stoi(myint1);
  bool isPrime = true;

    // 0 and 1 are not prime numbers
    if (n == 0 || n == 1) 
    {
        isPrime = false;
    }
    else 
    {
        for (int i = 2; i <= n / 2; ++i) 
        {
            if (n % i == 0) 
            {
                isPrime = false;
                break;
            }
        }
    }

  return isPrime;
}

void producer()
{
  string fileName = "rand_files/file";
  int fileNumber = 1;
  string fileExtension = ".txt";

  while (true) 
  {
    string path = fileName + to_string(fileNumber) + fileExtension ;
    vector<string> consumer_data = open_file(path);
    {
      unique_lock<mutex> ul(g_mutex);
      buffer = consumer_data;

      queueEmpty.notify_all();
    }
    fileNumber++;
  }
}

void consumeData(int data) { cout << "receive data: " << data << "\n"; }

void consumer() {
    string number;
    vector<string> consumerBuffer;
    while (true) {
    {
      unique_lock<mutex> ul(g_mutex);

      queueEmpty.wait(ul, [](){ return !buffer.empty(); });

      while(!buffer.empty())
      {
          number = buffer.at(buffer.size()-1);
          if( initializeMinMax)
          {
              minValue = stoi( number );
              maxValue = stoi( number );
              initializeMinMax = false;
          }
          if( primeNumber(number) )
          {
              int n = stoi(number);
              auto pairSeq= std::minmax({ n, minValue, maxValue});
              minValue = pairSeq.first;
              maxValue = pairSeq.second;
          }
          buffer.pop_back();
      }
      cout << "Minimum: " << minValue << " Maximum: " << maxValue << "\n";
    }
  }
}

void consumerThread() { consumer(); }

void producerThread() { producer(); }

int main() 
{
  thread t1(consumerThread);
  thread t2(producerThread);
  t1.join();
  t2.join();

  return 0;
}
