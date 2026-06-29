// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "google/protobuf/offset_ptr.h"

#include <cstddef>

#include "absl/log/absl_log.h"

namespace google {
namespace protobuf {
namespace internal {

void BasePointerFailure(const void* ptr, const void* base) noexcept {
  ptrdiff_t diff =
      reinterpret_cast<const char*>(ptr) - reinterpret_cast<const char*>(base);
  if (diff == 0) {
    ABSL_LOG(FATAL) << "Nullable base pointer can't self reference.";
  }
  ABSL_LOG(FATAL) << "Pointer out of scope for offset pointer: ptr=" << ptr
                  << " base=" << base << " diff=" << std::hex << diff;
}

}  // namespace internal
}  // namespace protobuf
}  // namespace google
