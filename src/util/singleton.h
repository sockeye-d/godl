#ifndef SINGLETON_H
#define SINGLETON_H

template<typename T>
class Singleton
{
public:
    static T *instance()
    {
        static T *inst = new T;
        return inst;
    }
};

#endif // SINGLETON_H
