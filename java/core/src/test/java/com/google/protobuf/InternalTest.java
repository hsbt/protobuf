// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package com.google.protobuf;

import static com.google.common.truth.Truth.assertThat;

import com.google.protobuf.Internal.BitmaskEnumVerifier;
import com.google.protobuf.Internal.SequentialEnumVerifier;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

/** Tests for {@link Internal} helper classes. */
@RunWith(JUnit4.class)
public class InternalTest {

  @Test
  public void testSequentialEnumVerifier() {
    SequentialEnumVerifier verifier = new SequentialEnumVerifier(2, 5);
    assertThat(verifier.isInRange(1)).isFalse();
    assertThat(verifier.isInRange(2)).isTrue();
    assertThat(verifier.isInRange(3)).isTrue();
    assertThat(verifier.isInRange(4)).isTrue();
    assertThat(verifier.isInRange(5)).isTrue();
    assertThat(verifier.isInRange(6)).isFalse();
  }

  @Test
  public void testSequentialEnumVerifier_singleValue() {
    SequentialEnumVerifier verifier = new SequentialEnumVerifier(3, 3);
    assertThat(verifier.isInRange(2)).isFalse();
    assertThat(verifier.isInRange(3)).isTrue();
    assertThat(verifier.isInRange(4)).isFalse();
  }

  @Test
  public void testBitmaskEnumVerifier() {
    // Mask for values: 2, 3, 5, 8
    // min = 2
    // offsets:
    // 2 -> 0 (1L << 0 = 1)
    // 3 -> 1 (1L << 1 = 2)
    // 5 -> 3 (1L << 3 = 8)
    // 8 -> 6 (1L << 6 = 64)
    // mask = 1 + 2 + 8 + 64 = 75
    BitmaskEnumVerifier verifier = new BitmaskEnumVerifier(2, 75L);

    assertThat(verifier.isInRange(1)).isFalse(); // offset -1
    assertThat(verifier.isInRange(2)).isTrue(); // offset 0
    assertThat(verifier.isInRange(3)).isTrue(); // offset 1
    assertThat(verifier.isInRange(4)).isFalse(); // offset 2
    assertThat(verifier.isInRange(5)).isTrue(); // offset 3
    assertThat(verifier.isInRange(6)).isFalse(); // offset 4
    assertThat(verifier.isInRange(7)).isFalse(); // offset 5
    assertThat(verifier.isInRange(8)).isTrue(); // offset 6
    assertThat(verifier.isInRange(9)).isFalse(); // offset 7

    // Test boundaries of bitmask (up to 64 values)
    assertThat(verifier.isInRange(2 + 63)).isFalse(); // offset 63 (not in mask)
    assertThat(verifier.isInRange(2 + 64)).isFalse(); // offset 64 (out of range)
  }

  @Test
  public void testBitmaskEnumVerifier_boundaryOffset63() {
    // min = 0, mask has only bit 63 set
    BitmaskEnumVerifier verifier = new BitmaskEnumVerifier(0, 1L << 63);
    assertThat(verifier.isInRange(0)).isFalse();
    assertThat(verifier.isInRange(62)).isFalse();
    assertThat(verifier.isInRange(63)).isTrue();
    assertThat(verifier.isInRange(64)).isFalse();
  }
}
