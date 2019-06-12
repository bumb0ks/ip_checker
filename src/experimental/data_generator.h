#ifndef DATA_GENERATOR
#define DATA_GENERATOR

/*
    It is example of object needed in threaded ioc manager
    as data provider.
*/

template <typename Container>
class data_generator
{
    typedef typename Container::iterator   iterator;

public:
    typedef typename Container::value_type value_type;

    data_generator(Container c) :
        begin(c.begin()),
        end(c.end())
    {
    }

    bool empty() const 
    {
        return begin == end;
    }

    value_type get()
    {
        return *(begin++);
    }

private:
    iterator begin;
    iterator end;
};

#endif