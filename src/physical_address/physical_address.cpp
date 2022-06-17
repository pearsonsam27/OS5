/**
 * This file contains implementations for methods in the PhysicalAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */
#include "physical_address/physical_address.h"

using namespace std;

string PhysicalAddress::to_string() const
{
    // TODO: implement me
    string frame = bitset<FRAME_BITS>(this->frame).to_string();
    string offset = bitset<OFFSET_BITS>(this->offset).to_string();
    string physAddress = frame;
    physAddress = physAddress.append(offset);
    return physAddress;
}

ostream &operator<<(ostream &out, const PhysicalAddress &address)
{
    out << address.to_string() << " [frame: " << address.frame << "; offset: " << address.offset << "]";
    // TODO: implement me
    return out;
}
