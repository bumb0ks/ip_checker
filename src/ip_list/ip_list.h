#ifndef IP_LIST
#define IP_LIST

#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>
#include <iostream>


using namespace std;

class IPList
{
    struct ip_address
    {
        typedef vector<uint8_t> octet_range;
    public:
        ip_address(const string& _ip) : ip(_ip)
        {}

        string operator*() 
        { 
            return ip; 
        }

        void operator++() 
        { 
            octet_range range(move(decompose()));

            ++range[3];

            if (range[3] == 0)
            {
                ++range[2];
                if (range[2] == 0)
                {
                    ++range[1];
                    if (range[1] == 0)
                    {
                        ++range[0];
                    }
                }
            }

            compose(move(range));
        }

        bool operator!=(ip_address rhs) 
        { 
            return ip != rhs.to_string(); 
        }

        string to_string() const
        {
            return ip;
        }

    private:
        octet_range decompose() const
        {
            octet_range octets;
            octets.reserve(4);

            vector<string> results;
            boost::split(results, ip, [](char c){return c == '.';});
            
            for (string s : results)
            {
                octets.push_back(static_cast<uint8_t>(stoi(s)));
            }

            return octets;
        }

        void compose(octet_range&& octets)
        {
            string result;
            for(uint8_t o : octets)
            {
                result += std::to_string(o) + ".";
            }
            result.pop_back();
            ip = move(result);
        }

    private:
        string ip;
    };

public:

    IPList(const string& _first_ip,
           const string& _last_ip);

    ip_address begin() const;

    ip_address end() const;

private:
    const string first_ip;
    const string last_ip;
    ip_address m_end;
};

#endif