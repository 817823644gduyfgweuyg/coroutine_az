
#include <chrono>

#include <gtest/gtest.h>

#include "src/snake/mcts/mcts.h"

namespace mcst {

struct NimGame {
  auto is_terminal() const { return nim_state == 1; }

  static consteval int valid_actions_count() { return 2; }
  static consteval std::array<bool, 2> valid_actions() {
    return {true, false};
  }
  static constexpr auto action_space_size() {
    return 2;
  }

  void execute(bool b) {
    if (b) {
      --nim_state;
    }
    --nim_state;
  }

  int nim_state{};
};



TEST(Mcts, SinglePredictionTest) {
  NimGame ng{10};
  mcts::Node<NimGame> node(ng);

  while (node.try_expand() != nullptr) ;
}

}  // namespace mcst