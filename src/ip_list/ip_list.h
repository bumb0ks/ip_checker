#ifndef IP_LIST
#define IP_LIST

#include <boost/algorithm/string.hpp>

#include <string>
#include <array>
#include <iostream>


using namespace std;

struct ip_address
{
    typedef array<uint8_t, 4> octet_range;
public:
    ip_address(const string& _ip) : ip(_ip)
    {
        decomposed_ip = move(decompose());
    }

    ip_address(const ip_address&) = default;

    string operator*() 
    { 
        return ip; 
    }

    ip_address& operator++() 
    { 
        ++decomposed_ip[3];

        if (decomposed_ip[3] == 0)
        {
            ++decomposed_ip[2];
            if (decomposed_ip[2] == 0)
            {
                ++decomposed_ip[1];
                if (decomposed_ip[1] == 0)
                {
                    ++decomposed_ip[0];
                }
            }
        }

        compose();
        return *this;
    }

    ip_address operator++(int) 
    { 
        ip_address tmp(*this);
        ++(*this);
        return tmp;
    }

    bool operator!=(ip_address rhs) 
    { 
        return ip != rhs.to_string(); 
    }

    bool operator==(const ip_address& rhs) const
    { 
        return ip == rhs.to_string(); 
    }

    string to_string() const
    {
        return ip;
    }

private:
    octet_range decompose() const
    {
        octet_range octets;

        vector<string> results;
        boost::split(results, ip, [](char c){return c == '.';});
        
        size_t i = 0;
        for (string s : results)
        {
            octets[i] = static_cast<uint8_t>(stoi(s));
            ++i;
        }

        return octets;
    }

    void compose()
    {
        string result;
        for(uint8_t o : decomposed_ip)
        {
            result += std::to_string(o) + ".";
        }
        result.pop_back();
        ip = move(result);
    }

private:
    string ip;
    octet_range decomposed_ip;
};

class IPList
{
public:

    typedef string     value_type;
    typedef ip_address iterator;

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