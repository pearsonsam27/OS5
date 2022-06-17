/**
 * This file contains implementations for methods in the Process class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "process/process.h"

using namespace std;

Process *Process::read_from_input(std::istream &in)
{
    //thank you piazza for seekg tellg
    vector<Page *> pages;
    in.seekg(0, in.end);
    size_t numBytes = in.tellg();
    in.seekg(0, in.beg);
    Page *nextPage = Page::read_from_input(in);
    while (nextPage)
    {
        pages.push_back(nextPage);
        nextPage = Page::read_from_input(in);
    }
    return new Process(numBytes, pages);
}

size_t Process::size() const
{
    // TODO
    return this->num_bytes;
}

bool Process::is_valid_page(size_t index) const
{
    // TODO
    if (index < pages.size() && !this->pages.empty())
    {
        return true;
    }
    return false;

    //(is_valid_offset(this->pages[index]));
    //return false;
}

size_t Process::get_rss() const
{
    //page table. getPresetPageCount()
    // TODO
    return page_table.get_present_page_count();
}

double Process::get_fault_percent() const
{
    // TODO
    if (this->memory_accesses > 0)
    {
        return 100 * (double(this->page_faults) / double(this->memory_accesses));
    }
    return 0;
}
