#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>

struct Node
{
  Node *left{};
  Node *right{};
  int val;
};

struct ReturnObject
{
  struct promise_type
  {
    ReturnObject get_return_object()
    {
      std::cout << "get_return_object" << std::endl;
      return {};
    }
    std::suspend_never initial_suspend()
    {
      std::cout << "initial_suspend" << std::endl;
      return {};
    }
    std::suspend_never final_suspend() noexcept
    {
      std::cout << "final_suspend" << std::endl;
      return {};
    }
    void return_value(int val)
    {
      std::cout << val << std::endl;
    }
    void yield_value(int val)
    {
      std::cout << val << std::endl;
    }
    void unhandled_exception() {}
  };
  ~ReturnObject()
  {
    std::cout << "~ReturnObject\n";
  }
  ReturnObject()
  {
    std::cout << "ReturnObject\n";
  }
};

struct Awaiter
{
  std::coroutine_handle<> &hp_;
  bool await_ready() const noexcept
  {
    std::cout << "await_ready\n";
    return false;
  }
  void await_suspend(std::coroutine_handle<> &h)
  {
    std::cout << "await_suspend\n";
    hp_ = h;
  }
  void await_resume() const noexcept
  {
    std::cout << "await_resume\n";
  }
  ~Awaiter()
  {
    std::cout << "~Awaiter\n";
  }
};

ReturnObject counter(/*std::coroutine_handle<> &continuation_out*/)
{
  // Awaiter a{continuation_out};
  for (unsigned i = 0;; ++i)
  {
    std::cout << "calling co_await\n";
    // co_await a;
    std::cout << "counter: " << i << std::endl;
    co_return 123;
  }
}

int main()
{
  auto obj = counter();

  // std::coroutine_handle<> h;
  // counter(h);
  // for (int i = 0; i < 3; ++i)
  // {
  //   std::cout << "In main1 function\n";
  //   h();
  // }
  // std::cout << "end of loop" << std::endl;
  // h.destroy();
  std::cout << "done" << std::endl;
}