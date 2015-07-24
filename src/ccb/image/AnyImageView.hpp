#pragma once

#include <functional>

#include <ccb/image/AnyImageIterator.hpp>

namespace ccb { namespace image
{
    template<typename DstPixelType, typename Enable = void>
    class AnyImageView
    {
    public:

        using ValueType = typename DstPixelType::ValueType;

        using PixelType = DstPixelType;

    private:

        void* data;

        size_t width;

        size_t height;

        size_t stride;

        std::function<details::AnyImageState (void*)> start;

        std::function<void(details::AnyImageState&, size_t)> advance;

        std::function<ValueType (const details::AnyImageState&)> read;

        std::function<void (details::AnyImageState&, ValueType)> write;

    public:

        AnyImageView(
            void* data,
            size_t width,
            size_t height,
            size_t stride,
            const std::function<details::AnyImageState (void*)>& start,
            const std::function<void(details::AnyImageState&, size_t)>& advance,
            const std::function<ValueType (const details::AnyImageState& state)>& read,
            const std::function<void (details::AnyImageState&, ValueType)>& write)
            : data(data)
            , width(width)
            , height(height)
            , stride(stride)
            , start(start)
            , advance(advance)
            , read(read)
            , write(write)
        {
        }

    public:

        void* GetData()
        {
            return this->data;
        }

        size_t GetWidth() const
        {
            return this->width;
        }

        size_t GetHeight() const
        {
            return this->height;
        }

        AnyImageIterator<PixelType> BeginRow(size_t row)
        {
            auto state = this->start(reinterpret_cast<uint8_t*>(this->data) + this->stride * row);

            return AnyImageIterator<PixelType>(state, this->advance, this->read, this->write);
        }

        AnyImageIterator<PixelType> EndRow(size_t row)
        {
            auto state = this->start(reinterpret_cast<uint8_t*>(this->data) + this->stride * row);

            this->advance(state, this->width);

            return AnyImageIterator<PixelType>(state, this->advance, this->read, this->write);
        }
    };

    template<typename DstPixelType>
    class AnyImageView<DstPixelType, typename std::enable_if<std::is_const<DstPixelType>::value, void>::type>
    {
    public:

        using ValueType = typename DstPixelType::ValueType;

        using PixelType = DstPixelType;

    private:

        const void* data;

        size_t width;

        size_t height;

        size_t stride;

        std::function<details::AnyImageState (const void*)> start;

        std::function<void(details::AnyImageState&, size_t)> advance;

        std::function<ValueType (const details::AnyImageState&)> read;

    public:

        AnyImageView(
            const void* data,
            size_t width,
            size_t height,
            size_t stride,
            const std::function<details::AnyImageState (const void*)>& start,
            const std::function<void(details::AnyImageState&, size_t)>& advance,
            const std::function<ValueType (const details::AnyImageState& state)>& read,
            const std::function<void (details::AnyImageState&, ValueType)>& write)
            : data(data)
            , width(width)
            , height(height)
            , stride(stride)
            , start(start)
            , advance(advance)
            , read(read)
        {
        }

    public:

        const void* GetData() const
        {
            return this->data;
        }

        size_t GetWidth() const
        {
            return this->width;
        }

        size_t GetHeight() const
        {
            return this->height;
        }

        AnyImageIterator<PixelType> BeginRow(size_t row)
        {
            auto state = this->start(reinterpret_cast<const uint8_t*>(this->data) + this->stride * row);

            return AnyImageIterator<PixelType>(state, this->advance, this->read);
        }

        AnyImageIterator<PixelType> EndRow(size_t row)
        {
            auto state = this->start(reinterpret_cast<const uint8_t*>(this->data) + this->stride * row);

            this->advance(state, this->width);

            return AnyImageIterator<PixelType>(state, this->advance, this->read);
        }
    };
} }
