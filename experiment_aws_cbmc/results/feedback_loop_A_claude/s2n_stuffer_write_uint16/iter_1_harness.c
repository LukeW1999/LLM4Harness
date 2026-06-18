#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* Minimal s2n struct definitions needed for the harness */
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

/* Forward declaration of the function under test */
int s2n_stuffer_write_uint16(struct s2n_stuffer *stuffer, const uint16_t u);

/* s2n validity predicate for stuffer */
static bool s2n_stuffer_is_valid(const struct s2n_stuffer *stuffer) {
    if (stuffer == NULL) return false;
    /* read_cursor <= write_cursor <= blob.size */
    if (stuffer->read_cursor > stuffer->write_cursor) return false;
    if (stuffer->write_cursor > stuffer->blob.size) return false;
    /* high_water_mark >= write_cursor */
    if (stuffer->high_water_mark < stuffer->write_cursor) return false;
    /* if blob.size > 0, data must be non-null */
    if (stuffer->blob.size > 0 && stuffer->blob.data == NULL) return false;
    return true;
}

void s2n_stuffer_write_uint16_harness(void) {
    /* 1. Declare and set up the stuffer */
    struct s2n_stuffer stuffer;

    /* Non-deterministic blob data */
    uint32_t blob_size;
    __CPROVER_assume(blob_size <= 64); /* bound the buffer size */
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = blob_size;
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Non-deterministic cursors, constrained to be valid */
    uint32_t write_cursor;
    uint32_t read_cursor;
    uint32_t high_water_mark;

    __CPROVER_assume(write_cursor <= blob_size);
    __CPROVER_assume(read_cursor <= write_cursor);
    __CPROVER_assume(high_water_mark >= write_cursor);
    __CPROVER_assume(high_water_mark <= blob_size);

    stuffer.write_cursor = write_cursor;
    stuffer.read_cursor = read_cursor;
    stuffer.high_water_mark = high_water_mark;
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Assume valid initial state */
    __CPROVER_assume(s2n_stuffer_is_valid(&stuffer));

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;
    uint8_t *old_data = stuffer.blob.data;

    /* 3. Non-deterministic value to write */
    uint16_t u = nondet_uint8_t(); /* any uint16 value */
    u = (uint16_t)nondet_uint32_t();

    /* 4. Call function under test */
    int result = s2n_stuffer_write_uint16(&stuffer, u);

    /* 5. Assert postconditions */
    if (result == 0) {
        /* Success: write_cursor advanced by sizeof(uint16_t) = 2 */
        assert(stuffer.write_cursor == old.write_cursor + 2);

        /* read_cursor unchanged */
        assert(stuffer.read_cursor == old.read_cursor);

        /* blob unchanged */
        assert(stuffer.blob.data == old_data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);

        /* stuffer metadata unchanged */
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);

        /* The two bytes written should be in network (big-endian) order */
        if (old_data != NULL && old.write_cursor + 2 <= old.blob.size) {
            assert(stuffer.blob.data[old.write_cursor] == (uint8_t)(u >> 8));
            assert(stuffer.blob.data[old.write_cursor + 1] == (uint8_t)(u & 0xFF));
        }

        /* high_water_mark >= write_cursor */
        assert(stuffer.high_water_mark >= stuffer.write_cursor);

        /* Validity still holds */
        assert(s2n_stuffer_is_valid(&stuffer));
    } else {
        /* Failure: result should be -1 (S2N_RESULT_ERROR) */
        assert(result == -1);

        /* On failure, stuffer state should be unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    }
}

void s2n_stuffer_write_uint16_harness(void) {
    s2n_stuffer_write_uint16_harness();
    return 0;
}
