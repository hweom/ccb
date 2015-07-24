#pragma once

#include <functional>

#include <ccb/image/ImageFormat.hpp>

namespace ccb { namespace image
{
    namespace details
    {
        struct AnyImageState
        {
            size_t data[4];
        };
    }

    template<typename PixelType, typename Enable = void>
    class AnyImageIterator
    {
    public:

        using ValueType = typename PixelType::ValueType;

    private:

        details::AnyImageState state;

        std::function<void(details::AnyImageState&, size_t)> advance;

        std::function<ValueType (const details::AnyImageState&)> read;

        std::function<void (details::AnyImageState&, ValueType)> write;

        ValueType value;

        bool valueValid = false;

    public:

        AnyImageIterator(
            const details::AnyImageState& state,
            const std::function<void(details::AnyImageState&, size_t)>& advance,
            const std::function<ValueType (const details::AnyImageState&)>& read,
            const std::function<void (details::AnyImageState&, ValueType)>& write)
            : state(state)
            , advance(advance)
            , read(read)
            , write(write)
        {
        }

        ~AnyImageIterator()
        {
            this->Write();
        }

    public:

        ValueType& operator * ()
        {
            this->Read();

            return this->value;
        }

        AnyImageIterator<PixelType> operator ++ (int)
        {
            this->Write();

            AnyImageIterator<PixelType> result(this->state, this->advance, this->read, this->write);

            this->advance(this->state, 1);

            this->valueValid = false;

            return result;
        }

        AnyImageIterator<PixelType>& operator ++ ()
        {
            this->advance(this->state, 1);

            this->valueValid = false;

            return *this;
        }

        friend inline bool operator == (AnyImageIterator<PixelType> i1, AnyImageIterator<PixelType> i2)
        {
            return (i1.state == i2.state) && (i1.advance == i2.advance) && (i1.read == i2.read);
        }

        friend inline bool operator != (AnyImageIterator<PixelType> i1, AnyImageIterator<PixelType> i2)
        {
            return (i1.state != i2.state) || (i1.advance != i2.advance) || (i1.read != i2.read);
        }

    private:

        void Read()
        {
            if (!this->valueValid)
            {
                this->value = this->read(this->state);

                this->valueValid = true;
            }
        }

        void Write()
        {
            if (this->valueValid)
            {
                this->write(this->state, this->value);
            }
        }
    };

    template<typename PixelType>
    class AnyImageIterator<PixelType, typename std::enable_if<std::is_const<PixelType>::value, void>::type>
    {
    public:

        using ValueType = typename PixelType::ValueType;

    private:

        details::AnyImageState state;

        std::function<void(details::AnyImageState&, size_t)> advance;

        std::function<ValueType (const details::AnyImageState&)> read;

    public:

        AnyImageIterator(
            const details::AnyImageState& state,
            const std::function<void(details::AnyImageState&, size_t)>& advance,
            const std::function<ValueType (const details::AnyImageState&)>& read)
            : state(state)
            , advance(advance)
            , read(read)
        {
        }

    public:

        ValueType operator * ()
        {
            return this->read(this->state);
        }

        AnyImageIterator<PixelType> operator ++ (int)
        {
            AnyImageIterator<PixelType> result(this->state, this->advance, this->read);

            this->advance(this->state, 1);

            return result;
        }

        AnyImageIterator<PixelType>& operator ++ ()
        {
            this->advance(this->state, 1);

            return *this;
        }

        friend inline bool operator == (AnyImageIterator<PixelType> i1, AnyImageIterator<PixelType> i2)
        {
            return (i1.state == i2.state) && (i1.advance == i2.advance) && (i1.read == i2.read);
        }

        friend inline bool operator != (AnyImageIterator<PixelType> i1, AnyImageIterator<PixelType> i2)
        {
            return (i1.state != i2.state) || (i1.advance != i2.advance) || (i1.read != i2.read);
        }
    };
} }
