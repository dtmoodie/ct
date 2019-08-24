#ifndef CT_REFLECT_CEREALIZE_HPP
#define CT_REFLECT_CEREALIZE_HPP

namespace ct
{
    template <class AR, class T>
    void loadStruct(AR& ar, T& obj);

    template <class AR, class T>
    void saveStruct(AR& ar, const T& obj);
} // namespace ct

/////////////////////////////////////////////////////////////
/// implementation
/////////////////////////////////////////////////////////////

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
// TODO make specialization for text archives that uses a size tag
#include <ct/VariadicTypedef.hpp>
#include <ct/types/std_array.hpp>

#include <ct/reflect.hpp>
#include <ct/reflect_traits.hpp>
#include <ct/static_asserts.hpp>
#include <ct/type_traits.hpp>
#include <ct/types/TArrayView.hpp>
namespace ct
{
    namespace cereal
    {

        template <class T>
        constexpr bool isArrayContainer()
        {
            return (indexOfField<T>("data") != -1) && (indexOfField<T>("size") != -1);
        }

        template <class T>
        constexpr bool isNDArrayContainer()
        {
            return (indexOfField<T>("data") != -1) && (indexOfField<T>("shape") != -1);
        }

        template <class T, class PTR, class ENABLE = void>
        struct FieldCerealizer
        {
            template <class AR>
            static void load(AR&, T&, const PTR)
            {
            }
            template <class AR>
            static void save(AR&, const T&, const PTR)
            {
            }
        };

        template <class T, class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA>
        struct FieldCerealizer<T, MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>, void>
        {
            using Ptr_t = MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA>;

            template <class AR>
            static void load(AR& ar, T& obj, const Ptr_t ptr)
            {
                using Set_t = typename SetType<Ptr_t>::type;
                using Ref_t = typename ReferenceType<Set_t>::Type;
                ar(::cereal::make_nvp(ptr.m_name.toString(), static_cast<Ref_t>(ptr.set(obj))));
            }

            template <class AR>
            static void save(AR& ar, const T& obj, const Ptr_t ptr)
            {
                ar(::cereal::make_nvp(ptr.m_name.toString(), ptr.get(obj)));
            }
        };

        template <class T, class GET_PTR, class SET_PTR, Flag_t FLAGS, class METADATA>
        struct FieldCerealizer<T, MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>, void>
        {
            using Ptr_t = MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA>;
            static constexpr const auto WRITABLE = FLAGS & ct::Flags::WRITABLE;
            static constexpr const auto CONST = FLAGS & ct::Flags::COMPILE_TIME_CONSTANT;

            template <class AR>
            static void load(AR& ar, T& obj, const Ptr_t ptr)
            {
                loadImpl(ar, obj, ptr, std::integral_constant<bool, WRITABLE != 0>{});
            }

            template <class AR>
            static void save(AR& ar, const T& obj, const Ptr_t ptr)
            {
                saveImpl(ar, obj, ptr, std::integral_constant<bool, !CONST>{});
            }

          private:
            template <class AR>
            static void loadImpl(AR& ar, T& obj, const Ptr_t ptr, std::integral_constant<bool, true>)
            {
                using Set_t = typename SetType<Ptr_t>::type;
                using Ref_t = typename ReferenceType<Set_t>::Type;
                ar(::cereal::make_nvp(ptr.m_name.toString(), static_cast<Ref_t>(ptr.set(obj))));
            }

            template <class AR>
            static void loadImpl(AR&, T&, const Ptr_t, std::integral_constant<bool, false>)
            {
            }

            template <class AR>
            static void saveImpl(AR& ar, const T& obj, const Ptr_t ptr, std::integral_constant<bool, true>)
            {
                ar(::cereal::make_nvp(ptr.m_name.toString(), ptr.get(obj)));
            }

            template <Flag_t FG, class AR>
            static void saveImpl(AR&, const T&, const Ptr_t, std::integral_constant<bool, false>)
            {
            }
        };

        template <class T>
        struct IsTensor
        {
            constexpr static const bool value = indexOfField<T>("data") != -1 && indexOfField<T>("shape") != -1;
        };

        template <class T>
        class StructCerealizer
        {
            template <class AR>
            static void load(AR& ar, T& obj, const Indexer<0> idx)
            {
                auto ptr = ct::Reflect<T>::getPtr(idx);
                FieldCerealizer<T, decltype(ptr)>::load(ar, obj, ptr);
            }

            template <class AR, index_t I>
            static void load(AR& ar, T& obj, const Indexer<I> idx)
            {
                load(ar, obj, --idx);
                auto ptr = ct::Reflect<T>::getPtr(idx);
                FieldCerealizer<T, decltype(ptr)>::load(ar, obj, ptr);
            }

            template <class AR>
            static void save(AR& ar, const T& obj, const Indexer<0> idx)
            {
                auto ptr = ct::Reflect<T>::getPtr(idx);
                FieldCerealizer<T, decltype(ptr)>::save(ar, obj, ptr);
            }

            template <class AR, index_t I>
            static void save(AR& ar, const T& obj, const Indexer<I> idx)
            {
                save(ar, obj, --idx);
                auto ptr = ct::Reflect<T>::getPtr(idx);
                FieldCerealizer<T, decltype(ptr)>::save(ar, obj, ptr);
            }

          public:
            template <class AR>
            static void load(AR& ar, T& obj)
            {
                load(ar, obj, ct::Reflect<T>::end());
            }

            template <class AR>
            static void save(AR& ar, const T& obj)
            {
                save(ar, obj, ct::Reflect<T>::end());
            }
        };

        template <class T>
        class TensorCerealizer
        {
            constexpr static const auto DATA_INDEX = indexOfField<T>("data");
            constexpr static const auto SHAPE_INDEX = indexOfField<T>("shape");
            constexpr static const auto SIZE_INDEX = indexOfField<T>("size");
            constexpr static const bool DYNAMIC_SHAPE = IsWritable<T, SHAPE_INDEX>::value;

            template <class AR, index_t I>
            static auto load(AR&, T&, const Indexer<I>)
                -> EnableIf<I == DATA_INDEX || I == SHAPE_INDEX || I == SIZE_INDEX>
            {
            }

            template <class AR, index_t I>
            static auto save(AR&, const T&, const Indexer<I>)
                -> EnableIf<I == DATA_INDEX || I == SHAPE_INDEX || I == SIZE_INDEX>
            {
            }

            template <class AR, index_t I>
            static auto load(AR& ar, T& obj, const Indexer<I> idx)
                -> EnableIf<I != DATA_INDEX && I != SHAPE_INDEX && I != SIZE_INDEX>
            {
                auto ptr = ct::Reflect<T>::getPtr(idx);
                FieldCerealizer<T, decltype(ptr)>::load(ar, obj, ptr);
            }

            template <class AR, index_t I>
            static auto save(AR& ar, const T& obj, const Indexer<I> idx)
                -> EnableIf<I != DATA_INDEX && I != SHAPE_INDEX && I != SIZE_INDEX>
            {
                auto ptr = ct::Reflect<T>::getPtr(idx);
                FieldCerealizer<T, decltype(ptr)>::save(ar, obj, ptr);
            }

            template <class AR>
            static void loadItr(AR& ar, T& obj, const Indexer<0> idx)
            {
                load(ar, obj, idx);
            }

            template <class AR, index_t I>
            static void loadItr(AR& ar, T& obj, const Indexer<I> idx)
            {
                loadItr(ar, obj, --idx);
                load(ar, obj, idx);
            }

            template <class AR>
            static void saveItr(AR& ar, const T& obj, const Indexer<0> idx)
            {
                save(ar, obj, idx);
            }

            template <class AR, index_t I>
            static void saveItr(AR& ar, const T& obj, const Indexer<I> idx)
            {
                saveItr(ar, obj, --idx);
                save(ar, obj, idx);
            }

			template<class SHAPE, class PTR>
			static void reshapeImpl(const SHAPE& shape, PTR ptr, T& obj, std::integral_constant<bool, true>)
			{
                ptr.set(obj, shape);
			}
			
			template <class SHAPE, class PTR>
            static void reshapeImpl(const SHAPE& shape, PTR ptr, T& obj, std::integral_constant<bool, false>)
            {
            }

            template <class SHAPE, class GET_PTR, class SET_PTR, Flag_t FLAGS, class METADATA>
            static void reshape(const SHAPE& shape,
                                MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA> ptr,
                                T& obj)
            {
                constexpr const bool MODIFYABLE_SHAPE = FLAGS & Flags::WRITABLE;
                reshapeImpl(shape, ptr, obj, std::integral_constant<bool, MODIFYABLE_SHAPE>{});
                
            }

          public:
            template <class AR>
            static void load(AR& ar, T& obj)
            {
                auto data_ptr = Reflect<T>::getPtr(Indexer<DATA_INDEX>());
                auto shape_ptr = Reflect<T>::getPtr(Indexer<SHAPE_INDEX>());
                auto size_ptr = Reflect<T>::getPtr(Indexer<indexOfField<T>("size")>());
                auto shape = shape_ptr.get(obj);
                if (!(getFlags<decltype(shape_ptr)>() & ct::value(Flags::COMPILE_TIME_CONSTANT)))
                {
                    ar(::cereal::make_nvp("shape", shape));
                    reshape(shape, shape_ptr, obj);
                }
                const auto size = size_ptr.get(obj);
                if (size > 0)
                {
                    ar(::cereal::make_nvp("data", makeArrayView(data_ptr.set(obj), size)));
                }
                loadItr(ar, obj, ct::Reflect<T>::end());
            }

            template <class AR>
            static void save(AR& ar, const T& obj)
            {
                auto data_ptr = Reflect<T>::getPtr(Indexer<DATA_INDEX>());
                auto shape_ptr = Reflect<T>::getPtr(Indexer<SHAPE_INDEX>());
                auto size_ptr = Reflect<T>::getPtr(Indexer<indexOfField<T>("size")>());
                const auto size = size_ptr.get(obj);
                FieldCerealizer<T, decltype(shape_ptr)>::save(ar, obj, shape_ptr);
                if (size > 0)
                {
                    ar(::cereal::make_nvp("data", makeArrayView(data_ptr.get(obj), size)));
                }
                saveItr(ar, obj, Reflect<T>::end());
            }
        };

        template <class T>
        struct SingleValueCerealizer
        {
            template <class AR>
            static void load(AR& ar, T& obj)
            {
                auto data_ptr = Reflect<T>::getPtr(Indexer<0>());
                ar(data_ptr.set(obj));
            }

            template <class AR>
            static void save(AR& ar, const T& obj)
            {
                // TODO this should be the first serializable field not necessarily the first field
                auto data_ptr = Reflect<T>::getPtr(Indexer<0>());
                ar(data_ptr.get(obj));
            }
        };

        template <class T, index_t I>
        struct CountSerializableFieldsHelper
        {
            constexpr static const uint32_t value = ShouldSerialize<T, I>::value ? 1 : 0;
            constexpr static const uint32_t count = CountSerializableFieldsHelper<T, I - 1>::count + value;
        };

        template <class T>
        struct CountSerializableFieldsHelper<T, 0>
        {
            constexpr static const uint32_t value = ShouldSerialize<T, 0>::value ? 1 : 0;
            constexpr static const uint32_t count = value;
        };
        template <class T, class ENABLE = void>
        struct CountSerializableFields;

        template <class T>
        struct CountSerializableFields<T, EnableIfReflected<T>>
        {
            constexpr static const uint32_t value = CountSerializableFieldsHelper<T, Reflect<T>::NUM_FIELDS - 1>::count;
        };

        template <class T>
        struct CountSerializableFields<T, DisableIfReflected<T>>
        {
            constexpr static const uint32_t value = 0;
        };

        template <class T, index_t PRIORITY = 10, class ENABLE = void>
        struct CerealizerSelector : public CerealizerSelector<T, PRIORITY - 1, void>
        {
        };

        // lowest priority select the generic StructCerealizer
        template <class T>
        struct CerealizerSelector<T, 0, void> : public StructCerealizer<T>
        {
        };

// Higher priority if IsTensor<T> is true, use the TensorCerealizer
#ifndef _MSC_VER
        // automatic tensor detection and serialization does not work with msvc since IsTensor fails to be constexpr
        template <class T>
        struct CerealizerSelector<T, 1, EnableIf<IsTensor<T>::value>> : public TensorCerealizer<T>
        {
        };
#endif
        /*template <class T>
        struct CerealizerSelector<T, 1, EnableIf<CountSerializableFields<T>::value == 1>>
            : public SingleValueCerealizer<T>
        {
        };*/

        template <class T, class ENABLE = void>
        struct CerealMinimalRepresentation;

        template <class T>
        struct CerealMinimalRepresentation<T, EnableIfReflected<T>>
        {
            using type = typename ct::FieldGetType<T, 0>::type;
        };

        template <class T>
        struct CerealMinimalRepresentation<T, DisableIfReflected<T>>
        {
            using type = int;
        };
    }

    template <class AR, class T>
    EnableIf<!VariadicTypedef<void, const void>::contains<T>()> save(AR& ar, const TArrayView<T>& view)
    {
        ::cereal::size_type size = view.size();
        ar(::cereal::make_size_tag(size));
        for (size_t i = 0; i < view.size(); ++i)
        {
            ar(view[i]);
        }
    }

    template <class AR, class T>
    EnableIf<!VariadicTypedef<void, const void>::contains<T>()> load(AR& ar, TArrayView<T>& view)
    {
        ::cereal::size_type size = 0;
        ar(::cereal::make_size_tag(size));
        if (size != view.size())
        {
            throw std::runtime_error("Deserializing an unexpected size");
        }
        for (size_t i = 0; i < view.size(); ++i)
        {
            ar(view[i]);
        }
    }

    template <class AR>
    void load(AR& ar, TArrayView<void>& view)
    {
        ar(::cereal::binary_data(view.data(), view.size()));
    }

    template <class AR>
    void save(AR& ar, const TArrayView<void>& view)
    {
        ar(::cereal::binary_data(view.data(), view.size()));
    }

    template <class AR>
    void save(AR& ar, const TArrayView<const void>& view)
    {
        ar(::cereal::binary_data(view.data(), view.size()));
    }

    template <class AR>
    void load(AR&, TArrayView<const void>&)
    {
        throw std::runtime_error("Attempting to load a const array view");
    }
}

namespace cereal
{

    template <class AR, class T>
    auto save(AR& ar, const T& data) -> ct::EnableIf<ct::IsReflected<T>::value>
    {
        ct::cereal::CerealizerSelector<T>::save(ar, data);
    }

    template <class AR, class T>
    auto load(AR& ar, T& data) -> ct::EnableIf<ct::IsReflected<T>::value>
    {
        ct::cereal::CerealizerSelector<T>::load(ar, data);
    }
} // namespace cereal
#endif // CT_REFLECT_CEREALIZE_HPP
