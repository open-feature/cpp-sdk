#include "openfeature/hook.h"

namespace openfeature {

// Explicit template instantiations for common hook types.
template class Hook<bool>;
template class Hook<std::string>;
template class Hook<int64_t>;
template class Hook<double>;
template class Hook<Value>;

}  // namespace openfeature
