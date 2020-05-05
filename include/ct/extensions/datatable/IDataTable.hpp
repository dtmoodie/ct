#ifndef CT_EXT_IDATA_TABLE_HPP
#define CT_EXT_IDATA_TABLE_HPP
#include "DataTableArrayIterator.hpp"
#include "DataTableStorage.hpp"

#include <ct/reflect.hpp>

namespace ct
{
    namespace ext
    {

        template <class T>
        struct ElementView
        {
            T* begin() const { return m_begin; }
            T* end() const { return m_end; }

            T* m_begin;
            T* m_end;
        };

        template <class DTYPE, class BASES = typename Reflect<DTYPE>::BaseTypes>
        struct IDataTable;

        template <class DTYPE>
        struct IDataTable<DTYPE, VariadicTypedef<>>
        {
            virtual ~IDataTable() = default;
            template <class T, class U>
            T* begin(T U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value ||
                                  IsBase<Base<U>, Derived<DTYPE>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<T> begin(TArrayView<T> U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value ||
                                  IsBase<Base<U>, Derived<DTYPE>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p;
            }

            template <class T, class U>
            const T* begin(T U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value ||
                                  IsBase<Base<U>, Derived<DTYPE>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<const T> begin(TArrayView<T> U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value ||
                                  IsBase<Base<U>, Derived<DTYPE>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p;
            }

            template <class T, class U>
            T* end(T U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<T> end(TArrayView<T> U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p;
            }

            template <class T, class U>
            const T* end(T U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<const T> end(TArrayView<T> U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p;
            }

            template <class T, class U>
            ElementView<T> view(T U::*mem_ptr)
            {
                return {begin(mem_ptr), end(mem_ptr)};
            }

            template <class T, class U>
            ElementView<const T> view(T U::*mem_ptr) const
            {
                return {begin(mem_ptr), end(mem_ptr)};
            }

            virtual size_t size() const = 0;

            /**
             * @brief populateData populates a struct DTYPE with the data from element idx in the table
             * @param out
             * @param index of the
             */
            // virtual void populateData(DTYPE& out, size_t index) { populateDataRecurse(out, index,
            // Reflect<DTYPE>::end()); }
            virtual void populateData(DTYPE& out, size_t index) = 0;

          private:
            virtual DataTableArrayIterator<void> ptr(size_t offset, size_t idx) = 0;
            virtual DataTableArrayIterator<const void> ptr(size_t offset, size_t idx) const = 0;
        };

        template <class DTYPE, class BASE_TYPE>
        struct IDataTable<DTYPE, VariadicTypedef<BASE_TYPE>> : virtual IDataTable<BASE_TYPE>
        {
            using IDataTable<BASE_TYPE>::populateData;

            virtual ~IDataTable() = default;
            template <class T, class U>
            T* begin(T U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value ||
                                  IsBase<Base<U>, Derived<DTYPE>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<T> begin(TArrayView<T> U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value ||
                                  IsBase<Base<U>, Derived<DTYPE>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p;
            }

            template <class T, class U>
            const T* begin(T U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value ||
                                  IsBase<Base<U>, Derived<DTYPE>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<const T> begin(TArrayView<T> U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value ||
                                  IsBase<Base<U>, Derived<DTYPE>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), 0);
                return p;
            }

            template <class T, class U>
            T* end(T U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<T> end(TArrayView<T> U::*mem_ptr)
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p;
            }

            template <class T, class U>
            const T* end(T U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p.template ptr<T>();
            }

            template <class T, class U>
            DataTableArrayIterator<const T> end(TArrayView<T> U::*mem_ptr) const
            {
                static_assert(std::is_same<U, DTYPE>::value || IsBase<Base<DTYPE>, Derived<U>>::value,
                              "Mem ptr must derive from DTYPE");
                auto p = ptr(memberOffset(mem_ptr), size());
                return p;
            }

            template <class T, class U>
            ElementView<T> view(T U::*mem_ptr)
            {
                return {begin(mem_ptr), end(mem_ptr)};
            }

            template <class T, class U>
            ElementView<const T> view(T U::*mem_ptr) const
            {
                return {begin(mem_ptr), end(mem_ptr)};
            }

            virtual size_t size() const = 0;

            /**
             * @brief populateData populates a struct DTYPE with the data from element idx in the table
             * @param out
             * @param index of the
             */
            // virtual void populateData(DTYPE& out, size_t index) { populateDataRecurse(out, index,
            // Reflect<DTYPE>::end()); }
            virtual void populateData(DTYPE& out, size_t index) = 0;

          private:
            virtual DataTableArrayIterator<void> ptr(size_t offset, size_t idx) = 0;
            virtual DataTableArrayIterator<const void> ptr(size_t offset, size_t idx) const = 0;
        };

        struct IComponentProvider
        {
            virtual ~IComponentProvider() = default;
            virtual bool providesComponent(const std::type_info&) const = 0;
        };

        template <class T>
        struct TComponentProvider : IComponentProvider
        {
            bool providesComponent(const std::type_info& info) const override { return &info == &typeid(T); }
            // We only get views to the data since we are mutating the fields of an entity, we do NOT want to add
            // new entities with this interface since we would be breaking the associating with other components.
            // At the table level we can add or remove an entity, but not at the component level
            virtual void getComponentMutable(TArrayView<T>&) = 0;
            virtual void getComponent(TArrayView<const T>&) const = 0;
        };

        template <class DERIVED, class T>
        struct TComponentProviderImpl : IComponentProvider
        {
            bool providesComponent(const std::type_info&) const override { return false; }
        };

        template <class DERIVED, class T>
        struct TComponentProviderImpl<DERIVED, VariadicTypedef<T>> : TComponentProvider<T>
        {
            void getComponentMutable(TArrayView<T>&) {}

            void getComponent(TArrayView<const T>&) const {}
        };

        template <class DERIVED, class T, class... U>
        struct TComponentProviderImpl<DERIVED, VariadicTypedef<T, U...>>
            : TComponentProvider<T>, TComponentProviderImpl<DERIVED, VariadicTypedef<U...>>
        {
            using super = TComponentProviderImpl<DERIVED, VariadicTypedef<U...>>;
            // using DType = typename DERIVED::DType;
            // static constexpr const index_t I = indexOfMemberType<DType, T>();

            bool providesComponent(const std::type_info& type) const override
            {
                if (super::providesComponent(type))
                {
                    return true;
                }
                return TComponentProvider<T>::providesComponent(type);
            }

            void getComponentMutable(TArrayView<T>&)
            {
                // constexpr const auto ptr = ct::Reflect<DType>::getPtr(Indexer<I>{});
            }

            void getComponent(TArrayView<const T>&) const
            {
                // constexpr const auto ptr = ct::Reflect<DType>::getPtr(Indexer<I>{});
            }
        };

        template <class DTYPE, class DERIVED, class DERIVED_TYPES = typename Reflect<DTYPE>::BaseTypes>
        struct IDataTableImpl;

        template <class DTYPE, class DERIVED, class BASE_TYPE>
        struct IDataTableImpl<DTYPE, DERIVED, VariadicTypedef<BASE_TYPE>> : virtual IDataTable<DTYPE>,
                                                                            IDataTableImpl<BASE_TYPE, DERIVED>
        {
            using IDataTableImpl<BASE_TYPE, DERIVED>::populateData;
            void populateData(DTYPE& out, size_t index) override
            {
                static_cast<DERIVED*>(this)->populateDataRecurse(out, index, Reflect<DTYPE>::end());
            }
        };

        template <class DTYPE, class DERIVED>
        struct IDataTableImpl<DTYPE, DERIVED, VariadicTypedef<>> : virtual IDataTable<DTYPE>
        {
            void populateData(DTYPE& out, size_t index) override
            {
                static_cast<DERIVED*>(this)->populateDataRecurse(out, index, Reflect<DTYPE>::end());
            }
        };
    } // namespace ext
} // namespace ct
#endif // CT_EXT_IDATA_TABLE_HPP
