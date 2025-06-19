#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <iostream>
#include <cassert>

// ============================================================================
// СТРАТЕГИИ СОЗДАНИЯ ОБЪЕКТОВ (Creation Policies)
// ============================================================================

template<class T>
struct CreateUsingNew
{
    static T* Create()
    {
        return new T;
    }

    static void Destroy(T* p)
    {
        delete p;
    }
};

template<class T>
struct CreateUsingMalloc
{
    static T* Create()
    {
        void* p = std::malloc(sizeof(T));
        if (!p) return nullptr;
        return new(p) T;
    }

    static void Destroy(T* p)
    {
        if (p)
        {
            p->~T();
            std::free(p);
        }
    }
};

template<class T>
struct CreateStatic
{
    static T* Create()
    {
        static T obj;
        return &obj;
    }

    static void Destroy(T* p)
    {
        // Статический объект не нуждается в уничтожении
    }
};

// ============================================================================
// СТРАТЕГИИ ВРЕМЕНИ ЖИЗНИ (Lifetime Policies)
// ============================================================================

template<class T>
struct DefaultLifetime
{
    static void ScheduleDestruction(T*, void(*)())
    {
        // По умолчанию не планируем уничтожение
    }

    static void OnDeadReference()
    {
        throw std::logic_error("Dead Reference Detected");
    }
};

template<class T>
struct PhoenixSingleton
{
    static void ScheduleDestruction(T*, void(*)())
    {
        // Phoenix singleton может быть пересоздан после уничтожения
    }

    static void OnDeadReference()
    {
        // Позволяем воскрешение
    }
};

// ============================================================================
// СТРАТЕГИИ ПОТОКОБЕЗОПАСНОСТИ (Threading Policies)
// ============================================================================

// Однопоточная стратегия
struct SingleThreaded
{
    struct Lock
    {
        Lock() {}
        ~Lock() {}
    };

    using VolatileType = int;

    template<typename T>
    static T& AtomicAssign(T& lval, const T& rval)
    {
        return lval = rval;
    }
};

// Многопоточная стратегия с мьютексом
struct MultiThreaded
{
    struct Lock
    {
        static std::mutex& GetMutex()
        {
            static std::mutex mtx;
            return mtx;
        }

        Lock() : lock_(GetMutex()) {}

    private:
        std::lock_guard<std::mutex> lock_;
    };

    using VolatileType = volatile int;

    template<typename T>
    static T& AtomicAssign(T& lval, const T& rval)
    {
        // В реальной реализации здесь должна быть атомарная операция
        lval = rval;
        return lval;
    }
};

// Стратегия с двойной проверкой блокировки (Double-Checked Locking)
struct DoubleCheckedLocking
{
    struct Lock
    {
        static std::mutex& GetMutex()
        {
            static std::mutex mtx;
            return mtx;
        }

        Lock() : lock_(GetMutex()) {}

    private:
        std::lock_guard<std::mutex> lock_;
    };

    using VolatileType = std::atomic<int>;

    template<typename T>
    static T& AtomicAssign(T& lval, const T& rval)
    {
        lval = rval;
        return lval;
    }
};

// ============================================================================
// ОСНОВНОЙ КЛАСС SINGLETON
// ============================================================================

template<
        typename T,
        template<class> class CreationPolicy = CreateUsingNew,
        template<class> class LifetimePolicy = DefaultLifetime,
        class ThreadingModel = SingleThreaded
>
class SingletonHolder
{
private:
    using PtrInstanceType = typename ThreadingModel::VolatileType;
    static PtrInstanceType pInstance_;
    static bool destroyed_;

public:
    static T& Instance()
    {
        if (!pInstance_)
        {
            MakeInstance();
        }
        return *pInstance_;
    }

private:
    static void MakeInstance()
    {
        typename ThreadingModel::Lock guard;

        if (!pInstance_)
        {
            if (destroyed_)
            {
                LifetimePolicy<T>::OnDeadReference();
                destroyed_ = false;
            }

            pInstance_ = CreationPolicy<T>::Create();
            LifetimePolicy<T>::ScheduleDestruction(pInstance_, &DestroySingleton);
        }
    }

    static void DestroySingleton()
    {
        assert(!destroyed_);
        CreationPolicy<T>::Destroy(pInstance_);
        pInstance_ = nullptr;
        destroyed_ = true;
    }

    // Запрещаем копирование и присваивание
    SingletonHolder(const SingletonHolder&) = delete;
    SingletonHolder& operator=(const SingletonHolder&) = delete;
};

// Инициализация статических членов
template<typename T, template<class> class C, template<class> class L, class M>
typename SingletonHolder<T, C, L, M>::PtrInstanceType
        SingletonHolder<T, C, L, M>::pInstance_ = nullptr;

template<typename T, template<class> class C, template<class> class L, class M>
bool SingletonHolder<T, C, L, M>::destroyed_ = false;

// ============================================================================
// СПЕЦИАЛИЗАЦИЯ ДЛЯ DOUBLE-CHECKED LOCKING
// ============================================================================

template<typename T, template<class> class CreationPolicy, template<class> class LifetimePolicy>
class SingletonHolder<T, CreationPolicy, LifetimePolicy, DoubleCheckedLocking>
{
private:
    static std::atomic<T*> pInstance_;
    static bool destroyed_;

public:
    static T& Instance()
    {
        T* tmp = pInstance_.load(std::memory_order_acquire);
        if (tmp == nullptr)
        {
            typename DoubleCheckedLocking::Lock lock;
            tmp = pInstance_.load(std::memory_order_relaxed);
            if (tmp == nullptr)
            {
                if (destroyed_)
                {
                    LifetimePolicy<T>::OnDeadReference();
                    destroyed_ = false;
                }

                tmp = CreationPolicy<T>::Create();
                LifetimePolicy<T>::ScheduleDestruction(tmp, &DestroySingleton);
                pInstance_.store(tmp, std::memory_order_release);
            }
        }
        return *tmp;
    }

private:
    static void DestroySingleton()
    {
        assert(!destroyed_);
        T* tmp = pInstance_.load();
        CreationPolicy<T>::Destroy(tmp);
        pInstance_.store(nullptr);
        destroyed_ = true;
    }

    SingletonHolder(const SingletonHolder&) = delete;
    SingletonHolder& operator=(const SingletonHolder&) = delete;
};

// Инициализация для специализации
template<typename T, template<class> class C, template<class> class L>
std::atomic<T*> SingletonHolder<T, C, L, DoubleCheckedLocking>::pInstance_{nullptr};

template<typename T, template<class> class C, template<class> class L>
bool SingletonHolder<T, C, L, DoubleCheckedLocking>::destroyed_ = false;

// ============================================================================
// ПРИМЕР ИСПОЛЬЗОВАНИЯ
// ============================================================================

// Тестовый класс
class Logger
{
private:
    static int instanceCount_;

public:
    Logger()
    {
        instanceCount_++;
        std::cout << "Logger instance #" << instanceCount_ << " created\n";
    }

    ~Logger()
    {
        std::cout << "Logger instance destroyed\n";
    }

    void Log(const std::string& message)
    {
        std::cout << "[LOG]: " << message << std::endl;
    }

    static int GetInstanceCount() { return instanceCount_; }
};

int Logger::instanceCount_ = 0;

// Определение различных типов Singleton'ов
using StandardSingleton = SingletonHolder<Logger>;
using ThreadSafeSingleton = SingletonHolder<Logger, CreateUsingNew, DefaultLifetime, MultiThreaded>;
using DCLSingleton = SingletonHolder<Logger, CreateUsingNew, DefaultLifetime, DoubleCheckedLocking>;
using StaticSingleton = SingletonHolder<Logger, CreateStatic, DefaultLifetime, SingleThreaded>;

// ============================================================================
// ДЕМОНСТРАЦИЯ РАБОТЫ
// ============================================================================

void TestSingletonStrategies()
{
    std::cout << "=== Тестирование Singleton с различными стратегиями ===\n\n";

    // Тест стандартного singleton
    std::cout << "1. Стандартный Singleton:\n";
    StandardSingleton::Instance().Log("Первое сообщение от стандартного singleton");
    StandardSingleton::Instance().Log("Второе сообщение от того же singleton");
    std::cout << "Количество созданных экземпляров: " << Logger::GetInstanceCount() << "\n\n";

    // Тест потокобезопасного singleton
    std::cout << "2. Потокобезопасный Singleton:\n";
    ThreadSafeSingleton::Instance().Log("Сообщение от потокобезопасного singleton");

    // Тест в нескольких потоках
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back([i]() {
            ThreadSafeSingleton::Instance().Log("Сообщение из потока " + std::to_string(i));
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    std::cout << "Общее количество созданных экземпляров: " << Logger::GetInstanceCount() << "\n\n";

    // Тест DCL singleton
    std::cout << "3. Double-Checked Locking Singleton:\n";
    DCLSingleton::Instance().Log("Сообщение от DCL singleton");
    std::cout << "Общее количество созданных экземпляров: " << Logger::GetInstanceCount() << "\n\n";
}

int main()
{
    TestSingletonStrategies();
    return 0;
}