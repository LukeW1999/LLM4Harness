#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* Minimal s2n type definitions needed for the harness */
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

/* s2n return codes */
#define S2N_SUCCESS 0
#define S2N_FAILURE -1

/* Forward declaration of function under test */
int s2n_stuffer_read_uint8(struct s2n_stuffer *stuffer, uint8_t *u);

/* s2n_stuffer validity predicate */
static bool s2n_stuffer_is_valid(const struct s2n_stuffer *s) {
    if (s == NULL) return false;
    /* read_cursor <= write_cursor <= blob.size */
    if (s->read_cursor > s->write_cursor) return false;
    if (s->write_cursor > s->blob.size) return false;
    /* high_water_mark >= write_cursor */
    if (s->high_water_mark < s->write_cursor) return false;
    /* if blob.size > 0, data must be non-null */
    if (s->blob.size > 0 && s->blob.data == NULL) return false;
    return true;
}

void s2n_stuffer_read_uint8_harness(void) {
    /* 1. Declare and initialize the stuffer */
    struct s2n_stuffer stuffer;

    /* Non-deterministic blob size, bounded to keep state space manageable */
    uint32_t blob_size = nondet_uint32_t();
    __CPROVER_assume(blob_size <= 64);

    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= blob_size || stuffer.blob.allocated == 0);
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Allocate blob data if size > 0 */
    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Set up cursors consistently */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();

    /* Assume valid stuffer state */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark >= stuffer.write_cursor);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* 2. Declare output parameter */
    uint8_t u;

    /* 3. Save old state */
    struct s2n_stuffer old_stuffer = stuffer;
    uint32_t old_read_cursor = stuffer.read_cursor;
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_high_water_mark = stuffer.high_water_mark;

    /* 4. Call function under test */
    int result = s2n_stuffer_read_uint8(&stuffer, &u);

    /* 5. Assert postconditions */
    if (result == S2N_SUCCESS) {
        /* On success: read_cursor advances by sizeof(uint8_t) = 1 */
        assert(stuffer.read_cursor == old_read_cursor + 1);

        /* write_cursor unchanged */
        assert(stuffer.write_cursor == old_write_cursor);

        /* high_water_mark unchanged */
        assert(stuffer.high_water_mark == old_high_water_mark);

        /* blob fields unchanged */
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);

        /* Other stuffer flags unchanged */
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);

        /* There must have been at least 1 byte available to read */
        assert(old_write_cursor >= old_read_cursor + 1);

        /* The value read must come from the blob data */
        assert(u == old_stuffer.blob.data[old_read_cursor]);

        /* Stuffer remains valid */
        assert(stuffer.read_cursor <= stuffer.write_cursor);
        assert(stuffer.write_cursor <= stuffer.blob.size);

    } else {
        /* On failure: stuffer should be unchanged */
        assert(stuffer.read_cursor == old_read_cursor);
        assert(stuffer.write_cursor == old_write_cursor);
        assert(stuffer.high_water_mark == old_high_water_mark);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
    }
}

void s2n_stuffer_read_uint8_harness(void) {
    s2n_stuffer_read_uint8_harness();
    return 0;
}
