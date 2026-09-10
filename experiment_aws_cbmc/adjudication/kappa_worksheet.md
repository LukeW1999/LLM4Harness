# Blind rating sheet: is the expert's property expressed at all?

For each item below you see the expert harness's assertions and the LLM
harness's assertions for the same function. You do not see which layer the
tooling assigned, and the items are shuffled.

One question per item:

> Does the LLM harness assert the property the expert's assertions check,
> in any form at all, however it is written or named?

Answer `Y`, `N`, or `?` on the ANSWER line. `Y` means the property is there
in some syntactic form, even paraphrased, split across assertions, or under
different variable names. `N` means it is absent. `?` means you cannot tell
from the assertions alone.

Do not open the repository while rating. Do not look up the function.

---

## Item 01  (`aws_byte_buf_reset`)

Expert harness asserts:
```c
assert(buf.len == 0);
assert(buf.allocator == old.allocator);
assert(buf.buffer == old.buffer);
assert(buf.capacity == old.capacity);
```

LLM harness asserts:
```c
assert(buf.len == 0);
assert(buf.capacity == buf_old.capacity);
assert(buf.buffer == buf_old.buffer);
assert(buf.allocator == buf_old.allocator);
assert(aws_byte_buf_is_valid(&buf));
```

ANSWER: 

---

## Item 02  (`aws_byte_buf_cat`)

Expert harness asserts:
```c
assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));
assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));
assert(aws_byte_buf_is_valid(&buffer1));
assert(aws_byte_buf_is_valid(&buffer2));
assert(aws_byte_buf_is_valid(&buffer3));
assert(aws_byte_buf_is_valid(&dest));
```

LLM harness asserts:
```c
assert(aws_byte_buf_is_valid(&dest));
assert(aws_byte_buf_is_valid(&src0));
assert(aws_byte_buf_is_valid(&src1));
assert(dest.capacity == old_dest.capacity);
assert(dest.allocator == old_dest.allocator);
assert(dest.buffer == old_dest.buffer);
assert(dest.len == old_dest.len + src0.len);
assert(dest.len == old_dest.len + src0.len + src1.len);
assert(dest.len <= dest.capacity);
assert(dest.len >= old_dest.len);
assert(dest.len <= dest.capacity);
```

ANSWER: 

---

## Item 03  (`aws_ptr_eq`)

Expert harness asserts:
```c
assert(rval == (p1 == p2));
```

LLM harness asserts:
```c
assert(result == (a == b));
```

ANSWER: 

---

## Item 04  (`aws_byte_buf_from_array`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == length);
assert(buf.capacity == length);
assert(buf.allocator == NULL);
```

LLM harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == len);
assert(buf.capacity == len);
assert(buf.allocator == NULL);
assert(buf.buffer == bytes);
assert(buf.buffer == NULL);
assert(memcmp(bytes, bytes_copy, len) == 0);
```

ANSWER: 

---

## Item 05  (`aws_nospec_mask`)

Expert harness asserts:
```c
assert((index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2)));
assert(rval == UINTPTR_MAX);
assert(!((index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2))));
```

LLM harness asserts:
```c
assert(result == UINTPTR_MAX);
assert(result == 0);
assert(result == 0 || result == UINTPTR_MAX);
```

ANSWER: 

---

## Item 06  (`aws_string_destroy`)

Expert harness asserts:
```c
(none extracted)
```

LLM harness asserts:
```c
assert(str == NULL);
assert(str == old_str);
```

ANSWER: 

---

## Item 07  (`aws_byte_buf_reset`)

Expert harness asserts:
```c
assert(buf.len == 0);
assert(buf.allocator == old.allocator);
assert(buf.buffer == old.buffer);
assert(buf.capacity == old.capacity);
```

LLM harness asserts:
```c
assert(buf.len == 0);
assert(buf.capacity == old_capacity);
assert(buf.allocator == old_allocator);
assert(buf.buffer == old_buffer);
assert(aws_byte_buf_is_valid(&buf));
```

ANSWER: 

---

## Item 08  (`aws_byte_buf_eq_c_str`)

Expert harness asserts:
```c
assert(buf.len == str_len);
assert(aws_byte_buf_is_valid(&buf));
```

LLM harness asserts:
```c
assert(buf.buffer == old.buffer);
assert(buf.len == old.len);
assert(buf.capacity == old.capacity);
assert(buf.allocator == old.allocator);
assert(aws_byte_buf_is_valid(&buf));
```

ANSWER: 

---

## Item 09  (`aws_byte_buf_from_array`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == length);
assert(buf.capacity == length);
assert(buf.allocator == NULL);
```

LLM harness asserts:
```c
assert(buf.len == len);
assert(buf.capacity == len);
assert(buf.allocator == NULL);
assert(buf.buffer == array);
assert(buf.len == 0);
assert(buf.capacity == 0);
assert(aws_byte_buf_is_valid(&buf));
```

ANSWER: 

---

## Item 10  (`aws_byte_buf_advance`)

Expert harness asserts:
```c
assert(buf.len == old.len + len);
assert(buf.capacity == old.capacity);
assert(buf.allocator == old.allocator);
assert(output.len == 0);
assert(output.capacity == len);
assert(output.allocator == NULL);
assert(output.len == 0);
assert(output.capacity == 0);
assert(output.allocator == NULL);
assert(output.buffer == NULL);
assert(aws_byte_buf_is_valid(&buf));
assert(aws_byte_buf_is_valid(&output));
```

LLM harness asserts:
```c
__CPROVER_assert(result, "aws_byte_buf_advance must succeed");
assert(buffer.buffer == old_buffer.buffer + len);
assert(buffer.len == old_buffer.len - len);
assert(buffer.capacity == old_buffer.capacity);
assert(buffer.allocator == old_buffer.allocator);
assert(output.buffer == NULL);
assert(output.buffer == old_buffer.buffer);
assert(output.capacity == len);
assert(output.len == len);
assert(output.allocator == NULL);
assert(aws_byte_buf_is_valid(&buffer));
assert(aws_byte_buf_is_valid(&output));
```

ANSWER: 

---

## Item 11  (`aws_byte_buf_cat`)

Expert harness asserts:
```c
assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));
assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));
assert(aws_byte_buf_is_valid(&buffer1));
assert(aws_byte_buf_is_valid(&buffer2));
assert(aws_byte_buf_is_valid(&buffer3));
assert(aws_byte_buf_is_valid(&dest));
```

LLM harness asserts:
```c
assert(aws_byte_buf_is_valid(&dest));
assert(aws_byte_buf_is_valid(&src1));
assert(aws_byte_buf_is_valid(&src2));
assert(dest.capacity == old_capacity);
assert(dest.allocator == old_allocator);
assert(dest.buffer == old_buffer);
assert(src1.len == src1_old_len);
assert(src2.len == src2_old_len);
assert(dest.len == old_len + src1_old_len + src2_old_len);
assert(dest.len <= dest.capacity);
assert(result == AWS_OP_ERR);
assert(dest.len >= old_len);
assert(dest.len <= dest.capacity);
```

ANSWER: 

---

## Item 12  (`aws_string_eq_byte_buf`)

Expert harness asserts:
```c
assert(str->len == buf.len);
assert(aws_string_is_valid(str));
assert(aws_byte_buf_is_valid(&buf));
```

LLM harness asserts:
```c
assert(result == true);
assert(result == false);
assert(result == false);
assert(str == old_str);
assert(str->len == old_str->len);
assert(str->allocator == old_str->allocator);
assert(buf->len == old_buf_val.len);
assert(buf->capacity == old_buf_val.capacity);
assert(buf->buffer == old_buf_val.buffer);
assert(buf->allocator == old_buf_val.allocator);
assert(aws_string_is_valid(str));
assert(aws_byte_buf_is_valid(buf));
assert(str == old_str);
assert(buf->len == old_buf_val.len);
assert(buf->capacity == old_buf_val.capacity);
assert(buf->buffer == old_buf_val.buffer);
assert(buf->allocator == old_buf_val.allocator);
assert(aws_byte_buf_is_valid(buf));
assert(str->len == old_str->len);
assert(str->allocator == old_str->allocator);
assert(aws_string_is_valid(str));
```

ANSWER: 

---

## Item 13  (`aws_byte_buf_init_copy`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(dest));
assert(aws_byte_buf_has_allocator(dest));
assert(dest->len == src.len);
assert(dest->capacity == src.capacity);
assert(aws_byte_buf_is_valid(&src));
```

LLM harness asserts:
```c
assert(dest.allocator == allocator);
assert(dest.len == 0);
assert(dest.capacity == 0);
assert(dest.buffer == NULL);
assert(dest.len == src.len);
assert(dest.capacity == src.capacity);
assert(dest.buffer != NULL);
assert(dest.buffer != src.buffer);
assert(dest.len == 0);
assert(dest.capacity == 0);
assert(dest.buffer == NULL);
assert(dest.allocator == NULL);
assert(src.len == old_src.len);
assert(src.capacity == old_src.capacity);
assert(src.buffer == old_src.buffer);
assert(src.allocator == old_src.allocator);
assert(aws_byte_buf_is_valid(&dest));
assert(aws_byte_buf_is_valid(&src));
```

ANSWER: 

---

## Item 14  (`aws_byte_buf_from_empty_array`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == 0);
assert(buf.capacity == capacity);
assert(buf.allocator == NULL);
```

LLM harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == 0);
assert(buf.capacity == capacity);
assert(buf.buffer == bytes);
assert(buf.buffer == NULL);
assert(raw[0] == 0xAA);
assert(raw[capacity + 1] == 0xBB);
assert(raw[i + 1] == raw[i + 1]);
```

ANSWER: 

---

## Item 15  (`aws_byte_buf_eq_c_str`)

Expert harness asserts:
```c
assert(buf.len == str_len);
assert(aws_byte_buf_is_valid(&buf));
```

LLM harness asserts:
```c
assert(buf.len == old_buf.len);
assert(buf.capacity == old_buf.capacity);
assert(buf.buffer == old_buf.buffer);
assert(buf.allocator == old_buf.allocator);
assert(aws_byte_buf_is_valid(&buf));
```

ANSWER: 

---

## Item 16  (`aws_byte_buf_from_empty_array`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == 0);
assert(buf.capacity == capacity);
assert(buf.allocator == NULL);
```

LLM harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == 0);
assert(buf.capacity == capacity);
assert(buf.buffer == (uint8_t *)array);
assert(buf.allocator == NULL);
```

ANSWER: 

---

## Item 17  (`aws_ptr_eq`)

Expert harness asserts:
```c
assert(rval == (p1 == p2));
```

LLM harness asserts:
```c
assert(result == (old_a == old_b));
assert(a == old_a);
assert(b == old_b);
assert(buf1.len == old_buf1.len);
assert(buf1.capacity == old_buf1.capacity);
assert(buf1.buffer == old_buf1.buffer);
assert(buf1.allocator == old_buf1.allocator);
assert(buf2.len == old_buf2.len);
assert(buf2.capacity == old_buf2.capacity);
assert(buf2.buffer == old_buf2.buffer);
assert(buf2.allocator == old_buf2.allocator);
```

ANSWER: 

---

## Item 18  (`aws_ptr_eq`)

Expert harness asserts:
```c
assert(rval == (p1 == p2));
```

LLM harness asserts:
```c
assert(result == (a == b));
assert(buf_a[0] == a_byte_snapshot);
assert(buf_b[0] == b_byte_snapshot);
```

ANSWER: 

---

## Item 19  (`aws_byte_buf_from_empty_array`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == 0);
assert(buf.capacity == capacity);
assert(buf.allocator == NULL);
```

LLM harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == 0);
assert(buf.capacity == len);
assert(buf.buffer == (uint8_t *)array);
assert(buf.allocator == NULL);
```

ANSWER: 

---

## Item 20  (`aws_byte_buf_cat`)

Expert harness asserts:
```c
assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));
assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));
assert(aws_byte_buf_is_valid(&buffer1));
assert(aws_byte_buf_is_valid(&buffer2));
assert(aws_byte_buf_is_valid(&buffer3));
assert(aws_byte_buf_is_valid(&dest));
```

LLM harness asserts:
```c
assert(aws_byte_buf_is_valid(&dest));
assert(aws_byte_buf_is_valid(&buf0));
assert(aws_byte_buf_is_valid(&buf1));
assert(dest.allocator == old_dest.allocator);
assert(dest.capacity == old_dest.capacity);
assert(dest.buffer == old_dest.buffer);
assert(buf0.len == old_buf0.len);
assert(buf0.capacity == old_buf0.capacity);
assert(buf0.buffer == old_buf0.buffer);
assert(buf0.allocator == old_buf0.allocator);
assert(buf1.len == old_buf1.len);
assert(buf1.capacity == old_buf1.capacity);
assert(buf1.buffer == old_buf1.buffer);
assert(buf1.allocator == old_buf1.allocator);
assert(dest.len == old_dest.len + buf0.len);
assert(dest.len == old_dest.len + buf0.len + buf1.len);
assert(dest.len <= dest.capacity);
```

ANSWER: 

---

## Item 21  (`aws_byte_buf_from_empty_array`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == 0);
assert(buf.capacity == capacity);
assert(buf.allocator == NULL);
```

LLM harness asserts:
```c
assert(buf.len == 0);
assert(buf.capacity == len);
assert(buf.buffer == (uint8_t *)array);
assert(buf.allocator == NULL);
assert(aws_byte_buf_is_valid(&buf));
```

ANSWER: 

---

## Item 22  (`aws_byte_buf_secure_zero`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == 0);
```

LLM harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf)); /* still a valid buffer */
assert(buf.len == 0); /* length reset to zero */
assert(buf.buffer[i] == 0);
assert(guard_before[i] == guard_before_orig[i]);
assert(guard_after[i] == guard_after_orig[i]);
assert(buf.buffer == NULL);
```

ANSWER: 

---

## Item 23  (`aws_string_eq_byte_cursor`)

Expert harness asserts:
```c
assert(aws_string_is_valid(str));
assert(str->len == cursor.len);
assert(aws_byte_cursor_is_valid(&cursor));
```

LLM harness asserts:
```c
assert(result == true);
assert(result == false);
assert(result == false);
assert(result == true);
assert(str->len == old_str_len);
assert(aws_string_is_valid(str));
assert(cur->len == old_cur_len);
assert(cur->ptr == old_cur_ptr);
assert(aws_byte_cursor_is_valid(cur));
```

ANSWER: 

---

## Item 24  (`aws_byte_buf_secure_zero`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.len == 0);
```

LLM harness asserts:
```c
assert(buf.buffer == old_buffer);
assert(buf.capacity == old_capacity);
assert(buf.allocator == old_allocator);
assert(aws_byte_buf_is_valid(&buf));
```

ANSWER: 

---

## Item 25  (`aws_string_destroy`)

Expert harness asserts:
```c
(none extracted)
```

LLM harness asserts:
```c
assert(str == NULL);
assert(was_null ? (str == NULL) : true);
```

ANSWER: 

---

## Item 26  (`aws_byte_buf_reset`)

Expert harness asserts:
```c
assert(buf.len == 0);
assert(buf.allocator == old.allocator);
assert(buf.buffer == old.buffer);
assert(buf.capacity == old.capacity);
```

LLM harness asserts:
```c
assert(buf.len == 0);
assert(buf.capacity == old_buf.capacity);
assert(buf.allocator == old_buf.allocator);
assert(buf.buffer == old_buf.buffer);
assert(aws_byte_buf_is_valid(&buf));
```

ANSWER: 

---

## Item 27  (`aws_byte_buf_from_c_str`)

Expert harness asserts:
```c
assert(aws_byte_buf_is_valid(&buf));
assert(buf.allocator == NULL);
assert(buf.len == strlen(c_str));
assert(buf.capacity == buf.len);
assert(strlen(c_str) == 0);
assert(buf.len == 0);
assert(buf.capacity == 0);
```

LLM harness asserts:
```c
assert(result.allocator == NULL);
assert(result.len == 0);
assert(result.capacity == 0);
assert(result.buffer == NULL);
assert(expected_len <= MAX_BUFFER_SIZE);
assert(result.len == expected_len);
assert(result.capacity >= expected_len);
assert(result.buffer != NULL);
assert(!memcmp(result.buffer, c_str, expected_len));
assert(((char *)result.buffer)[expected_len] == '\0');
assert(aws_byte_buf_is_valid(&result));
```

ANSWER: 

---

## Item 28  (`aws_ring_buffer_buf_belongs_to_pool`)

Expert harness asserts:
```c
assert(is_member == result);
assert(aws_ring_buffer_is_valid(&ring_buf));
assert(aws_byte_buf_is_valid(&buf));
assert(ring_buf_old == ring_buf);
assert(buf_old == buf);
```

LLM harness asserts:
```c
assert(ring_buffer.allocation == old_allocation);
assert(ring_buffer.allocation_end == old_allocation_end);
assert(buf.buffer == old_buf_buffer);
```

ANSWER: 

---

## Item 29  (`aws_byte_buf_clean_up`)

Expert harness asserts:
```c
assert(buf.allocator == NULL);
assert(buf.buffer == NULL);
assert(buf.len == 0);
assert(buf.capacity == 0);
```

LLM harness asserts:
```c
assert(buf.buffer == NULL);
assert(buf.len == 0);
assert(buf.capacity == 0);
assert(buf.allocator == NULL);
assert(old_buf.len == old_buf.len);
assert(old_buf.capacity == old_buf.capacity);
assert(old_buf.allocator == alloc);
assert(old_buf.buffer != NULL);
assert(old_buf.buffer == NULL);
assert(aws_byte_buf_is_valid(&buf));
```

ANSWER: 

---

## Item 30  (`aws_byte_buf_advance`)

Expert harness asserts:
```c
assert(buf.len == old.len + len);
assert(buf.capacity == old.capacity);
assert(buf.allocator == old.allocator);
assert(output.len == 0);
assert(output.capacity == len);
assert(output.allocator == NULL);
assert(output.len == 0);
assert(output.capacity == 0);
assert(output.allocator == NULL);
assert(output.buffer == NULL);
assert(aws_byte_buf_is_valid(&buf));
assert(aws_byte_buf_is_valid(&output));
```

LLM harness asserts:
```c
assert(ret == true);
assert(output.buffer == (old_buf_ptr == NULL ? NULL : old_buf_ptr + old_buf_len));
assert(output.capacity == len);
assert(output.len == 0);
assert(aws_byte_buf_is_valid(&output));
assert(buf.len == old_buf_len + len);
assert(buf.capacity == old_buf_capacity);
assert(buf.buffer == old_buf_ptr);
assert(aws_byte_buf_is_valid(&buf));
assert(memcmp(buf.buffer, buf_mem_copy, buf.capacity) == 0);
```

ANSWER: 

---

## Item 31  (`aws_byte_buf_cat`)

Expert harness asserts:
```c
assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));
assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));
assert(aws_byte_buf_is_valid(&buffer1));
assert(aws_byte_buf_is_valid(&buffer2));
assert(aws_byte_buf_is_valid(&buffer3));
assert(aws_byte_buf_is_valid(&dest));
```

LLM harness asserts:
```c
assert(dest.len == old_dest.len + src1.len + src2.len);
assert(dest.buffer == old_dest.buffer);
assert(dest.capacity == old_dest.capacity);
assert(dest.allocator == old_dest.allocator);
assert(src1.len == old_src1.len);
assert(src1.capacity == old_src1.capacity);
assert(src1.buffer == old_src1.buffer);
assert(src1.allocator == old_src1.allocator);
assert(src2.len == old_src2.len);
assert(src2.capacity == old_src2.capacity);
assert(src2.buffer == old_src2.buffer);
assert(src2.allocator == old_src2.allocator);
assert(dest.buffer == old_dest.buffer);
assert(dest.capacity == old_dest.capacity);
assert(dest.allocator == old_dest.allocator);
assert(src1.len == old_src1.len);
assert(src1.capacity == old_src1.capacity);
assert(src1.buffer == old_src1.buffer);
assert(src1.allocator == old_src1.allocator);
assert(src2.len == old_src2.len);
assert(src2.capacity == old_src2.capacity);
assert(src2.buffer == old_src2.buffer);
assert(src2.allocator == old_src2.allocator);
assert(aws_byte_buf_is_valid(&dest));
assert(aws_byte_buf_is_valid(&src1));
assert(aws_byte_buf_is_valid(&src2));
```

ANSWER: 

---

## Item 32  (`aws_byte_buf_clean_up`)

Expert harness asserts:
```c
assert(buf.allocator == NULL);
assert(buf.buffer == NULL);
assert(buf.len == 0);
assert(buf.capacity == 0);
```

LLM harness asserts:
```c
assert(buf.buffer == NULL);
assert(buf.len == 0);
assert(buf.capacity == 0);
assert(buf.allocator == NULL);
assert(aws_byte_buf_is_valid(&buf));
assert(buf.allocator != old_buf.allocator);
```

ANSWER: 

---
