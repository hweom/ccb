#pragma once

namespace ccb { namespace binary
{
    struct BigEndian {};
    struct LittleEndian {};

    namespace details
    {
        template<typename Endianness, size_t N, typename Iter, typename Unit>
        struct ByteIteratorIncrementer
        {
        };

        template<size_t N, typename Iter, typename Unit>
        struct ByteIteratorIncrementer<BigEndian, N, Iter, Unit>
        {
            void operator () (Unit& value, Iter& pos, Iter end, bool firstIncrement)
            {
                value = 0;

                for (size_t i = 0; (i < N) && (pos != end); i++)
                {
                    if ((i > 0) || firstIncrement)
                    {
                        pos++;
                        firstIncrement = false;
                    }

                    if (pos == end)
                    {
                        return;
                    }

                    auto byte = static_cast<uint8_t>((*pos) & 0xff);
                    value = (value << 8) | byte;
                }
            }
        };

        template<size_t N, typename Iter, typename Unit>
        struct ByteIteratorIncrementer<LittleEndian, N, Iter, Unit>
        {
            void operator () (Unit& value, Iter& pos, Iter end, bool firstIncrement)
            {
                value = 0;

                for (size_t i = 0; (i < N) && (pos != end); i++)
                {
                    if ((i > 0) || firstIncrement)
                    {
                        pos++;
                        firstIncrement = false;
                    }

                    if (pos == end)
                    {
                        return;
                    }

                    auto byte = static_cast<uint8_t>((*pos) & 0xff);
                    value = value | (static_cast<Unit>(byte) << (8 * i));
                }
            }
        };
    }

    template<typename Endianness, size_t N, typename Iter, typename Unit = uint32_t>
    class ByteIterator
    {
    private:

        Iter pos;

        Iter end;

        Unit value = 0;

    public:

        ByteIterator(Iter pos, Iter end)
            : pos(pos)
            , end(end)
        {
            this->Next(false);
        }

    private:

        ByteIterator(Iter pos, Iter end, Unit value)
            : pos(pos)
            , end(end)
            , value(value)
        {
        }

    public:

        ByteIterator<Endianness, N, Iter, Unit> operator ++ (int)
        {
            ByteIterator<Endianness, N, Iter, Unit> result(this->pos, this->end, this->value);

            this->Next(true);

            return result;
        }

        ByteIterator<Endianness, N, Iter, Unit>& operator ++ ()
        {
            this->Next(true);

            return *this;
        }

        Unit operator * ()
        {
            return this->value;
        }

        inline friend bool operator != (const ByteIterator<Endianness, N, Iter, Unit>& i1, const ByteIterator<Endianness, N, Iter, Unit>& i2)
        {
            return i1.pos != i2.pos;
        }

        inline friend bool operator == (const ByteIterator<Endianness, N, Iter, Unit>& i1, const ByteIterator<Endianness, N, Iter, Unit>& i2)
        {
            return i1.pos == i2.pos;
        }

    private:

        void Next(bool firstIncrement)
        {
            details::ByteIteratorIncrementer<Endianness, N, Iter, Unit>()(this->value, this->pos, this->end, firstIncrement);
        }
    };

} }
