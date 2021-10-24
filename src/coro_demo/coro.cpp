#include <limits>
#include <benchmark/benchmark.h>

#include <boost/coroutine2/coroutine.hpp>
#include <coroutine>
#include <iostream>
#include <memory>

#include "src/cppcoro/generator.h"
#include "tensorflow/core/platform/default/logging.h"

#define INFORM
// #define INFORM do {std::cout << __PRETTY_FUNCTION__ << std::endl;} while(0);

struct Node {
  explicit Node(int v) : val(v){};
  std::unique_ptr<Node> left{};
  std::unique_ptr<Node> right{};
  int val{};
};

struct promise_t {
  // promise_t() { INFORM; }
  // ~promise_t() { INFORM; }
  auto get_return_object() {
    INFORM;
    return std::coroutine_handle<promise_t>::from_promise(*this);
  }
  constexpr std::suspend_never initial_suspend() const noexcept {
    INFORM;
    return {};
  }
  constexpr std::suspend_always final_suspend() const noexcept {
    INFORM;
    return {};
  }
  void return_void() const { INFORM; }
  std::suspend_always yield_value(int val) noexcept {
    value = val;
    INFORM;
    return {};
  }
  [[noreturn]] void unhandled_exception() { std::terminate(); }
  int value{};
};

struct ReturnObject {
  using promise_type = promise_t;
  ~ReturnObject() {
    handle_.destroy();
    INFORM;
  }
  ReturnObject(std::coroutine_handle<promise_type> handle) : handle_(handle) {
    INFORM;
  }
  auto value() {
    INFORM;
    return handle_.promise().value;
  }
  bool done() {
    INFORM;
    return handle_.done();
  }
  void resume() {
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

ReturnObject InOrder(const Node *node) {
  if (node == nullptr) co_return;
  auto l = InOrder(node->left.get());
  while (!l.done()) {
    co_yield l.value();
    l.resume();
  }
  co_yield node->val;
  auto r = InOrder(node->right.get());
  while (!r.done()) {
    co_yield r.value();
    r.resume();
  }
}

// int main()
// {
//   Node root(2);
//   Node *p = &root;
//   for (int i = 0; i < 1000; ++i)
//   {
//     p->left = std::make_unique<Node>(-i);
//     p->right = std::make_unique<Node>(-2 * i);
//     p = p->left.get();
//   }
//   Node root2(2);
//   Node *p2 = &root2;
//   for (int i = 0; i < 100000; ++i)
//   {
//     p2->left = std::make_unique<Node>(-i);
//     p2->right = std::make_unique<Node>(-2 * i);
//     p2 = p2->left.get();
//   }
//   // root.left = std::make_unique<Node>(-1);
//   // root.right = std::make_unique<Node>(3);

//   auto obj = InOrder(&root);

//   while (!obj.done())
//   {
//     //std::cout << obj.value() << ' ';
//     obj.value();
//     obj.resume();
//   }
// }

auto GenerateTree2() {
  Node root(2);
  Node *p = &root;
  for (int i = 0; i < 100; ++i) {
    p->left = std::make_unique<Node>(-i);
    p->right = std::make_unique<Node>(-2 * i);
    p = p->left.get();
  }
  return root;
}

auto GenerateTree() {
  Node root(2);
  Node *p = &root;
  auto recurse = [](Node *n, int level, auto &&self) {
    if (n == nullptr) return;
    if (level <= 0) return;
    --level;
    n->left = std::make_unique<Node>(-level);
    n->right = std::make_unique<Node>(-level);
    self(n->left.get(), level, self);
    self(n->right.get(), level, self);
  };
  recurse(p, 5, recurse);
  return root;
}

int CoroTraverse(const Node &root) {
  int maxVal = std::numeric_limits<int>::min();
  auto obj = InOrder(&root);
  while (!obj.done()) {
    maxVal = std::max(maxVal, obj.value());
    obj.resume();
  }
  return maxVal;
}

static void BM_CoroTraversal(benchmark::State &state) {
  auto root = GenerateTree();
  for (auto _ : state) {
    CHECK_EQ(2, CoroTraverse(root));
  }
}
BENCHMARK(BM_CoroTraversal);

void _Traverse(const Node *n, int *maxVal) {
  if (n == nullptr) {
    return;
  }
  _Traverse(n->left.get(), maxVal);
  *maxVal = std::max(*maxVal, n->val);
  _Traverse(n->right.get(), maxVal);
}

int InorderTraversal(const Node &root) {
  int maxVal = std::numeric_limits<int>::min();
  _Traverse(&root, &maxVal);
  return maxVal;
}

static void BM_InorderTraversal(benchmark::State &state) {
  auto root = GenerateTree();
  // Perform setup here
  for (auto _ : state) {
    CHECK_EQ(2, InorderTraversal(root));
  }
}
BENCHMARK(BM_InorderTraversal);

// Lists the immediate contents of a directory.
cppcoro::generator<int> cppcorotraverse(const Node *node) {
  if (node == nullptr) co_return;
  for (auto c : cppcorotraverse(node->left.get())) {
    co_yield c;
  }
  co_yield int{node->val};
  for (auto c : cppcorotraverse(node->right.get())) {
    co_yield c;
  }
}

int CppcoroInorder(const Node &root) {
  int maxVal = std::numeric_limits<int>::min();
  for (auto x : cppcorotraverse(&root)) {
    maxVal = std::max(x, maxVal);
  }
  return maxVal;
}

static void BM_CppcoroGenerator(benchmark::State &state) {
  auto root = GenerateTree();

  // Perform setup here
  for (auto _ : state) {
    CHECK_EQ(2, CppcoroInorder(root));
  }
}
BENCHMARK(BM_CppcoroGenerator);

// Lists the immediate contents of a directory.
cppcoro::recursive_generator<int> CppcoroRecursiveInorder(const Node *node) {
  if (node == nullptr) co_return;
  co_yield CppcoroRecursiveInorder(node->left.get());
  co_yield int{node->val};
  co_yield CppcoroRecursiveInorder(node->right.get());
}

int CppcoroRecursiveGeneratorInorder(const Node &root) {
  int maxVal = std::numeric_limits<int>::min();
  for (auto x : CppcoroRecursiveInorder(&root)) {
    maxVal = std::max(x, maxVal);
  }
  return maxVal;
}

static void BM_CppcoroRecursiveGenerator(benchmark::State &state) {
  auto root = GenerateTree();

  // Perform setup here
  for (auto _ : state) {
    CHECK_EQ(2, CppcoroRecursiveGeneratorInorder(root));
  }
}
BENCHMARK(BM_CppcoroRecursiveGenerator);

///////////// boost
using Sink = typename boost::coroutines2::coroutine<int>::push_type;
using Source = typename boost::coroutines2::coroutine<int>::pull_type;
void InorderTraverseToSink(const Node *node, Sink &sink) {
  if (node == nullptr) return;
  InorderTraverseToSink(node->left.get(), sink);
  sink(node->val);
  InorderTraverseToSink(node->right.get(), sink);
}
Source BoostCoroInorderTraverse(const Node *root) {
  return Source([root](Sink &sink) { InorderTraverseToSink(root, sink); });
}

int BoostCoroFindMax(const Node &root) {
  int maxVal = std::numeric_limits<int>::min();
  for (auto x : BoostCoroInorderTraverse(&root)) {
    maxVal = std::max(x, maxVal);
  }
  return maxVal;
}

static void BM_BoostCoro2Generator(benchmark::State &state) {
  auto root = GenerateTree();

  // Perform setup here
  for (auto _ : state) {
    CHECK_EQ(2, BoostCoroFindMax(root));
  }
}
BENCHMARK(BM_BoostCoro2Generator);

// Run the benchmark
BENCHMARK_MAIN();