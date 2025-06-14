#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QtTypes>
#include <iterator>

template<typename T, qsizetype capacity>
class CircularBuffer
{
private:
    qsizetype m_realSize = 0;
    qsizetype m_index = 0;
    T *const m_data = new T[capacity];

public:
    class const_iterator
    {
    private:
        const T *m_data;
        qsizetype m_index;

    public:
        explicit const_iterator(const T *data, qsizetype index = 0)
            : m_data(data)
            , m_index(index)
        {}

        const_iterator &operator++()
        {
            m_index++;
            return *this;
        }

        bool operator==(const_iterator other) const { return m_index == other.m_index; }
        bool operator!=(const_iterator other) const { return m_index != other.m_index; }

        const T &operator*() const { return m_data[m_index]; }

        using difference_type = T;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;
        using iterator_category = std::forward_iterator_tag;
    };

    CircularBuffer() {}
    ~CircularBuffer() { delete[] m_data; }

    void append(const T &value)
    {
        m_data[m_index++] = value;
        m_index %= capacity;
        if (m_realSize < capacity) {
            m_realSize++;
        }
    }

    int size() { return m_realSize; }

    const_iterator begin() const { return const_iterator(m_data, 0); }
    const_iterator end() const { return const_iterator(m_data, m_realSize); }
};

#endif // CIRCULARBUFFER_H
