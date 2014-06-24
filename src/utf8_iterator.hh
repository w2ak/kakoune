#ifndef utf8_iterator_hh_INCLUDED
#define utf8_iterator_hh_INCLUDED

#include "utf8.hh"

namespace Kakoune
{

namespace utf8
{

// adapter for an iterator on bytes which permits to iterate
// on unicode codepoints instead.
template<typename Iterator,
         typename InvalidPolicy = InvalidBytePolicy::Assert>
class iterator
{
public:
    iterator() = default;
    iterator(Iterator it) : m_it(std::move(it)) {}

    iterator& operator++()
    {
        m_it = utf8::next(m_it);
        invalidate_value();
        return *this;
    }

    iterator operator++(int)
    {
        iterator save = *this;
        ++*this;
        return save;
    }

    void advance(CharCount count, const iterator& end)
    {
        while (*this != end and count-- > 0)
            ++*this;
    }

    iterator& operator--()
    {
        m_it = utf8::previous(m_it);
        invalidate_value();
        return *this;
    }

    iterator operator--(int)
    {
        iterator save = *this;
        --*this;
        return save;
    }

    iterator operator+(CharCount count) const
    {
        if (count < 0)
            return operator-(-count);

        iterator res = *this;
        while (count--)
            ++res;
        return res;
    }

    iterator operator-(CharCount count) const
    {
        if (count < 0)
            return operator+(-count);

        iterator res = *this;
        while (count--)
            --res;
        return res;
    }

    bool operator==(const iterator& other) { return m_it == other.m_it; }
    bool operator!=(const iterator& other) { return m_it != other.m_it; }

    bool operator< (const iterator& other) const
    {
        return m_it < other.m_it;
    }

    bool operator<= (const iterator& other) const
    {
        return m_it <= other.m_it;
    }

    bool operator> (const iterator& other) const
    {
        return m_it > other.m_it;
    }

    bool operator>= (const iterator& other) const
    {
        return m_it >= other.m_it;
    }

    CharCount operator-(iterator other) const
    {
        //kak_assert(other < *this);
        check_invariant();
        other.check_invariant();
        CharCount dist = 0;
        while (other.m_it < m_it)
        {
            ++dist;
            ++other;
        }
        return dist;
    }

    Codepoint operator*() const
    {
        return get_value();
    }

    const Iterator& base() const { return m_it; }
    Iterator& base() { return m_it; }

protected:
    void check_invariant() const
    {
        // always point to a character first byte;
        // kak_assert(is_character_start(it));
    }

private:
    void invalidate_value() { m_value = -1; }
    Codepoint get_value() const
    {
        if (m_value == -1)
            m_value = utf8::codepoint<InvalidPolicy>(m_it);
        return m_value;
    }

    Iterator m_it;
    mutable Codepoint m_value = -1;
};

template<typename InvalidPolicy = InvalidBytePolicy::Assert, typename Iterator>
iterator<Iterator, InvalidPolicy> make_iterator(Iterator it)
{
    return iterator<Iterator, InvalidPolicy>{std::move(it)};
}

}

}
#endif // utf8_iterator_hh_INCLUDED
