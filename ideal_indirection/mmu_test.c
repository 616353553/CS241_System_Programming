/**
* Ideal Indirection Lab
* CS 241 - Fall 2018
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mmu.h"
#define TEST_PID 42

/**
 * This asks the mmu to read from one virutal address:
 */
int read_1_virtual_address(mmu *test_mmu) {
    addr32 virtual_address = PAGE_SIZE;
    char buff[PAGE_SIZE];
    mmu_read_from_virtual_address(test_mmu, virtual_address, TEST_PID, buff, 1);
    assert(test_mmu->num_page_faults == 0);
    assert(test_mmu->num_segmentation_faults == 0);
    assert(test_mmu->num_tlb_misses == 1);
    return 0;
}

int read_and_write_1_virtual_address(mmu *test_mmu) {
    addr32 virtual_address = PAGE_SIZE;
    char buff[PAGE_SIZE];
    char *test_string = "hello world";

    mmu_write_to_virtual_address(test_mmu, virtual_address, TEST_PID,
                                 test_string, strlen(test_string) + 1);
    mmu_read_from_virtual_address(test_mmu, virtual_address, TEST_PID, buff,
                                  strlen(test_string) + 1);
    assert(strcmp(buff, test_string) == 0);

    assert(test_mmu->num_page_faults == 0);
    assert(test_mmu->num_segmentation_faults == 0);
    assert(test_mmu->num_tlb_misses == 1);

    return 0;
}

/**
 * This asks the mmu to read and write to different virtual adddress interleaved
 * among each other:
 *  Checks that your mmu isn't reading and writing to the same buffer.
 *  Checks that the reads and writes are persistent.
 *  Checks that the number of faults and misses are correct.
 */
int interleave(mmu *test_mmu) {
    addr32 virtual_address1 = PAGE_SIZE;
    addr32 virtual_address2 = PAGE_SIZE * 2;
    char buff1[PAGE_SIZE];
    char buff2[PAGE_SIZE];
    char *test_string1 = "hello world";
    char *test_string2 = "foobar";

    // Write to two different pages
    mmu_write_to_virtual_address(test_mmu, virtual_address1, TEST_PID,
                                 test_string1, strlen(test_string1) + 1);
    mmu_write_to_virtual_address(test_mmu, virtual_address2, TEST_PID,
                                 test_string2, strlen(test_string2) + 1);

    // Make sure that the data did not get corrupted.
    mmu_read_from_virtual_address(test_mmu, virtual_address1, TEST_PID, buff1,
                                  strlen(test_string1) + 1);
    assert(strcmp(buff1, test_string1) == 0);
    mmu_read_from_virtual_address(test_mmu, virtual_address2, TEST_PID, buff2,
                                  strlen(test_string2) + 1);
    assert(strcmp(buff2, test_string2) == 0);

    assert(test_mmu->num_page_faults == 0);
    assert(test_mmu->num_segmentation_faults == 0);
    // we still need to traverse the table 2 times, since the base address
    // differ.
    assert(test_mmu->num_tlb_misses == 2);
    return 0;
}

/**
 * spams read_and_write_1_virtual_address() a bunch.
 * This is to test if they are taking advantage of the tlb.
 */
int spam1(mmu *test_mmu) {
    for (size_t i = 0; i < 100; i++) {
        read_and_write_1_virtual_address(test_mmu);
    }

    return 0;
}

/**
 * This tests two processes asking for the same virtual address.
 *  Checks to see that the physical address are different for the same virtual
 * address of different processes
 *  Checks to see that you are flushing the TLB, since you are context switching
 * from different processes.
 */
int multiprocess(mmu *test_mmu) {
    addr32 virtual_address = PAGE_SIZE * 2;
    size_t pid1 = 1234;
    size_t pid2 = 9001;
    mmu_add_process(test_mmu, pid1);
    mmu_add_process(test_mmu, pid2);

    char buff1[PAGE_SIZE];
    char buff2[PAGE_SIZE];
    char *test_string1 = "hello world";
    char *test_string2 = "foobar";

    // Write to the same virtual address for 2 different processes
    mmu_write_to_virtual_address(test_mmu, virtual_address, pid1, test_string1,
                                 strlen(test_string1) + 1);
    mmu_write_to_virtual_address(test_mmu, virtual_address, pid2, test_string2,
                                 strlen(test_string2) + 1);

    // Make sure that the data did not get corrupted.
    mmu_read_from_virtual_address(test_mmu, virtual_address, pid1, buff1,
                                  strlen(test_string1) + 1);
    assert(strcmp(buff1, test_string1) == 0);
    mmu_read_from_virtual_address(test_mmu, virtual_address, pid2, buff2,
                                  strlen(test_string2) + 1);
    assert(strcmp(buff2, test_string2) == 0);

    assert(test_mmu->num_page_faults == 0);
    assert(test_mmu->num_segmentation_faults == 0);
    // The context switch means we tlb miss even for the same virtual address,
    // because
    // we are going from process 1->2->1->2
    assert(test_mmu->num_tlb_misses == 4);
    return 0;
}

/**
 * This tests if you are effectively using the TLB for the fact that
 * "all virtual addresses with the same virtual page numbers will map to the
 * same frame in memory".
 */
int same_vpns_diff_offset(mmu *test_mmu) {
    // Notice that the offset is 0.
    addr32 virtual_address = PAGE_SIZE;
    char buff[PAGE_SIZE];

    for (size_t offset = 0; offset < PAGE_SIZE; offset++) {
        // Add the current offset to the begining of the frame
        // Notice that this virtual address has the same virtual page numbers
        addr32 virtual_address_increment_offset = virtual_address + offset;
        char test_char = virtual_address_increment_offset;
        mmu_write_to_virtual_address(test_mmu, virtual_address_increment_offset,
                                     TEST_PID, &test_char, 1);
    }

    // Read it all back to see if it checks out
    mmu_read_from_virtual_address(test_mmu, virtual_address, TEST_PID, buff,
                                  PAGE_SIZE);
    for (size_t offset = 0; offset < PAGE_SIZE; offset++) {
        addr32 virtual_address_increment_offset = virtual_address + offset;
        char test_char = virtual_address_increment_offset;
        assert(buff[offset] == test_char);
    }

    assert(test_mmu->num_segmentation_faults == 0);
    assert(test_mmu->num_page_faults == 0);
    // We should only cache miss for the first lookup
    // and everything else in the frame is a cache hit.
    assert(test_mmu->num_tlb_misses == 1);
    return 0;
}

// This should dereference NULL, which should cause a segfault, since that page
// is not in any segmentation.
int segfault(mmu *test_mmu) {
    char *test_string = "hello world";

    mmu_write_to_virtual_address(test_mmu, (addr32)0, TEST_PID, test_string,
                                 strlen(test_string) + 1);

    assert(test_mmu->num_page_faults == 0);
    assert(test_mmu->num_segmentation_faults == 1);
    assert(test_mmu->num_tlb_misses == 0);
    return 0;
}

/**
 * This test will read and write to a page in the DATA segment that is in the
 * processes segmentation but no pages have been allocated for it yet.
 *
 * You should page fault once when you access the page directory to get the page
 * table thus aksing the kernel for a frame to create this page table and update
 * your page directory.
 * You should page fault one more time when you access the page table to get the
 * frame thus asking the kernel for another frame to create this page and update
 * your page table.
 */
int page_fault_no_swap(mmu *test_mmu) {
    addr32 virtual_address =
        0x08052000; // virtual address of the first DATA page.
    char buff[PAGE_SIZE];
    char *test_string = "hello world";

    mmu_write_to_virtual_address(test_mmu, virtual_address, TEST_PID,
                                 test_string, strlen(test_string) + 1);
    mmu_read_from_virtual_address(test_mmu, virtual_address, TEST_PID, buff,
                                  strlen(test_string) + 1);
    assert(strcmp(buff, test_string) == 0);

    assert(test_mmu->num_page_faults == 2);
    assert(test_mmu->num_segmentation_faults == 0);
    assert(test_mmu->num_tlb_misses == 1);

    return 0;
}

/**
 * This test will read and write to a page that has been paged to disk
 *
 * You should page fault only when you access the page table to get the frame
 * thus asking the kernel for another frame to create this page, updating your
 * page table, and reading the page from disk.
 */
int page_fault_with_swap(mmu *test_mmu) {
    addr32 virtual_address = PAGE_SIZE;
    char buff[PAGE_SIZE];
    char *test_string = "hello world";

    // write the test string to the page.
    mmu_write_to_virtual_address(test_mmu, virtual_address, TEST_PID,
                                 test_string, strlen(test_string) + 1);
    // create a bunch of processes so that the page gets kicked out
    // each process creates 4 pages (page directory, page table, and 2 frames)
    for (int pid = 0; pid < NUM_PHYSICAL_PAGES / 4; pid++) {
        // offsetting by a large number so we do not use a previous pid
        mmu_add_process(test_mmu, pid + 9001);
    }

    // Now reading from a page that should be on disk.
    mmu_read_from_virtual_address(test_mmu, virtual_address, TEST_PID, buff,
                                  strlen(test_string) + 1);
    assert(strcmp(buff, test_string) == 0);

    // only 1 page fault since the eviction scheme does not page page tables or
    // directories to disk.
    assert(test_mmu->num_page_faults == 1);
    assert(test_mmu->num_segmentation_faults == 0);
    assert(test_mmu->num_tlb_misses == 1);

    return 0;
}

int main(int argc, char *argv[]) {
    int result = 13;
    // Please add more test cases
    if (argc == 1) {
        fprintf(stderr, "%s\n", "Needs test number");
        return result;
    }

    mmu *test_mmu = mmu_create();
    mmu_add_process(test_mmu, TEST_PID);

    int test_number = atoi(argv[1]);
    switch (test_number) {
    default:
        fprintf(stderr, "%s\n", "Invalid test number");
        break;
    case 1:
        result = read_1_virtual_address(test_mmu);
        break;
    case 2:
        result = read_and_write_1_virtual_address(test_mmu);
        break;
    case 3:
        result = interleave(test_mmu);
        break;
    case 4:
        result = spam1(test_mmu);
        break;
    case 5:
        result = multiprocess(test_mmu);
        break;
    case 6:
        result = same_vpns_diff_offset(test_mmu);
        break;
    case 7:
        result = segfault(test_mmu);
        break;
    case 8:
        result = page_fault_no_swap(test_mmu);
        break;
    case 9:
        result = page_fault_with_swap(test_mmu);
        break;
    }

    mmu_delete(test_mmu);
    return result;
}
