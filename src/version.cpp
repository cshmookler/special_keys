// Local includes
#include "../build/version.hpp"

namespace keys {

const char* get_runtime_version() {
    return ::keys::compiletime_version;
}

} // namespace keys