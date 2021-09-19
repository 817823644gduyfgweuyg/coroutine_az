#include <grpcpp/grpcpp.h>

#include "snake/snake.grpc.pb.h"
#include "snake/snake.pb.h"
#include "tensorflow/core/framework/tensor.h"

using grpc::ServerContext;
using grpc::Status;

namespace snake {
class ReplayBufferImpl final : public ReplayBuffer::Service {
 public:
  ReplayBufferImpl();
  Status StoreBuffer(ServerContext* context, const StoreBufferRequest* request,
                     StoreBufferResponse* reply) override;

 private:
  const int size_;
  tensorflow::Tensor boards_;
  tensorflow::Tensor policies_;
  tensorflow::Tensor values_;
};
}  // namespace snake
