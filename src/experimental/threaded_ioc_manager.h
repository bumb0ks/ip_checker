#ifndef THREADED_IOC_MANAGER
#define THREADED_IOC_MANAGER

#include <vector>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

template< typename worker_t, typename data_generator_t >
class threaded_io_service_manager
{
public:

    template<typename... RetType>
    threaded_io_service_manager(
        size_t thread_count,
        size_t worker_count
    ) :
    m_thread_count(thread_count),
    m_worker_count(worker_count)
    {
        for(size_t tmp = 0; tmp < m_worker_count; ++tmp)
            workers.emplace_back(ioc, boost::bind(&threaded_io_service_manager::write_result, this,
                boost::placeholders::_1));
    }

    void set_data_generator(boost::shared_ptr<data_generator_t> data_gen)
    {
        m_data_generator = data_gen;
    }

    void run()
    {
        if (m_data_generator == nullptr) return;

        for (size_t i = 0; i < m_worker_count; ++i)
            push_data_and_process();

        for (size_t i = 0; i < m_thread_count; ++i)
            threads.create_thread(boost::bind(&threaded_io_service_manager::run_io_context, this));

        threads.join_all();
    }

private:
    void push_data_and_process()
    {
        ioc.dispatch([&](){
            boost::lock_guard grd(m_reuse_mutex);
            
            if (m_data_generator->empty())
                return;

            for (worker_t& worker : workers)
            {
                if (!worker.is_busy())
                {
                    worker.process(m_data_generator->get());
                    break;
                }
            }
        });
    }

    void write_result(const std::string& ans)
    {
        if (ans.back() == '1')
        {
            std::stringstream a;
            a << ans << std::endl;
            std::cout << a.str();
        }
        push_data_and_process();
    }

    void run_io_context() 
    {
        ioc.run(); 
    }

private:
    boost::asio::io_context ioc;

    std::vector<worker_t> workers;
    boost::thread_group threads;

    boost::shared_ptr<data_generator_t> m_data_generator;

    size_t m_thread_count;
    size_t m_worker_count;

    boost::mutex m_reuse_mutex;
};

#endif