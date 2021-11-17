#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <memory>
#include <mutex>


template<typename T>
class Singleton {
public:
 
  //Soctt Meyer type
//   static T& Instance_1() {
//     static T t;
//     return t;
//   }

  // Eager type
//   static T& Instance_eager() {
//       return *t_e;
//   }
  
  // Lazy type
  static T& Instance() {
      if(t_ == nullptr) {
          std::unique_lock<std::mutex> lock(mutex_);
          if(t_ == nullptr) {
              t_ = std::unique_ptr<T>(new T());
          }
      }

      return *t_;
  }

  Singleton(const T&) = delete;
  Singleton& operator=(const T&) = delete;
  Singleton(const T&&) = delete;

protected:
  Singleton() = default;
  virtual ~Singleton() = default;

private:
  static std::unique_ptr<T> t_;
  static std::mutex mutex_;

//   static T* t_e;
};

template<typename T> std::unique_ptr<T> Singleton<T>::t_;
template<typename T> std::mutex Singleton<T>::mutex_;

// template<typename T> T* Singleton<T>::t_e = new (std::nothrow) T();

#endif
