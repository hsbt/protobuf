// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "google/protobuf/offset_ptr.h"

#include <type_traits>

#include <gtest/gtest.h>
#include "google/protobuf/unittest.pb.h"

namespace google {
namespace protobuf {
namespace internal {
namespace {

int array[10];

TEST(BasePtrTest, Basic) {
  BasePointer<int, false> b(&array[5], &array);
  EXPECT_EQ(&array[5], b.Resolve(&array));
}

TEST(BasePtrTest, Copyable) {
  EXPECT_TRUE(
      (std::is_trivially_copy_constructible_v<BasePointer<int, false>>));
  EXPECT_TRUE((std::is_trivially_copy_assignable_v<BasePointer<int, false>>));
  EXPECT_TRUE((std::is_trivially_destructible_v<BasePointer<int, false>>));

  BasePointer<int, false> b(&array[7], &array);
  EXPECT_EQ(&array[7], b.Resolve(&array));

  auto b2 = b;
  EXPECT_EQ(&array[7], b2.Resolve(&array));
}

TEST(BasePtrTest, BaseCanBeOnEitherSide) {
  EXPECT_EQ(&array[7],
            (BasePointer<int, false>(&array[7], &array[3])).Resolve(&array[3]));
  EXPECT_EQ(&array[4],
            (BasePointer<int, false>(&array[4], &array[7])).Resolve(&array[7]));
}

TEST(BasePtrTest, BaseCanBeSameAsPointer) {
  int var = 0;

  EXPECT_EQ(&var, (BasePointer<int, false>(&var, &var)).Resolve(&var));
}

TEST(BasePtrTest, NullIsAllowed) {
  bool dummy = false;
  BasePointer<int, true> b(nullptr, &dummy);
  EXPECT_EQ(nullptr, b.Resolve(&dummy));

  EXPECT_DEATH((BasePointer<int, false>(nullptr, &dummy)),
               "Pointer out of scope");
}

TEST(OffsetPtrTest, Basic) {
  struct Object {
    int array[10];
    OffsetPtr<int, false> ptr;
  } object{};

  object.ptr = object.array + 3;
  EXPECT_EQ(object.ptr, object.array + 3);
}

TEST(OffsetProtoPtr, Basic) {
  using P = proto2_unittest::TestAllTypes;
  OffsetProtoPtr<const P> p;

  p = nullptr;
  EXPECT_EQ(&P::default_instance(), p);

  P msg;
  p = &msg;
  EXPECT_EQ(&msg, p);

  p = &P::default_instance();
  EXPECT_EQ(&P::default_instance(), p);
}

}  // namespace
}  // namespace internal
}  // namespace protobuf
}  // namespace google
