#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

class ThreadPool
{
public:
    /**
     * @brief Construct a new Thread Pool object
     * 
     * @param t_num 
     * @param j_num 
     */
    explicit ThreadPool(int t_num, int j_num)
    : m_thread_num(t_num), m_que_max_size(j_num)
    , m_resource(0, 0)
    , m_res_empty(0, m_que_max_size)
    , m_mtx(0,1)
    , m_stop(false) 
    {
        for (int i = 0; i < m_thread_num; ++i)
        {
            std::thread([this]()
            {
                while(!m_stop)
                {
                    m_resource.Wait();
                    m_mtx.Wait();

                    auto todo_task = m_job_queue.front();
                    m_job_queue.pop();

                    m_res_empty.post();
                    m_mtx.post();

                    todo_task();
                }
            }).detach();
        }
    }

    /**
     * @brief Destroy the Thread Pool object
     * 
     */
    ~ThreadPool()
    {
        m_stop = true;
    }

    /**
     * @brief 对外接口：向线程池中添加任务
     * 
     * @tparam F 
     * @param task 
     */
    template <class F>
    void Append(F &&task)
    {
        m_res_empty.Wait();
        m_mtx.Wait();

        m_job_queue.emplace(std::forward<F>(task));

        m_resource.post();
        m_mtx.post();
    }
    
    ThreadPool(const ThreadPool &) = delete;    /* 删除我们不需要的拷贝函数 */
    ThreadPool &operator=(const ThreadPool &) = delete;

private:
    int m_thread_num;
    int m_que_max_size;
    Locker m_resource;
    Locker m_res_empty;
    Locker m_mtx;
    bool m_stop;
    std::queue<std::function<void()>> m_job_queue;
    std::vector<std::thread> m_work_threads;
};
#endif
