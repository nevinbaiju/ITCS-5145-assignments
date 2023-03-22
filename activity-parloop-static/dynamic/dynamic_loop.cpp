#ifndef __DYNAMIC_LOOP_H
#define __DYNAMIC_LOOP_H

#include <functional>
#include <mutex>
#include <thread>

class DynamicLoop {
private:
  // @brief you will want to have class member to store the number of threads
  // add a public setter function for this member.
  int num_threads;
  int granularity;
public:
  // @breif write setters here.
  DynamicLoop(int num_threads, int granularity)
    : num_threads(num_threads), granularity(granularity)
    {}


  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel
  void parfor (size_t beg, size_t end, size_t inc,
	       std::function<void(int)> f) {
    for (size_t i=beg; i<end; i+= inc) {
      f(i);
    }
  }


  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel.
  ///
  /// Each thread that participate in the calculation will have its
  /// own TLS object.
  ///
  /// Each thread will execute function before prior to any
  /// calculation on a TLS object.
  ///
  /// Each thread will be passed the TLS object it executed before on.
  ///
  /// Once the iterations are complete, each thread will execute after
  /// on the TLS object. No two thread can execute after at the same time.
  template<typename TLS>
  void parfor (size_t beg, size_t end, size_t increment,
	       std::function<void(TLS&)> before,
	       std::function<void(int, TLS&)> f,
	       std::function<void(TLS&)> after
	       ) {
    std::mutex after_mutex;
    // TLS tls;
    // before(tls); 
    
    // for (size_t i=beg; i<end; i+= increment) {
    //   f(i, tls);
    // }
    // after(tls);

    std::vector<std::thread> integration_threads;
    int partition_size;
    if ((end-beg) < granularity){
      partition_size = (end-beg)-1;
    }
    else{
      partition_size = (end-beg)/granularity;
    }
    for(int i=beg; i<end; i+=partition_size){
      if (integration_threads.size() == num_threads){
        while(integration_threads.size() != 0){
          int last_index = integration_threads.size()-1;
          integration_threads[last_index].join();
          integration_threads.pop_back();
        }
      }
      int chunk_beg = i;
      int chunk_end = std::min(i+partition_size, (int)end);
      integration_threads.push_back(std::move(std::thread([&](int start, int end, int increment) -> void{
		   TLS tls;
       before(tls);
       for(int i=start; i<end; i+=increment){
          f(i, tls);
       }
       after_mutex.lock();
       after(tls);
       after_mutex.unlock();
		 }, chunk_beg, chunk_end, increment)));
    }
    for(int i=0; i<integration_threads.size(); i++){
      integration_threads[i].join();
    }
  }
  
  
};

#endif
