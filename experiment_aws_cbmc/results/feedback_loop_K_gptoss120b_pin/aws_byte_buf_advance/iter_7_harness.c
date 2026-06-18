/*=== Contract for aws_byte_buf_advance ===
Preconditions:
  - __CPROVER_assume(buffer != NULL);
  - __CPROVER_assume(output != NULL);
  - __CPROVER_assume(buffer->capacity >= buffer->len);
  - __CPROVER_assume(buffer->capacity > 0);               // ensure buffer->buffer is non‑NULL
  - __CPROVER_assume(buffer->capacity == 0 || buffer->buffer != NULL);
  - __CPROVER_assume(buffer->len <= buffer->capacity);
  - __CPROVER_assume(aws_byte_buf_is_valid(buffer));
  - __CPROVER_assume(aws_byte_buf_is_valid(output));
Postconditions (validity):
  - If return true:
        * output->capacity == len
        * output->len == 0
        * output->allocator == NULL
  - If return false:
        * output->buffer == NULL
        * output->len == 0
        * output->capacity == 0
        * output->allocator == NULL
Postconditions (length):
  - If return true: buffer->len == old_len + len
  - If return false: buffer->len == old_len
  - buffer->capacity unchanged
  - buffer->buffer unchanged
  - buffer->allocator unchanged
Postconditions (frame):
  - No memory outside of *buffer and *output is modified.
===*/

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf *buffer;
    struct aws_byte_buf *output;
    size_t len;

    /* Allocate the structs */
    buffer = malloc(sizeof(struct aws_byte_buf));
    output = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buffer != NULL);
    __CPROVER_assume(output != NULL);

    /* Nondeterministic initialization of buffer fields */
    buffer->len = nondet_size_t();
    buffer->capacity = nondet_size_t();
    buffer->allocator = NULL; /* allocator not used in this function */

    /* Ensure capacity >= len to avoid overflow in subtraction */
    __CPROVER_assume(buffer->capacity >= buffer->len);
    __CPROVER_assume(buffer->capacity > 0);               // guarantee non‑NULL underlying buffer

    /* Allocate underlying byte array */
    buffer->buffer = malloc(buffer->capacity);
    __CPROVER_assume(buffer->buffer != NULL);

    /* Initialize output as a valid empty buffer */
    output->buffer = NULL;
    output->capacity = 0;
    output->len = 0;
    output->allocator = NULL;

    /* Assume output is valid per library contract */
    __CPROVER_assume(aws_byte_buf_is_valid(output));

    /* Save old state for later comparison */
    struct aws_byte_buf old_buffer = *buffer;
    struct aws_byte_buf old_output = *output;

    /* Nondeterministic len */
    len = nondet_size_t();

    /* Prevent unsigned overflow in the addition performed by the function */
    __CPROVER_assume(buffer->len + len >= buffer->len);

    /* Ensure there is enough remaining capacity so the function can succeed. */
    __CPROVER_assume(len <= buffer->capacity - buffer->len);

    /* Call the function under test */
    bool result = aws_byte_buf_advance(buffer, output, len);

    /* Postcondition checks */
    if (result) {
        __CPROVER_assert(buffer->capacity - old_buffer.len >= len,
                         "buffer had enough capacity for advance");
        __CPROVER_assert(buffer->len == old_buffer.len + len,
                         "buffer len increased correctly");
        __CPROVER_assert(buffer->capacity == old_buffer.capacity,
                         "buffer capacity unchanged");
        __CPROVER_assert(buffer->buffer == old_buffer.buffer,
                         "buffer pointer unchanged");

        /* Use current buffer state to avoid reliance on old struct for pointer arithmetic */
        __CPROVER_assert(output->buffer == buffer->buffer + (buffer->len - len),
                         "output buffer points to correct location");
        __CPROVER_assert(output->capacity == len,
                         "output capacity set to len");
        __CPROVER_assert(output->len == 0,
                         "output len set to 0");
        __CPROVER_assert(output->allocator == NULL,
                         "output allocator is NULL");
    } else {
        __CPROVER_assert(buffer->len == old_buffer.len,
                         "buffer len unchanged on failure");
        __CPROVER_assert(buffer->capacity == old_buffer.capacity,
                         "buffer capacity unchanged on failure");
        __CPROVER_assert(buffer->buffer == old_buffer.buffer,
                         "buffer pointer unchanged on failure");
        __CPROVER_assert(output->buffer == NULL,
                         "output buffer NULL on failure");
        __CPROVER_assert(output->len == 0,
                         "output len 0 on failure");
        __CPROVER_assert(output->capacity == 0,
                         "output capacity 0 on failure");
        __CPROVER_assert(output->allocator == NULL,
                         "output allocator NULL on failure");
    }

    /* Frame condition: no other memory modified */
    __CPROVER_assert(__builtin_memcmp(output, &old_output, sizeof(struct aws_byte_buf)) == 0 ||
                     result,
                     "output struct only modified as specified");
    __CPROVER_assert(__builtin_memcmp(buffer, &old_buffer, sizeof(struct aws_byte_buf)) == 0 ||
                     result,
                     "buffer struct only modified as specified");
}
