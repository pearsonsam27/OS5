/**
 * This file contains implementations for methods in the VirtualAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "virtual_address/virtual_address.h"

using namespace std;

VirtualAddress VirtualAddress::from_string(int process_id, string address)
{
    // TODO: implement me
    int addressBinary = stoi(address, 0, 2);
    int offset = addressBinary & OFFSET_BITMASK;
    int page = addressBinary & PAGE_BITMASK;
    page = page >> OFFSET_BITS;

    //first 10, last 6
    //const size_t page = bitset<32>(address.substr(0, 9)).to_ulong();
    // const size_t offset = bitset<32>(address.substr(10, 15)).to_ulong();
    return VirtualAddress(process_id, page, offset);
}

string VirtualAddress::to_string() const
{
    // TODO: implement me

    string page = bitset<PAGE_BITS>(this->page).to_string();
    string offset = bitset<OFFSET_BITS>(this->offset).to_string();
    string vAddress = page;
    vAddress = vAddress.append(offset);
    return vAddress;
    //return "";
}

ostream &operator<<(ostream &out, const VirtualAddress &address)
{
    // TODO: implement me
    out << "PID " << address.process_id << " @ " << address.to_string() << " [page: " << address.page << "; offset: " << address.offset << "]";
    return out;
}
