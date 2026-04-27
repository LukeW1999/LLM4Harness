// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_from_c_str returns a valid struct aws_byte_buf:
//   - buf.len: SET to the length of the input string (or 0 if c_str is NULL)
//   - buf.capacity: SET to the length of the input string (or 0 if c_str is NULL)
//   - buf.buffer: SET to the input string (or NULL if c_str is NULL)
//   - buf.allocator: SET to NULL

// === STEP 2: FAILURE PATH ===
// aws_byte_buf_from_c_str cannot fail as it does not allocate memory and simply initializes the struct
// Thus, there is no failure path in terms of return value, but we can consider the case where c_str is NULL
//   - buf.len: SET to 0
//   - buf.capacity: SET to 0
//   - buf.buffer: SET to NULL
//   - buf.allocator: SET to NULL

// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - len: CHANGED on success (or failure, but always 0 if failure)
//   - capacity: CHANGED on success (or failure, but always 0 if failure)
//   - buffer: CHANGED on success (or failure, but always NULL if failure)
//   - allocator: CHANGED on success (or failure, but always NULL if failure)

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_byte_buf_from_c_str_harness() {
    const char *c_str = nondet_bool() ? NULL : "example";
    struct aws_byte_buf old_buf;
    old_buf.len = nondet_size_t();
    old_buf.capacity = nondet_size_t();
    old_buf.buffer = (uint8_t *)nondet_voidp();
    old_buf.allocator = (struct aws_allocator *)nondet_voidp();

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    // Frame conditions
    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else {
        assert(buf.len == strlen(c_str));
        assert(buf.capacity == strlen(c_str));
        assert(buf.buffer == (uint8_t *)c_str);
        assert(buf.allocator == NULL);
    }

    // Validity invariants
    assert(aws_byte_buf_is_valid(&buf));
}
