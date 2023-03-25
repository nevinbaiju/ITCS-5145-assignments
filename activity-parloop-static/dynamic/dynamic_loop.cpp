#ifndef __DYNAMIC_LOOP_H
#define __DYNAMIC_LOOP_H

#include <functional>
#include <mutex>
#include <thread>
#include <iostream>

class DynamicLoop {
private:
  // @brief you will want to have class member to store the number of threads
  // add a public setter function for this member.
  int num_threads;
  int granularity;
  std::mutex after_mutex;
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


  // template<typename TLS>
  static void perform_reduce(int start, int end, size_t increment
	      //  std::function<void(TLS&)> before,
	      //  std::function<void(int, TLS&)> f,
	      //  std::function<void(TLS&)> after
         ){
		  //  TLS tls;
      //  before(tls);
      //  for(int i=start; i<end; i+=increment){
      //     f(i, tls);
      //  }
      //  after_mutex.lock();
      //  after(tls);
      //  after_mutex.unlock();
      std::cout << start << " " << end << std::endl;
	}

  static void demo(int start, int end){
    for(int i=start; i<end; i++){
      std::cout << i << std::endl;
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
    

    std::vector<std::thread> threads;
    
    // bool thread_empty_status[num_threads];
    // for (int j=0; j<num_threads; j++)
    // {
    //     thread_empty_status[j] = true;
    // }

    auto reduce_lambda = [&](int start, int end, int increment) -> void{
		   TLS tls;
       before(tls);
       for(int i=start; i<end; i+=increment){
          f(i, tls);
       }
       after_mutex.lock();
       after(tls);
       after_mutex.unlock();
		 };

    int i = beg;
    int chunk_beg, chunk_end;

    while(i < end){
      // for (int thread_id=0; thread_id<num_threads; thread_id++){
      //   std::cout << "Visiting thread id " << thread_id << std::endl;
        // if(thread_empty_status[thread_id]){
        //   std::cout << "THread " << thread_id << std::endl;
        //   thread_empty_status[thread_id] = false;
        //   chunk_beg = i;
        //   chunk_end = std::min(i+granularity, (int)end);
        //   threads[thread_id] = std::move(std::thread(reduce_lambda, chunk_beg, chunk_end, increment, thread_id));
        //   i = i+granularity;
        // }
        chunk_beg = i;
        chunk_end = std::min(i+granularity, (int)end);
        // std::cout << chunk_beg << " " << chunk_end << std::endl;
        threads.push_back(std::move(std::thread(reduce_lambda, chunk_beg, chunk_end, increment)));
        i = i+granularity;
        if (threads.size() >= num_threads) {
            // wait for the threads to finish before spawning more
            for (auto& t : threads) {
                t.join();
            }
            threads.clear();
        }
      // }
    }
    for (auto& t : threads) {
      t.join();
    }
  }
  
  
};

#endif
