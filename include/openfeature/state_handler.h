#ifndef CPP_SDK_INCLUDE_OPENFEATURE_STATE_HANDLER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_STATE_HANDLER_H_

#include "openfeature/evaluation_context.h"

namespace openfeature {

// A Provider can opt in for this behavior by implementing this interface and the FeatureProvider interface.
// The StateHandler interface provides lifecycle methods for initializing and shutting down provider state.
class StateHandler {
 public:
  virtual ~StateHandler() = default;
  virtual void Init(const EvaluationContext& ctx) = 0;
  virtual void Shutdown() = 0;
};
} // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_STATE_HANDLER_H_
