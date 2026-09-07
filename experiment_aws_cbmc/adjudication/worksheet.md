# Mechanism adjudication worksheet (blind)

One card per silenced (condition, function) group. For each card, read the evidence
and write one label on the `rating:` line, then run `score_adjudication.py`.

- **NW** never-written: no GT assertion that would catch the fault is in the final
  harness, and the missing one was never written exactly in any iteration (or was
  removed after a CBMC FAIL, i.e. self-correction).
- **Del** deleted-to-pass: a GT assertion was written exactly, CBMC returned UNKNOWN
  at that iteration, and it is gone from the final harness.
- **Nar** narrowed-away: every GT assertion is present in the final harness, but the
  harness's assumptions are tighter than the expert's, so the faulty states are unreachable.
- **Unres** unresolved: the evidence does not support any of the three (for example no
  GT assertion could be parsed, so nothing can be said about presence).

Do not open `key.json` before finishing.

## 1. `Oracle_gptoss120b` / `aws_byte_buf_cat` (33 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer1));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer2));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer3));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&dest));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert(buffer1.buffer == old_buf1.buffer);`
- `assert(buffer1.capacity == old_buf1.capacity);`
- `assert(buffer1.len == old_buf1.len);`
- `assert(buffer2.buffer == old_buf2.buffer);`
- `assert(buffer2.capacity == old_buf2.capacity);`
- `assert(buffer2.len == old_buf2.len);`
- `assert(buffer3.buffer == old_buf3.buffer);`
- `assert(buffer3.capacity == old_buf3.capacity);`
- `assert(buffer3.len == old_buf3.len);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.capacity == old_dest.capacity);`
- `assert(dest.len <= dest.capacity);`
- `assert(dest.len == old_dest.len + total_appended);`
- `assert(dest.len == old_dest.len);`
- `assert(memcmp(buffer1.buffer, old_buf1_data, buffer1.capacity) == 0);`
- `assert(memcmp(buffer2.buffer, old_buf2_data, buffer2.capacity) == 0);`
- `assert(memcmp(buffer3.buffer, old_buf3_data, buffer3.capacity) == 0);`
- `assert(memcmp(dest.buffer + off, buffer1.buffer, buffer1.len) == 0);`
- `assert(memcmp(dest.buffer + off, buffer2.buffer, buffer2.len) == 0);`
- `assert(memcmp(dest.buffer + off, buffer3.buffer, buffer3.len) == 0);`
- `assert(memcmp(dest.buffer, old_dest_buf, old_dest.capacity) == 0);`
- `assert(memcmp(dest.buffer, old_dest_buf, old_dest.len) == 0);`
- `assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(dest.capacity <= MAX_BUFFER_SIZE);
- __CPROVER_assume(dest.len <= dest.capacity);
- __CPROVER_assume(srcs[i]->capacity <= MAX_BUFFER_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 2. `M_claude` / `aws_byte_buf_cat` (4 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer1));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer2));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer3));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&dest));`  
  → present in the final harness

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf0));`
- `assert(aws_byte_buf_is_valid(&buf1));`
- `assert(aws_byte_buf_is_valid(&dest));`
- `assert(buf0.allocator == old_buf0.allocator);`
- `assert(buf0.buffer == old_buf0.buffer);`
- `assert(buf0.capacity == old_buf0.capacity);`
- `assert(buf0.len == old_buf0.len);`
- `assert(buf1.allocator == old_buf1.allocator);`
- `assert(buf1.buffer == old_buf1.buffer);`
- `assert(buf1.capacity == old_buf1.capacity);`
- `assert(buf1.len == old_buf1.len);`
- `assert(dest.allocator == old_dest.allocator);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.capacity == old_dest.capacity);`
- `assert(dest.len <= dest.capacity);`
- `assert(dest.len == old_dest.len + buf0.len + buf1.len);`
- `assert(dest.len == old_dest.len + buf0.len);`

rating: 

---

## 3. `H_claude` / `aws_byte_buf_from_empty_array` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.capacity == capacity);`  
  → present in the final harness
- `assert(buf.allocator == NULL);`  
  → present in the final harness

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == NULL);`
- `assert(buf.buffer == (uint8_t *)array);`
- `assert(buf.capacity == capacity);`
- `assert(buf.len == 0);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 4. `A_claude` / `aws_byte_buf_secure_zero` (4 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(buf.len == 0);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (3), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == old_allocator);`
- `assert(buf.buffer == old_buffer);`
- `assert(buf.capacity == old_capacity);`

rating: 

---

## 5. `A_claude` / `aws_ring_buffer_buf_belongs_to_pool` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(is_member == result);`  
  → never written exactly, in any iteration
- `assert(aws_ring_buffer_is_valid(&ring_buf));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buf));`  
  → written exactly in iteration 1, absent from the final; CBMC at iteration 1 returned UNKNOWN
- `assert(ring_buf_old == ring_buf);`  
  → never written exactly, in any iteration
- `assert(buf_old == buf);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (4), assertions written:**

- `assert(buf.buffer == old_buf_buffer);`
- `assert(ring_buffer.allocation == old_allocation);`
- `assert(ring_buffer.allocation_end == old_allocation_end);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);
- __CPROVER_assume(head_offset <= ring_size);
- __CPROVER_assume(tail_offset <= ring_size);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 6. `A_gptoss120b` / `aws_nospec_mask` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2)));`  
  → never written exactly, in any iteration
- `assert(rval == UINTPTR_MAX);`  
  → never written exactly, in any iteration
- `assert(!((index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2))));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (3), assertions written:**

- `assert(mask == 0);`
- `assert(mask == SIZE_MAX);`

rating: 

---

## 7. `Oracle_gptoss120b` / `aws_ring_buffer_buf_belongs_to_pool` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(is_member == result);`  
  → never written exactly, in any iteration
- `assert(aws_ring_buffer_is_valid(&ring_buf));`  
  → present in the final harness
- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(ring_buf_old == ring_buf);`  
  → never written exactly, in any iteration
- `assert(buf_old == buf);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(aws_ring_buffer_is_valid(&ring_buf));`
- `assert(memcmp(&buf, &buf_old, sizeof(buf)) == 0);`
- `assert(memcmp(&ring_buf, &ring_buf_old, sizeof(ring_buf)) == 0);`
- `assert(result == expected);`

rating: 

---

## 8. `M_gptoss120b` / `aws_byte_buf_eq_c_str` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.len == str_len);`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == old.allocator);`
- `assert(buf.buffer == old.buffer);`
- `assert(buf.capacity == old.capacity);`
- `assert(buf.len == old.len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 9. `M_claude` / `aws_byte_buf_from_empty_array` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.capacity == capacity);`  
  → never written exactly, in any iteration
- `assert(buf.allocator == NULL);`  
  → present in the final harness

**LLM harness, final iteration (3), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == NULL);`
- `assert(buf.buffer == (uint8_t *)array);`
- `assert(buf.capacity == len);`
- `assert(buf.len == 0);`

**Assume-envelope comparison (LLM vs expert):**

- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 10. `M_claude` / `aws_byte_buf_reset` (3 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.allocator == old.allocator);`  
  → never written exactly, in any iteration
- `assert(buf.buffer == old.buffer);`  
  → never written exactly, in any iteration
- `assert(buf.capacity == old.capacity);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == old_allocator);`
- `assert(buf.buffer == old_buffer);`
- `assert(buf.capacity == old_capacity);`
- `assert(buf.len == 0);`

rating: 

---

## 11. `H_gptoss120b` / `aws_byte_buf_cat` (33 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer1));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer2));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer3));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&dest));`  
  → present in the final harness

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(&dest));`
- `assert(aws_byte_buf_is_valid(&src[i]));`
- `assert(dest.allocator == old_dest.allocator);`
- `assert(dest.allocator == old_dest.allocator);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.capacity == old_dest.capacity);`
- `assert(dest.capacity == old_dest.capacity);`
- `assert(dest.len == old_dest.len + total_len);`
- `assert(dest.len == old_dest.len);`
- `assert(src[i].allocator == old_src[i].allocator);`
- `assert(src[i].allocator == old_src[i].allocator);`
- `assert(src[i].buffer == old_src[i].buffer);`
- `assert(src[i].buffer == old_src[i].buffer);`
- `assert(src[i].capacity == old_src[i].capacity);`
- `assert(src[i].capacity == old_src[i].capacity);`
- `assert(src[i].len == old_src[i].len);`
- `assert(src[i].len == old_src[i].len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(number_of_args <= MAX_ARGS);

rating: 

---

## 12. `M_gptoss120b` / `aws_byte_buf_from_c_str` (3 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → never written exactly, in any iteration
- `assert(buf.allocator == NULL);`  
  → never written exactly, in any iteration
- `assert(buf.len == strlen(c_str));`  
  → never written exactly, in any iteration
- `assert(buf.capacity == buf.len);`  
  → never written exactly, in any iteration
- `assert(strlen(c_str) == 0);`  
  → never written exactly, in any iteration
- `assert(buf.len == 0);`  
  → never written exactly, in any iteration
- `assert(buf.capacity == 0);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (4), assertions written:**

- `assert(!memcmp(result.buffer, c_str, expected_len));`
- `assert(((char *)result.buffer)[expected_len] == '\0');`
- `assert(aws_byte_buf_is_valid(&result));`
- `assert(expected_len <= MAX_BUFFER_SIZE);`
- `assert(result.allocator == NULL);`
- `assert(result.buffer != NULL);`
- `assert(result.buffer == NULL);`
- `assert(result.capacity == 0);`
- `assert(result.capacity >= expected_len);`
- `assert(result.len == 0);`
- `assert(result.len == expected_len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(len <= MAX_BUFFER_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 13. `A_claude` / `aws_byte_buf_from_array` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(buf.len == length);`  
  → never written exactly, in any iteration
- `assert(buf.capacity == length);`  
  → never written exactly, in any iteration
- `assert(buf.allocator == NULL);`  
  → present in the final harness

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == NULL);`
- `assert(buf.buffer == array);`
- `assert(buf.capacity == 0);`
- `assert(buf.capacity == len);`
- `assert(buf.len == 0);`
- `assert(buf.len == len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(len <= MAX_BUFFER_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 14. `Oracle_gptoss120b` / `aws_byte_buf_clean_up_secure` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.allocator == NULL);`  
  → never written exactly, in any iteration
- `assert(buf.buffer == NULL);`  
  → present in the final harness
- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.capacity == 0);`  
  → present in the final harness

**LLM harness, final iteration (1), assertions written:**

- `assert(buf.allocator == orig_alloc);`
- `assert(buf.buffer == NULL);`
- `assert(buf.buffer == orig_buffer);`
- `assert(buf.capacity == 0);`
- `assert(buf.len == 0);`
- `assert(orig_buffer[i] == 0);`
- `assert(outside[i] == outside_before[i]);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(cap <= MAX_BUFFER_SIZE);
- __CPROVER_assume(buf.len <= buf.capacity);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 15. `M_gptoss120b` / `aws_byte_buf_advance` (15 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.len == old.len + len);`  
  → never written exactly, in any iteration
- `assert(buf.capacity == old.capacity);`  
  → never written exactly, in any iteration
- `assert(buf.allocator == old.allocator);`  
  → never written exactly, in any iteration
- `assert(output.len == 0);`  
  → written exactly in iteration 2, absent from the final; CBMC at iteration 2 returned FAIL
- `assert(output.capacity == len);`  
  → present in the final harness
- `assert(output.allocator == NULL);`  
  → present in the final harness
- `assert(output.len == 0);`  
  → written exactly in iteration 2, absent from the final; CBMC at iteration 2 returned FAIL
- `assert(output.capacity == 0);`  
  → written exactly in iteration 1, absent from the final; CBMC at iteration 1 returned FAIL
- `assert(output.allocator == NULL);`  
  → present in the final harness
- `assert(output.buffer == NULL);`  
  → present in the final harness
- `assert(aws_byte_buf_is_valid(&buf));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&output));`  
  → present in the final harness

**LLM harness, final iteration (7), assertions written:**

- `__CPROVER_assert(result, "aws_byte_buf_advance must succeed");`
- `assert(aws_byte_buf_is_valid(&buffer));`
- `assert(aws_byte_buf_is_valid(&output));`
- `assert(buffer.allocator == old_buffer.allocator);`
- `assert(buffer.buffer == old_buffer.buffer + len);`
- `assert(buffer.capacity == old_buffer.capacity);`
- `assert(buffer.len == old_buffer.len - len);`
- `assert(output.allocator == NULL);`
- `assert(output.buffer == NULL);`
- `assert(output.buffer == old_buffer.buffer);`
- `assert(output.capacity == len);`
- `assert(output.len == len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(len <= MAX_BUFFER_SIZE);
- __CPROVER_assume(len <= buffer.len);

rating: 

---

## 16. `M_claude` / `aws_string_eq_byte_buf` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(str->len == buf.len);`  
  → never written exactly, in any iteration
- `assert(aws_string_is_valid(str));`  
  → present in the final harness
- `assert(aws_byte_buf_is_valid(&buf));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(buf));`
- `assert(aws_byte_buf_is_valid(buf));`
- `assert(aws_string_is_valid(str));`
- `assert(aws_string_is_valid(str));`
- `assert(buf->allocator == old_buf_val.allocator);`
- `assert(buf->allocator == old_buf_val.allocator);`
- `assert(buf->buffer == old_buf_val.buffer);`
- `assert(buf->buffer == old_buf_val.buffer);`
- `assert(buf->capacity == old_buf_val.capacity);`
- `assert(buf->capacity == old_buf_val.capacity);`
- `assert(buf->len == old_buf_val.len);`
- `assert(buf->len == old_buf_val.len);`
- `assert(result == false);`
- `assert(result == false);`
- `assert(result == true);`
- `assert(str == old_str);`
- `assert(str == old_str);`
- `assert(str->allocator == old_str->allocator);`
- `assert(str->allocator == old_str->allocator);`
- `assert(str->len == old_str->len);`
- `assert(str->len == old_str->len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 17. `Oracle_gptoss120b` / `aws_byte_buf_from_empty_array` (3 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.capacity == capacity);`  
  → present in the final harness
- `assert(buf.allocator == NULL);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.buffer == NULL);`
- `assert(buf.buffer == bytes);`
- `assert(buf.capacity == capacity);`
- `assert(buf.len == 0);`
- `assert(raw[0] == 0xAA);`
- `assert(raw[capacity + 1] == 0xBB);`
- `assert(raw[i + 1] == raw[i + 1]);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(capacity <= MAX_CAPACITY);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 18. `H_gptoss120b` / `aws_byte_buf_clean_up` (4 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.allocator == NULL);`  
  → present in the final harness
- `assert(buf.buffer == NULL);`  
  → present in the final harness
- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.capacity == 0);`  
  → present in the final harness

**LLM harness, final iteration (4), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator != old_buf.allocator);`
- `assert(buf.allocator == NULL);`
- `assert(buf.buffer == NULL);`
- `assert(buf.capacity == 0);`
- `assert(buf.len == 0);`

rating: 

---

## 19. `H_claude` / `aws_byte_buf_reset` (3 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.allocator == old.allocator);`  
  → never written exactly, in any iteration
- `assert(buf.buffer == old.buffer);`  
  → never written exactly, in any iteration
- `assert(buf.capacity == old.capacity);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == old_buf.allocator);`
- `assert(buf.buffer == old_buf.buffer);`
- `assert(buf.capacity == old_buf.capacity);`
- `assert(buf.len == 0);`

rating: 

---

## 20. `Oracle_gptoss120b` / `aws_byte_buf_from_array` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(buf.len == length);`  
  → never written exactly, in any iteration
- `assert(buf.capacity == length);`  
  → never written exactly, in any iteration
- `assert(buf.allocator == NULL);`  
  → present in the final harness

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == NULL);`
- `assert(buf.buffer == NULL);`
- `assert(buf.buffer == bytes);`
- `assert(buf.capacity == len);`
- `assert(buf.len == len);`
- `assert(memcmp(bytes, bytes_copy, len) == 0);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(len <= 64);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 21. `Oracle_gptoss120b` / `aws_byte_cursor_from_array` (5 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_cursor_is_valid(&cur));`  
  → never written exactly, in any iteration
- `assert(cur.len == length);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `__CPROVER_assert(aws_byte_cursor_is_valid(&cur),`
- `__CPROVER_assert(cur.len == len,`
- `__CPROVER_assert(cur.ptr == (uint8_t *)bytes,`
- `__CPROVER_assert(src_buf[i] == src_buf_old[i],`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(len <= MAX_BUF_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 22. `A_gptoss120b` / `aws_ptr_eq` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(rval == (p1 == p2));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (3), assertions written:**

- `assert(result == (a == b));`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(choice_a >= 0 && choice_a <= 2);
- __CPROVER_assume(choice_b >= 0 && choice_b <= 2);

rating: 

---

## 23. `A_gptoss120b` / `aws_byte_buf_clean_up` (4 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.allocator == NULL);`  
  → present in the final harness
- `assert(buf.buffer == NULL);`  
  → present in the final harness
- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.capacity == 0);`  
  → present in the final harness

**LLM harness, final iteration (3), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == NULL);`
- `assert(buf.buffer == NULL);`
- `assert(buf.capacity == 0);`
- `assert(buf.len == 0);`
- `assert(old_buf.allocator == alloc);`
- `assert(old_buf.buffer != NULL);`
- `assert(old_buf.buffer == NULL);`
- `assert(old_buf.capacity == old_buf.capacity);`
- `assert(old_buf.len == old_buf.len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume((cap = nondet_size_t()) <= MAX_BUFFER_SIZE);
- __CPROVER_assume(buf.len <= buf.capacity);

rating: 

---

## 24. `Oracle_gptoss120b` / `aws_byte_buf_secure_zero` (5 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → never written exactly, in any iteration
- `assert(buf.len == 0);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));   /* still a valid buffer */`
- `assert(buf.buffer == NULL);`
- `assert(buf.buffer[i] == 0);`
- `assert(buf.len == 0);                  /* length reset to zero */`
- `assert(guard_after[i]  == guard_after_orig[i]);`
- `assert(guard_before[i] == guard_before_orig[i]);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(capacity <= MAX_CAPACITY);
- __CPROVER_assume(len <= capacity);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 25. `H_claude` / `aws_byte_buf_eq_c_str` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.len == str_len);`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == old_buf.allocator);`
- `assert(buf.buffer == old_buf.buffer);`
- `assert(buf.capacity == old_buf.capacity);`
- `assert(buf.len == old_buf.len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 26. `Oracle_gptoss120b` / `aws_nospec_mask` (35 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2)));`  
  → never written exactly, in any iteration
- `assert(rval == UINTPTR_MAX);`  
  → never written exactly, in any iteration
- `assert(!((index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2))));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert((in_range && result == UINTPTR_MAX) ||`
- `assert(result == 0 || result == UINTPTR_MAX);`
- `assert(snapshot[i] == snapshot[i]); /* trivially true, forces CBMC to keep snapshot unchanged */`

rating: 

---

## 27. `A_claude` / `aws_byte_buf_cat` (3 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer1));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer2));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer3));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&dest));`  
  → present in the final harness

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&dest));`
- `assert(aws_byte_buf_is_valid(&src1));`
- `assert(aws_byte_buf_is_valid(&src2));`
- `assert(dest.allocator == old_allocator);`
- `assert(dest.buffer == old_buffer);`
- `assert(dest.capacity == old_capacity);`
- `assert(dest.len <= dest.capacity);`
- `assert(dest.len <= dest.capacity);`
- `assert(dest.len == old_len + src1_old_len + src2_old_len);`
- `assert(dest.len >= old_len);`
- `assert(result == AWS_OP_ERR);`
- `assert(src1.len == src1_old_len);`
- `assert(src2.len == src2_old_len);`

rating: 

---

## 28. `H_claude` / `aws_byte_buf_cat` (9 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer1));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer2));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer3));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&dest));`  
  → present in the final harness

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(&dest));`
- `assert(aws_byte_buf_is_valid(&src0));`
- `assert(aws_byte_buf_is_valid(&src1));`
- `assert(dest.allocator == old_dest.allocator);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.capacity == old_dest.capacity);`
- `assert(dest.len <= dest.capacity);`
- `assert(dest.len <= dest.capacity);`
- `assert(dest.len == old_dest.len + src0.len + src1.len);`
- `assert(dest.len == old_dest.len + src0.len);`
- `assert(dest.len >= old_dest.len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(old_dest.len + src0.len + src1.len <= dest.capacity);

rating: 

---

## 29. `A_gptoss120b` / `aws_byte_buf_cat` (33 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer1));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer2));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer3));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&dest));`  
  → present in the final harness

**LLM harness, final iteration (3), assertions written:**

- `assert(aws_byte_buf_is_valid(&dest));`
- `assert(aws_byte_buf_is_valid(&src_bufs[i]));`
- `assert(dest.allocator == old_dest.allocator);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.capacity == old_dest.capacity);`
- `assert(dest.len <= old_dest.len + total_len);`
- `assert(dest.len == old_dest.len + total_len);`
- `assert(dest.len >= old_dest.len);`
- `assert(src_bufs[i].allocator == old_src_bufs[i].allocator);`
- `assert(src_bufs[i].buffer == old_src_bufs[i].buffer);`
- `assert(src_bufs[i].capacity == old_src_bufs[i].capacity);`
- `assert(src_bufs[i].len == old_src_bufs[i].len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(dest.len <= dest.capacity);
- __CPROVER_assume(number_of_args <= MAX_ARGS);
- __CPROVER_assume(src_bufs[i].len <= src_bufs[i].capacity);

rating: 

---

## 30. `M_gptoss120b` / `aws_nospec_mask` (3 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2)));`  
  → never written exactly, in any iteration
- `assert(rval == UINTPTR_MAX);`  
  → never written exactly, in any iteration
- `assert(!((index >= bound) || (bound > (SIZE_MAX / 2)) || (index > (SIZE_MAX / 2))));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert(result == 0 || result == UINTPTR_MAX);`
- `assert(result == 0);`
- `assert(result == UINTPTR_MAX);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(index <= MAX_BUFFER_SIZE);
- __CPROVER_assume(bound <= MAX_BUFFER_SIZE);

rating: 

---

## 31. `A_claude` / `aws_byte_buf_from_empty_array` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.capacity == capacity);`  
  → never written exactly, in any iteration
- `assert(buf.allocator == NULL);`  
  → present in the final harness

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == NULL);`
- `assert(buf.buffer == (uint8_t *)array);`
- `assert(buf.capacity == len);`
- `assert(buf.len == 0);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(len <= MAX_BUFFER_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 32. `Oracle_gptoss120b` / `aws_byte_buf_clean_up` (16 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.allocator == NULL);`  
  → never written exactly, in any iteration
- `assert(buf.buffer == NULL);`  
  → never written exactly, in any iteration
- `assert(buf.len == 0);`  
  → never written exactly, in any iteration
- `assert(buf.capacity == 0);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (2), assertions written:**

- `__CPROVER_assert(`
- `__CPROVER_assert(aws_byte_buf_is_valid(&buf), "postcondition: buffer remains valid");`
- `__CPROVER_assert(aws_byte_buf_is_valid(&buf), "precondition: buffer is valid");`
- `__CPROVER_assert(buf.allocator == NULL, "postcondition: allocator cleared");`
- `__CPROVER_assert(buf.buffer == NULL, "postcondition: buffer pointer cleared");`
- `__CPROVER_assert(buf.capacity == 0, "postcondition: capacity cleared");`
- `__CPROVER_assert(buf.len == 0, "postcondition: length cleared");`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(capacity <= 1024);
- __CPROVER_assume(len <= capacity);

rating: 

---

## 33. `Oracle_gptoss120b` / `aws_byte_buf_init` (17 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(buf.allocator == allocator);`  
  → present in the final harness
- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.capacity == capacity);`  
  → present in the final harness

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == NULL);`
- `assert(buf.allocator == allocator);`
- `assert(buf.buffer == NULL);`
- `assert(buf.buffer == NULL);`
- `assert(buf.capacity == 0);`
- `assert(buf.capacity == capacity);`
- `assert(buf.len == 0);`
- `assert(buf.len == 0);`
- `assert(memcmp(dummy, old_dummy, sizeof(dummy)) == 0);`
- `assert(result == AWS_OP_ERR);`
- `assert(result == AWS_OP_SUCCESS);`
- `assert(result == AWS_OP_SUCCESS);`

rating: 

---

## 34. `M_gptoss120b` / `aws_byte_buf_init_copy` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_byte_buf_is_valid(dest));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_has_allocator(dest));`  
  → never written exactly, in any iteration
- `assert(dest->len == src.len);`  
  → never written exactly, in any iteration
- `assert(dest->capacity == src.capacity);`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&src));`  
  → present in the final harness

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(&dest));`
- `assert(aws_byte_buf_is_valid(&src));`
- `assert(dest.allocator == NULL);`
- `assert(dest.allocator == allocator);`
- `assert(dest.buffer != NULL);`
- `assert(dest.buffer != src.buffer);`
- `assert(dest.buffer == NULL);`
- `assert(dest.buffer == NULL);`
- `assert(dest.capacity == 0);`
- `assert(dest.capacity == 0);`
- `assert(dest.capacity == src.capacity);`
- `assert(dest.len == 0);`
- `assert(dest.len == 0);`
- `assert(dest.len == src.len);`
- `assert(src.allocator == old_src.allocator);`
- `assert(src.buffer == old_src.buffer);`
- `assert(src.capacity == old_src.capacity);`
- `assert(src.len == old_src.len);`

rating: 

---

## 35. `M_claude` / `aws_string_eq_byte_cursor` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_string_is_valid(str));`  
  → present in the final harness
- `assert(str->len == cursor.len);`  
  → never written exactly, in any iteration
- `assert(aws_byte_cursor_is_valid(&cursor));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_cursor_is_valid(cur));`
- `assert(aws_string_is_valid(str));`
- `assert(cur->len == old_cur_len);`
- `assert(cur->ptr == old_cur_ptr);`
- `assert(result == false);`
- `assert(result == false);`
- `assert(result == true);`
- `assert(result == true);`
- `assert(str->len == old_str_len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
- __CPROVER_assume(cur_val.len <= MAX_BUFFER_SIZE);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 36. `M_gptoss120b` / `aws_byte_buf_cat` (4 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer1));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer2));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer3));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&dest));`  
  → present in the final harness

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&dest));`
- `assert(aws_byte_buf_is_valid(&src1));`
- `assert(aws_byte_buf_is_valid(&src2));`
- `assert(dest.allocator == old_dest.allocator);`
- `assert(dest.allocator == old_dest.allocator);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.capacity == old_dest.capacity);`
- `assert(dest.capacity == old_dest.capacity);`
- `assert(dest.len == old_dest.len + src1.len + src2.len);`
- `assert(src1.allocator == old_src1.allocator);`
- `assert(src1.allocator == old_src1.allocator);`
- `assert(src1.buffer == old_src1.buffer);`
- `assert(src1.buffer == old_src1.buffer);`
- `assert(src1.capacity == old_src1.capacity);`
- `assert(src1.capacity == old_src1.capacity);`
- `assert(src1.len == old_src1.len);`
- `assert(src1.len == old_src1.len);`
- `assert(src2.allocator == old_src2.allocator);`
- `assert(src2.allocator == old_src2.allocator);`
- `assert(src2.buffer == old_src2.buffer);`
- `assert(src2.buffer == old_src2.buffer);`
- `assert(src2.capacity == old_src2.capacity);`
- `assert(src2.capacity == old_src2.capacity);`
- `assert(src2.len == old_src2.len);`
- `assert(src2.len == old_src2.len);`

rating: 

---

## 37. `A_claude` / `aws_byte_buf_reset` (3 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.len == 0);`  
  → present in the final harness
- `assert(buf.allocator == old.allocator);`  
  → never written exactly, in any iteration
- `assert(buf.buffer == old.buffer);`  
  → never written exactly, in any iteration
- `assert(buf.capacity == old.capacity);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (2), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(buf.allocator == buf_old.allocator);`
- `assert(buf.buffer == buf_old.buffer);`
- `assert(buf.capacity == buf_old.capacity);`
- `assert(buf.len == 0);`

rating: 

---

## 38. `Oracle_gptoss120b` / `aws_byte_buf_advance` (11 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(buf.len == old.len + len);`  
  → never written exactly, in any iteration
- `assert(buf.capacity == old.capacity);`  
  → never written exactly, in any iteration
- `assert(buf.allocator == old.allocator);`  
  → never written exactly, in any iteration
- `assert(output.len == 0);`  
  → present in the final harness
- `assert(output.capacity == len);`  
  → present in the final harness
- `assert(output.allocator == NULL);`  
  → never written exactly, in any iteration
- `assert(output.len == 0);`  
  → present in the final harness
- `assert(output.capacity == 0);`  
  → written exactly in iteration 1, absent from the final; CBMC at iteration 1 returned FAIL
- `assert(output.allocator == NULL);`  
  → never written exactly, in any iteration
- `assert(output.buffer == NULL);`  
  → written exactly in iteration 1, absent from the final; CBMC at iteration 1 returned FAIL
- `assert(aws_byte_buf_is_valid(&buf));`  
  → present in the final harness
- `assert(aws_byte_buf_is_valid(&output));`  
  → present in the final harness

**LLM harness, final iteration (3), assertions written:**

- `assert(aws_byte_buf_is_valid(&buf));`
- `assert(aws_byte_buf_is_valid(&output));`
- `assert(buf.buffer == old_buf_ptr);`
- `assert(buf.capacity == old_buf_capacity);`
- `assert(buf.len == old_buf_len + len);`
- `assert(memcmp(buf.buffer, buf_mem_copy, buf.capacity) == 0);`
- `assert(output.buffer == (old_buf_ptr == NULL ? NULL : old_buf_ptr + old_buf_len));`
- `assert(output.capacity == len);`
- `assert(output.len == 0);`
- `assert(ret == true);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
- __CPROVER_assume(buf.len <= buf.capacity);
- __CPROVER_assume(len <= buf.capacity - buf.len);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 39. `Oracle_gptoss120b` / `aws_string_new_from_c_str` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(str->len <= MAX_STRING_LEN);`  
  → never written exactly, in any iteration
- `assert(str->bytes[str->len] == 0);`  
  → never written exactly, in any iteration
- `assert(aws_string_is_valid(str));`  
  → never written exactly, in any iteration
- `assert(aws_c_string_is_valid(c_str));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert(((const uint8_t *)result->bytes)[c_len] == (uint8_t)'\0');`
- `assert(((const uint8_t *)result->bytes)[i] == (uint8_t)c_str[i]);`
- `assert(c_str[i] == c_str_old[i]);`
- `assert(result == NULL || aws_string_is_valid(result));`
- `assert(result->len == c_len);`

**Assume-envelope comparison (LLM vs expert):**

- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 40. `Oracle_gptoss120b` / `aws_array_list_swap` (25 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(aws_array_list_is_valid(&list));`  
  → present in the final harness
- `assert(list.alloc == old.alloc);`  
  → never written exactly, in any iteration
- `assert(list.current_size == old.current_size);`  
  → never written exactly, in any iteration
- `assert(list.length == old.length);`  
  → never written exactly, in any iteration
- `assert(list.item_size == old.item_size);`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_array_list_is_valid(&list));`
- `assert(list.alloc == old_alloc);`
- `assert(list.current_size == old_current_size);`
- `assert(list.data == old_data_ptr);`
- `assert(list.item_size == old_item_size);`
- `assert(list.length == old_length);`
- `assert(new_elem[j] == old_a[j]);`
- `assert(new_elem[j] == old_b[j]);`
- `assert(new_elem[j] == old_elem[j]);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
- __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
- __CPROVER_assume(len <= list.length);
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 41. `H_claude` / `aws_string_destroy` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- (no GT assertion could be parsed from the expert harness)

**LLM harness, final iteration (2), assertions written:**

- `assert(str == NULL);`
- `assert(was_null ? (str == NULL) : true);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(len <= 10); /* bound the string length */
- [malloc-vs-ASSUME_VALID_MEMORY]

rating: 

---

## 42. `M_gptoss120b` / `aws_string_destroy` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- (no GT assertion could be parsed from the expert harness)

**LLM harness, final iteration (9), assertions written:**

- `assert(str == NULL);`
- `assert(str == old_str);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(len <= MAX_BUFFER_SIZE);

rating: 

---

## 43. `Oracle_gptoss120b` / `aws_ptr_eq` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(rval == (p1 == p2));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (1), assertions written:**

- `assert(buf_a[0] == a_byte_snapshot);`
- `assert(buf_b[0] == b_byte_snapshot);`
- `assert(result == (a == b));`

rating: 

---

## 44. `M_gptoss120b` / `aws_ptr_eq` (2 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert(rval == (p1 == p2));`  
  → never written exactly, in any iteration

**LLM harness, final iteration (6), assertions written:**

- `assert(a == old_a);`
- `assert(b == old_b);`
- `assert(buf1.allocator == old_buf1.allocator);`
- `assert(buf1.buffer == old_buf1.buffer);`
- `assert(buf1.capacity == old_buf1.capacity);`
- `assert(buf1.len == old_buf1.len);`
- `assert(buf2.allocator == old_buf2.allocator);`
- `assert(buf2.buffer == old_buf2.buffer);`
- `assert(buf2.capacity == old_buf2.capacity);`
- `assert(buf2.len == old_buf2.len);`
- `assert(result == (old_a == old_b));`

rating: 

---

## 45. `G_gptoss120b` / `aws_byte_buf_cat` (1 silenced mutants)

**Expert (GT) assertions and what the LLM harness did with each:**

- `assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer1));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer2));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&buffer3));`  
  → never written exactly, in any iteration
- `assert(aws_byte_buf_is_valid(&dest));`  
  → present in the final harness

**LLM harness, final iteration (1), assertions written:**

- `assert(aws_byte_buf_is_valid(&dest));`
- `assert(aws_byte_buf_is_valid(&src0));`
- `assert(aws_byte_buf_is_valid(&src1));`
- `assert(aws_byte_buf_is_valid(&src2));`
- `assert(aws_byte_buf_is_valid(&src3));`
- `assert(dest.allocator == old_dest.allocator);`
- `assert(dest.buffer == old_dest.buffer);`
- `assert(dest.capacity == old_dest.capacity);`
- `assert(dest.len <= old_dest.len + total_len);`
- `assert(dest.len == old_dest.len + total_len);`
- `assert(dest.len >= old_dest.len);`
- `assert(src0.allocator == old_src0.allocator);`
- `assert(src0.buffer == old_src0.buffer);`
- `assert(src0.capacity == old_src0.capacity);`
- `assert(src0.len == old_src0.len);`
- `assert(src1.allocator == old_src1.allocator);`
- `assert(src1.buffer == old_src1.buffer);`
- `assert(src1.capacity == old_src1.capacity);`
- `assert(src1.len == old_src1.len);`
- `assert(src2.allocator == old_src2.allocator);`
- `assert(src2.buffer == old_src2.buffer);`
- `assert(src2.capacity == old_src2.capacity);`
- `assert(src2.len == old_src2.len);`
- `assert(src3.allocator == old_src3.allocator);`
- `assert(src3.buffer == old_src3.buffer);`
- `assert(src3.capacity == old_src3.capacity);`
- `assert(src3.len == old_src3.len);`

**Assume-envelope comparison (LLM vs expert):**

- __CPROVER_assume(number_of_args >= 2 && number_of_args <= MAX_ARGS);

rating: 

---
