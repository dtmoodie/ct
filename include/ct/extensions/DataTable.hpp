#ifndef CT_EXTENSIONS_DATA_TABLE_HPP
#define CT_EXTENSIONS_DATA_TABLE_HPP
#include "../reflect.hpp"
#include <vector>
#include <tuple>
namespace ct
{
    namespace ext
    {
        template<class T, class U>
        size_t pointerValue(T U::* ptr)
        {
            size_t output = 0;
            memcpy(&output, &ptr, sizeof(ptr));
            return output;
        }

        template<class U, class T>
        struct DataTableBase
        {
        };

        template<class U, class ... Args>
        struct DataTableBase<U, VariadicTypedef<Args...>>
        {
            DataTableBase()
            {
                fillOffsets(Reflect<U>::end());
            }

            void push_back(const U& data)
            {
                push(data, ct::Reflect<U>::end());
            }

            template<class T>
            T& access(T U::* mem_ptr, const size_t idx)
            {
                void* p = ptr(pointerValue(mem_ptr), idx, Reflect<U>::end());
                return *static_cast<T*>(p);
            }

            template<class T>
            const T& access(T U::* mem_ptr, const size_t idx) const
            {
                const void* p = ptr(pointerValue(mem_ptr), idx, Reflect<U>::end());
                return *static_cast<const T*>(p);
            }

            template<class T>
            T* begin(T U::* mem_ptr)
            {
                void* p = ptr(pointerValue(mem_ptr), 0, Reflect<U>::end());
                return static_cast<T*>(p);
            }

            template<class T>
            const T* begin(T U::* mem_ptr) const
            {
                const void* p = ptr(pointerValue(mem_ptr), 0, Reflect<U>::end());
                return static_cast<const T*>(p);
            }

            template<class T>
            T* end(T U::* mem_ptr)
            {
                void* p = ptr(pointerValue(mem_ptr), std::get<0>(m_data).size(), Reflect<U>::end());
                return static_cast<T*>(p);
            }

            template<class T>
            const T* end(T U::* mem_ptr) const
            {
                const void* p = ptr(pointerValue(mem_ptr), std::get<0>(m_data).size(), Reflect<U>::end());
                return static_cast<const T*>(p);
            }

            U access(const size_t idx)
            {
                U out;
                populateData(out, Reflect<U>::end(), idx);
                return out;
            }

            void reserve(const size_t size)
            {
                reserveImpl(size, Reflect<U>::end());
            }

        private:

            void reserveImpl(const size_t size, const ct::Indexer<0>)
            {
                std::get<0>(m_data).reserve(size);
            }

            template<index_t I>
            void reserveImpl(const size_t size, const ct::Indexer<I> idx)
            {
                std::get<I>(m_data).reserve(size);
                reserveImpl(size, --idx);
            }


            void populateData(U& data, const ct::Indexer<0> idx, const size_t row)
            {
                const auto accessor = Reflect<U>::getAccessor(idx);
                accessor.set(data, std::get<0>(m_data)[row]);
            }

            template<index_t I>
            void populateData(U& data, const ct::Indexer<I> idx, const size_t row)
            {
                const auto accessor = Reflect<U>::getAccessor(idx);
                accessor.set(data, std::get<I>(m_data)[row]);
                populateData(data, --idx, row);
            }

            void push(const U& data, const ct::Indexer<0> idx)
            {
                const auto accessor = Reflect<U>::getAccessor(idx);
                std::get<0>(m_data).push_back(accessor.get(data));
            }

            template<index_t I>
            void push(const U& data, const ct::Indexer<I> idx)
            {
                const auto accessor = Reflect<U>::getAccessor(idx);
                std::get<static_cast<size_t>(I)>(m_data).push_back(accessor.get(data));
                push(data, --idx);
            }

            void* ptr(const size_t offset, const size_t index, const ct::Indexer<0>)
            {
                if(offset == m_field_offsets[0])
                {
                    return &(std::get<0>(m_data)[index]);
                }
                return nullptr;
            }

            template<index_t I>
            void* ptr(const size_t offset, const size_t index, const ct::Indexer<I> field_index)
            {
                if(offset == m_field_offsets[I])
                {
                    return &(std::get<I>(m_data)[index]);
                }
                return ptr(offset, index, --field_index);
            }

            const void* ptr(const size_t offset, const size_t index, ct::Indexer<0>) const
            {
                if(offset == m_field_offsets[0])
                {
                    return &(std::get<0>(m_data)[index]);
                }
                return nullptr;
            }

            template<index_t I>
            const void* ptr(const size_t offset, const size_t index, ct::Indexer<I> field_index) const
            {
                if(offset == m_field_offsets[I])
                {
                    return &(std::get<I>(m_data)[index]);
                }
                return ptr(offset, index, --field_index);
            }



            std::tuple<std::vector<Args>...> m_data;

            void fillOffsets(const Indexer<0> idx)
            {
                const auto accessor = Reflect<U>::getAccessor(idx);
                const auto field_offset =  pointerValue(accessor.m_getter);
                m_field_offsets[0] = field_offset;
            }

            template<index_t I>
            void fillOffsets(const Indexer<I> idx)
            {
                const auto accessor = Reflect<U>::getAccessor(idx);
                const auto field_offset =  pointerValue(accessor.m_getter);
                m_field_offsets[I] = field_offset;
                fillOffsets(--idx);
            }

            std::array<size_t, sizeof...(Args)> m_field_offsets;

        };

        template<class T>
        struct DataTable: public DataTableBase<T, typename ct::GlobMemberObjects<T>::types>
        {

        };

    }
}

#endif // CT_EXTENSIONS_DATA_TABLE_HPP
