#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>
#include <memory>

#define INFORM 
// #define INFORM do {std::cout << __PRETTY_FUNCTION__ << std::endl;} while(0);

struct Node
{
  explicit Node(int v) : val(v) {};
  std::unique_ptr<Node> left{};
  std::unique_ptr<Node> right{};
  int val{};
};

struct promise_t
{
  promise_t()
  {
    INFORM;
  }
  ~promise_t()
  {
    INFORM;
  }
  auto get_return_object()
  {
    INFORM;
    return std::coroutine_handle<promise_t>::from_promise(*this);
  }
  std::suspend_never initial_suspend()
  {
    INFORM;
    return {};
  }
  std::suspend_always final_suspend() noexcept
  {
    INFORM;
    return {};
  }
  void return_void()
  {
    INFORM;
  }
  std::suspend_always yield_value(int val)
  {
    value = val;
    INFORM;
    return {};
  }
  void unhandled_exception()
  {
    std::terminate();
  }
  int value{};
};

struct ReturnObject
{
  using promise_type = promise_t;
  ~ReturnObject()
  {
    handle_.destroy();
    INFORM;
  }
  ReturnObject(std::coroutine_handle<promise_type> handle) : handle_(handle)
  {
    INFORM;
  }
  auto value()
  {
    INFORM;
    return handle_.promise().value;
  }
  bool done()
  {
    INFORM;
    return handle_.done();
  }
  void resume()
  {
    INFORM;
    handle_.resume();
  }

private:
  std::coroutine_handle<promise_type> handle_;
};

// struct Awaiter
// {
//   std::coroutine_handle<> &hp_;
//   bool await_ready() const noexcept
//   {
//     INFORM;
//     return false;
//   }
//   void await_suspend(std::coroutine_handle<> &h)
//   {
//     INFORM;
//     hp_ = h;
//   }
//   void await_resume() const noexcept
//   {
//     INFORM;
//   }
//   ~Awaiter()
//   {
//     INFORM;
//   }
// };

// ReturnObject counter(/*std::coroutine_handle<> &continuation_out*/)
// {
//   // Awaiter a{continuation_out};
//   for (unsigned i = 0;; ++i)
//   {
//     std::cout << "calling co_await\n";
//     // co_await a;
//     std::cout << "counter: " << i << std::endl;
//     co_return 123;
//   }
// }


ReturnObject InOrder(Node* node)
{
  if (node == nullptr) co_return;
  auto l = InOrder(node->left.get());
  while (!l.done())
  {
    l.resume();
    co_yield l.value();
  }
  co_yield node->val;
  auto r = InOrder(node->right.get());
  while (!r.done())
  {
    r.resume();
    co_yield r.value();
  }
}

int main()
{
  Node root(2);
  Node* p = &root;
  for (int i =0; i < 1000;++i)  {
    p->left = std::make_unique<Node>(-i);
    p->right = std::make_unique<Node>(-2*i);
    p=p->left.get();
  }
  Node root2(2);
  Node* p2 = &root2;
  for (int i =0; i < 100000; ++i)  {
    p2->left = std::make_unique<Node>(-i);
    p2->right = std::make_unique<Node>(-2*i);
    p2=p2->left.get();
  }
  // root.left = std::make_unique<Node>(-1);
  // root.right = std::make_unique<Node>(3);
  
  auto obj = InOrder(&root);

  while (!obj.done())
  {
    //std::cout << obj.value() << ' ';
    obj.value();
    obj.resume();
  }
}