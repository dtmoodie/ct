#pragma once

namespace ct
{
    template <int N>
    struct _counter_
    {
        _counter_<N - 1> operator--() const { return _counter_<N - 1>(); }

        _counter_<N + 1> operator++() const { return _counter_<N + 1>(); }
    };
}
