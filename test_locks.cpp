// test_locks.cpp 
//

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <thread>
#include <mutex>
#include <map>

using namespace std;

int64_t  __start_tm = 0;
int64_t  __latest_tm = 0;
int64_t  __now64 = 0;
int32_t  __now = 0 ;
int32_t  __thread_id_ticker = 100; 

int64_t timeGetTime64() {
  __latest_tm = std::chrono::system_clock::now().time_since_epoch() / std::chrono::microseconds(1);
  if (__start_tm == 0) __start_tm = __latest_tm;
  return (__now64 = (__latest_tm - __start_tm));
} // :: timeGetTime64

int32_t thread_self()
{
  return (++__thread_id_ticker) ;
} // :: thread_self

#if defined(_WIN32) || defined(_WIN64)
#  include <windows.h>
#  include <mmsystem.h>  // brings in timeGetTime
#  include <stdint.h>

#else

int32_t timeGetTime() {
  timeGetTime64();
  __now = __now64 / 1000;
  return __now;
} // :: timeGetTime

#endif

void u_sleep(uint32_t ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
} // :: u_sleep


class spin
{
private:
  std::atomic_flag lock_ = ATOMIC_FLAG_INIT;

public:
  spin() noexcept {}

  spin(const spin&) = delete;
  spin& operator=(const spin&) = delete;

  void lock() noexcept {
    while (lock_.test_and_set(std::memory_order_acquire))
      ;
  }

  void unlock() noexcept {
    lock_.clear(std::memory_order_release);
  }
}; // class spin 

typedef map<uint32_t, uint32_t>               ThreadMap;
typedef map<uint32_t, uint32_t>::iterator     ThreadMap_iter ;
typedef map<uint32_t, uint32_t>::value_type   ThreadMap_pair ;

#define  nTHREADS       4 
#define  nITERATIONS    1000000

mutex      tids_mtx;
spin       tids_spx;
ThreadMap  tids;
int32_t    ticker = 0;
bool       start = false;

void display()
{
  for (auto x : tids)
  {
    printf("%3d   %6d\n", x.first, x.second);
  }
} // :: display

void consume_mtx()
{
  uint32_t  tid = thread_self();
  {
    lock_guard<mutex>  sc(tids_mtx);
    tids[tid] = 0;
  }
  while (!start) { u_sleep(1); }
  while (ticker < nITERATIONS)
  {
    lock_guard<mutex>  sc(tids_mtx);
    tids[tid]++;
    ticker++;
  }
} // :: consume_mtx

void consume_spx()
{
  uint32_t  tid = thread_self();
  {
    lock_guard<spin>  sc(tids_spx);
    tids[tid] = 0;
  }
  while (!start) { u_sleep(1) ; }
  while (ticker < nITERATIONS)
  {
    lock_guard<spin>  sc(tids_spx);
    tids[tid]++;
    ticker++;
  }
} // :: consume_spx

void consume_nolock()
{
  uint32_t  tid = thread_self();
  {
    lock_guard<mutex>  sc(tids_mtx);
    tids[tid] = 0;
  }
  while (!start) { u_sleep(1); }
  while (ticker < nITERATIONS)
  {
    tids[tid]++;
    ticker++;
  }
} // :: consume_nolock


void test_mutex( void (*fn)() )
{
  start = false;
  ticker = 0;
  vector<thread*>  t_vec;
  for (auto i = 0; i < nTHREADS; i++)
  {
    t_vec.push_back(new thread([fn] { (*fn)(); }));
  }

  int64_t start_tm = timeGetTime64();
  start = true;

  while (ticker < nITERATIONS)
  {
    u_sleep(10);
  }

  for (auto t : t_vec)
  {
    t->join();
  }

  int64_t end_tm = timeGetTime64();

  int64_t  dt = end_tm - start_tm;
#if defined(_WIN32) || defined(_WIN64)
  printf("task completed in %I64d usec \n", dt);
#else
  printf("task completed in %I64ld usec \n", dt);
#endif
} // :: test_mutex

int main()
{
  printf("====[]  mutex\n");
  u_sleep(50); // let everything get ready
  tids.clear();
  test_mutex( consume_mtx );
  display();

  printf("====[]  spin_lock\n");
  u_sleep(50); // let everything get ready
  tids.clear();
  test_mutex(consume_spx);
  display();

  printf("====[]  nolock\n");
  u_sleep(50); // let everything get ready
  tids.clear();
  test_mutex(consume_nolock);
  display();

  return 0;
} // :: main
