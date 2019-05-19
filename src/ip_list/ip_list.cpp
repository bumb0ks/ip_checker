#include "ip_list.h"

#include <utility>
#include <iostream>

IPList::IPList(const string& _first_ip,
               const string& _last_ip) : 
    first_ip(move(_first_ip)),
    last_ip(move(_last_ip)),
    m_end(last_ip)
{
    m_end.operator++();
}


ip_address IPList::begin() const
{
    return ip_address(first_ip);
}

ip_address IPList::end() const
{
    return m_end;
}
