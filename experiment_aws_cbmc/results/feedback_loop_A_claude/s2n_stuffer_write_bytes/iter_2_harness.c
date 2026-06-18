#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>

/* s2n struct definitions needed for the harness */
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

/* S2N result codes */
#define S2N_SUCCESS 0
#define S2N_FAILURE -1

/* Forward declaration of function under test */
int s2n_stuffer_write_bytes(struct s2n_stuffer *stuffer, const uint8_t *data, const uint32_t size);

/* Validity predicate for s2n_stuffer */
bool s2n_stuffer_is_valid(const struct s2n_stuffer *stuffer) {
    if (stuffer == NULL) return false;
    if (stuffer->read_cursor > stuffer->write_cursor) return false;
    if (stuffer->write_cursor > stuffer->blob.size) return false;
    if (stuffer->high_water_mark < stuffer->write_cursor) return false;
    if (stuffer->blob.size > 0 && stuffer->blob.data == NULL) return false;
    return true;
}

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void s2n_stuffer_write_bytes_harness(void) {
    /* 1. Set up the stuffer */
    struct s2n_stuffer stuffer;

    /* Allocate blob data non-deterministically */
    uint32_t blob_size;
    __CPROVER_assume(blob_size <= MAX_BUFFER_SIZE);

    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }
    stuffer.blob.size = blob_size;
    __CPROVER_assume(stuffer.blob.allocated >= blob_size);
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Set up cursors consistently */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= blob_size);
    __CPROVER_assume(stuffer.high_water_mark >= stuffer.write_cursor);
    __CPROVER_assume(stuffer.high_water_mark <= blob_size);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Assume valid stuffer before call */
    __CPROVER_assume(s2n_stuffer_is_valid(&stuffer));

    /* 2. Set up input data */
    uint32_t in_size;
    __CPROVER_assume(in_size <= MAX_BUFFER_SIZE);

    uint8_t *in_data = NULL;
    if (in_size > 0) {
        in_data = malloc(in_size);
        __CPROVER_assume(in_data != NULL);
    }

    /* 3. Save old state */
    struct s2n_stuffer old_stuffer = stuffer;
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_read_cursor = stuffer.read_cursor;

    /* 4. Call function under test */
    int result = s2n_stuffer_write_bytes(&stuffer, in_data, in_size);

    /* 5. Assert postconditions */
    if (result == S2N_SUCCESS) {
        if (in_size == 0) {
            assert(stuffer.write_cursor == old_write_cursor);
            assert(stuffer.read_cursor == old_read_cursor);
        } else {
            assert(stuffer.write_cursor == old_write_cursor + in_size);
            assert(stuffer.read_cursor == old_read_cursor);
            assert(stuffer.high_water_mark >= stuffer.write_cursor);
        }
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
    } else {
        assert(stuffer.read_cursor == old_read_cursor);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
    }

    /* 6. Validity invariant */
    assert(s2n_stuffer_is_valid(&stuffer));
}
