#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* Save a byte from the original buffer (if any) for later zero‑check */
    struct store_byte_from_buffer old_byte;
    if (old.buffer != NULL && old.capacity > 0) {
        save_byte_from_array(old.buffer, old.capacity, &old_byte);
    }

    /* 3. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Post‑conditions that must hold after the call */

    /* The buffer should be released */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* Allocator is not modified */
    assert(buf.allocator == old.allocator);

    /* If there was an original allocation, its contents must have been zeroed
       before being freed. We check the saved byte. */
    if (old.buffer != NULL && old.capacity > 0) {
        /* The saved index is stored inside old_byte.idx (implementation‑defined). */
        assert(old.buffer[old_byte.idx] == 0);
    }

    /* 5. The invariant that a byte buffer is always valid must still hold */
    assert(aws_byte_buf_is_valid(&buf));
}
