#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton
{
public:
    static T *instance()
    {
        static T inst;
        return &inst;
    }
};

#endif // SINGLETON_H
