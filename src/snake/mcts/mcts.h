#pragma once

#include <array>
#include <bitset>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include <concepts>

//  #include "src/tag_invoke/tag_invoke.h"

#include "tensorflow/core/platform/default/logging.h"

namespace mcts {

// inline struct valid_actions_t {
//   // An optional default implementation
//   template <typename T>
//   friend bool tag_invoke(valid_actions_t, const T& x) noexcept {
//     return false;
//   }

//   template <typename... T>
//   auto operator()(T&&... x) const
//       noexcept(unifex::is_nothrow_tag_invocable_v<valid_actions_t, T&&...>)
//           -> unifex::tag_invoke_result_t<valid_actions_t, T&&...> {
//     // Dispatch to the call to tag_invoke() passing the CPO as the
//     // first parameter.
//     return tag_invoke(valid_actions_t{}, std::forward<T>(x)...);
//   }
// } valid_actions;

/*
double random_rollout(const SnakeMctsAdapter& state) {
  auto s = state;
  std::random_device rd;
  std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> distrib(Direction_MIN, Direction_MAX);

  while (!s.is_terminal()) {
    Direction candidate_d = static_cast<Direction>(distrib(gen));
    if (s.valid_action(candidate_d)) {
      s.execute(candidate_d);
    }
  }
  return s.value();
}
*/

/*

template <typename STATE>
*/

template <typename STATE, typename ACTION>
concept IsState = requires(STATE t, ACTION a) {
  { std::as_const(t).is_terminal() } -> std::same_as<bool>;
  {
    std::as_const(t).valid_actions_count()
    } -> std::convertible_to<std::size_t>;
  { a } -> std::convertible_to<std::size_t>;
  { *std::begin(t.valid_actions()) } -> std::convertible_to<ACTION>;
  { *std::end(t.valid_actions()) } -> std::convertible_to<ACTION>;
  { STATE::action_space_size() } -> std::convertible_to<std::size_t>;
  t.execute(a);
};

template <typename STATE, typename ACTION = std::decay_t<decltype(*std::begin(
                              std::declval<STATE>().valid_actions()))>>
requires IsState<STATE, ACTION>
class Node {
 public:
  explicit Node(
      STATE state = STATE{} /* current state */,
      ACTION action = ACTION{} /* last action that led to this state */,
      Node* parent = nullptr)
      : state_(state),
        action_(action),
        parent_(parent),
        is_terminal_(state_.is_terminal())
  {
    CHECK_GT(state_.valid_actions_count(), 0);
  }

  ~Node() {
    for (Node* n : children_) {
      if (n) {
        delete n;
      }
    }
  }

//   bool is_fully_expanded() const {
//     return num_children_expanded == state_.valid_actions_count();
//   }

//   double ucb(double exploration_value) const {
//     return (total_reward_ / num_visits_) * state_.player() +
//            exploration_value *
//                sqrt(2.0 * log(parent_->num_visits_) / num_visits_);
//   }

  Node* try_expand() {
    for (auto&& action : state_.valid_actions()) {
      if (children_[action]) {
        continue;
      }
      auto clone_state = state_;
      clone_state.execute(action);
      children_[action] = new Node(clone_state, action, this);
      ++num_children_expanded;
      return children_[action];
    }
    return nullptr;
  }

  /*  std::string graphviz_dot() const {
      struct GraphBuilder {
        std::vector<std::string> lines;
        void operator()(const Node* node, std::string name) {
          if (node == nullptr) {
            return;
          }
          auto info = absl::StrCat("reward=", node->total_reward_, " ",
                                   "num_visits=", node->num_visits_);

          lines.push_back(absl::StrCat("  ", name,
                                       "[shape=none margin=0 label=< ",
                                       node->state_.html(info), " >];"));
          for (int i = Direction_MIN; i < Direction_ARRAYSIZE; ++i) {
            const Node* child = node->children_[i];
            if (child == nullptr) {
              continue;
            }
            auto child_name = name + Direction_Name(static_cast<Direction>(i));
            lines.push_back(absl::StrCat(
                "  ", name, "->", child_name, " [label=\"",
                Direction_Name(static_cast<Direction>(i)), " ucb=",
    child->ucb(0.0), "\"];")); this->operator()(child, child_name);
          }
        }
      };

      GraphBuilder builder;
      builder.lines.push_back("digraph G {");
      builder(this, "root");
      builder.lines.push_back("}");
      return absl::StrJoin(builder.lines, "\n");
    }*/

  STATE state_;
  ACTION action_;
  Node* parent_;
  bool is_terminal_;

  int num_visits_ = 0;
  double total_reward_ = 0;
  size_t num_children_expanded = 0;
  std::array<Node*, STATE::action_space_size()> children_ = {};
  double prior_ = 0.0;
};


template <typename STATE, typename ACTION>
class Mcts {
 public:
  using node_t = Node<STATE, ACTION>;

  // Mcts(std::function<double(const SnakeMctsAdapter&)> rollout)
  //     : rollout_(rollout) {}
  ACTION Search(STATE state) {
    root_ = std::make_unique<node_t>(state);

    for (int i = 0; i < 1000; ++i) {
      execute_round();
    }

    auto* best_child = get_best_child(root_.get(), 0.0);
    CHECK(best_child != nullptr);
    return best_child->action_;
  }

  void backpropogate(node_t* node, double reward) {
    while (node != nullptr) {
      node->num_visits_++;
      node->total_reward_ += reward;
      node = node->parent_;
    }
  }

  void execute_round() {
    node_t* node = select_node(root_.get());
    double reward = rollout_(node->state_);
    backpropogate(node, reward);
  }

  /*Node* select_node(Node* node) {
    while (!node->is_terminal_) {
      if (node->is_fully_expanded()) {
        node = get_best_child(node, exploration_constant_);
      } else {
        return node->expand();
      }
    }
    return node;
  }*/
  
  node_t* select_node(node_t* node) {
    while (!node->is_terminal_) {
      auto* child = node->try_expand();
      if (child == nullptr) {
        node = get_best_child(node, exploration_constant_);
      } else {
        return child;
      }
    }
    return node;
  }

  node_t* get_best_child(node_t* node, double exploration_value) const {
    return *std::max_element(node->children_.begin(), node->children_.end(),
                             [exploration_value](node_t* a, node_t* b) {
                               if (a == nullptr && b == nullptr) {
                                 return false;
                               }
                               if (a == nullptr || b == nullptr) {
                                 return a == nullptr;
                               }
                               return a->ucb(exploration_value) <
                                      b->ucb(exploration_value);
                             });
  }

 private:
  float exploration_constant_ = 2.0;
  //std::function<double(const SnakeMctsAdapter&)> rollout_;
  std::unique_ptr<node_t> root_;
};

}  // namespace mcts