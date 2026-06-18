#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>

/* Nondeterministic helpers */
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

#define MAX_BUFFER_SIZE 1024
#define S2N_SUCCESS 0

/* Re-declare the structs as described in the specification */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

/* Function under test */
int s2n_stuffer_erase_and_read(struct s2n_stuffer *stuffer, struct s2n_blob *out);

void s2n_stuffer_erase_and_read_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct s2n_stuffer stuffer;
    struct s2n_blob out;

    /* nondet sizes bounded by MAX_BUFFER_SIZE */
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= MAX_BUFFER_SIZE);
    out.size = nondet_uint32_t();
    __CPROVER_assume(out.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(out.size <= stuffer.blob.size);

    /* allocate blob data */
    stuffer.blob.data = (uint8_t *)aws_mem_acquire(allocator, stuffer.blob.size);
    __CPROVER_assume(stuffer.blob.data != NULL);
    out.data = (uint8_t *)aws_mem_acquire(allocator, out.size);
    __CPROVER_assume(out.data != NULL);

    /* set other fields nondeterministically */
    stuffer.blob.allocated = stuffer.blob.size;
    stuffer.blob.growable = nondet_bool();

    out.allocated = out.size;
    out.growable = nondet_bool();

    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool();
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    /* Ensure cursors are within the blob bounds */
    if (stuffer.read_cursor > stuffer.blob.size) {
        stuffer.read_cursor = stuffer.blob.size;
    }
    if (stuffer.write_cursor > stuffer.blob.size) {
        stuffer.write_cursor = stuffer.blob.size;
    }

    /* Ensure we have enough data to read */
    __CPROVER_assume(stuffer.read_cursor + out.size <= stuffer.blob.size);

    /* Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = stuffer;
    struct s2n_blob old_out = out;

    /* Save copies of the memory regions that may be modified */
    uint8_t *old_region = NULL;
    if (out.size > 0) {
        old_region = (uint8_t *)aws_mem_acquire(allocator, out.size);
        __CPROVER_assume(old_region != NULL);
        for (uint32_t i = 0; i < out.size; ++i) {
            old_region[i] = stuffer.blob.data[old_stuffer.read_cursor + i];
        }
    }

    uint8_t *old_out_data = NULL;
    if (out.size > 0) {
        old_out_data = (uint8_t *)aws_mem_acquire(allocator, out.size);
        __CPROVER_assume(old_out_data != NULL);
        for (uint32_t i = 0; i < out.size; ++i) {
            old_out_data[i] = out.data[i];
        }
    }

    /* Call function under test */
    int result = s2n_stuffer_erase_and_read(&stuffer, &out);

    /* Assert postconditions for BOTH success and failure paths */
    if (result == S2N_SUCCESS) {
        /* read_cursor advanced by out.size */
        assert(stuffer.read_cursor == old_stuffer.read_cursor + out.size);

        /* out data now contains the bytes that were at the original read position */
        for (uint32_t i = 0; i < out.size; ++i) {
            assert(out.data[i] == old_region[i]);
        }

        /* the region in the stuffer has been zeroed */
        for (uint32_t i = 0; i < out.size; ++i) {
            assert(stuffer.blob.data[old_stuffer.read_cursor + i] == 0);
        }
    } else {
        /* out data must be unchanged */
        for (uint32_t i = 0; i < out.size; ++i) {
            assert(out.data[i] == old_out_data[i]);
        }

        /* stuffer fields other than read_cursor must be unchanged */
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        assert(stuffer.write_cursor == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);

        /* read_cursor is either unchanged or advanced by out.size */
        assert(stuffer.read_cursor == old_stuffer.read_cursor ||
               stuffer.read_cursor == old_stuffer.read_cursor + out.size);

        /* the blob region must remain unchanged */
        for (uint32_t i = 0; i < out.size; ++i) {
            assert(stuffer.blob.data[old_stuffer.read_cursor + i] == old_region[i]);
        }
    }

    /* Fields that must NOT change regardless of result */
    assert(stuffer.blob.data == old_stuffer.blob.data);
    assert(stuffer.blob.size == old_stuffer.blob.size);
    assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
    assert(stuffer.blob.growable == old_stuffer.blob.growable);
    assert(stuffer.write_cursor == old_stuffer.write_cursor);
    assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
    assert(stuffer.alloced == old_stuffer.alloced);
    assert(stuffer.growable == old_stuffer.growable);
    assert(stuffer.tainted == old_stuffer.tainted);

    assert(out.data == old_out.data);
    assert(out.size == old_out.size);
    assert(out.allocated == old_out.allocated);
    assert(out.growable == old_out.growable);

    /* Internal pointers remain non‑NULL when sizes are non‑zero */
    if (stuffer.blob.size > 0) {
        assert(stuffer.blob.data != NULL);
    }
    if (out.size > 0) {
        assert(out.data != NULL);
    }
}
