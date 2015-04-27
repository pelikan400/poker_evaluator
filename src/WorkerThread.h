#ifndef WORKER_THREAD__H
#define WORKER_THREAD__H

#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

template < class T > class SynchronizedQueue
{
private:
  std::mutex qMutex;
  std::queue< T > q;
  std::condition_variable cv;
  
  void push( T& var )
  {
    {
      std::lock_guard< std::mutex > lock(qMutex);
      q.push( var );
    }
    
    cv.notify_one();
  }


  bool empty()
  {
    std::lock_guard< std::mutex > lock(qMutex);
    return q.empty();
  }


  T pop() {
    std::lock_guard< std::mutex > lock( qMutex );
    while( q.empty() ) {
      cv.wait( lock, [this] { return !q.empty(); } );
    }
    
    return q.pop();
  }
};

class Message 
{
   enum MessageType {
      PROGRESS_INDICATOR,
      COMMAND,
      STATUS
  };

  float progressIndicator1;
  float progressIndicator2;
  std::string shortText;
std::string text;
};



class WorkerThread : public std::thread
{
private:
   SynchronizedQueue< Message > mqOut;
   SynchronizedQueue< Message > mqIn;

public:
   virtual void run();

   WorkerThread() 
     : std::thread( [this] { return run(); } )
   {
   }
};


#endif
