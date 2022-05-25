#ifndef CT_INTEROP_BOOST_PYTHON_REFLECTED_CONVERTER_HPP
#define CT_INTEROP_BOOST_PYTHON_REFLECTED_CONVERTER_HPP
#include "PythonConverter.hpp"

#include <ct/reflect.hpp>
#include <ct/reflect_traits.hpp>
#include <ct/type_traits.hpp>

namespace ct
{
    namespace bp
    {
        template <class T, index_t PRIORITY = 10, class ENABLE = void>
        struct ReflectedConverter : ReflectedConverter<T, PRIORITY - 1, void>
        {
        };

        template <class T>
        struct PythonConverter<T, 3, EnableIfReflected<T>> : public ReflectedConverter<T>
        {

        }; // PythonConverter<T, 2, ct::EnableIfReflected<T>>

        template <class T>
        struct PythonConverter<T, 4, EnableIfIsEnum<T>>
        {
            static std::string repr(const T& obj);

            static void registerToPython(const char* name);

            static bool convertFromPython(const boost::python::object&, T&);

            static boost::python::object convertToPython(const T& result);
        }; // PythonConverter<T, 2, ct::EnableIfReflected<T>>

        template <class T>
        struct ReflectedConverter<T, 1, void>
        {
            static std::string repr(const T& obj);

            static void registerToPython(const char* name);

            static bool convertFromPython(const boost::python::object&, T&);

            static boost::python::object convertToPython(const T& result);
        };

        /////////////////////////////////////////////////////////////////////////////
        ///      IMPLEMENTATION
        /////////////////////////////////////////////////////////////////////////////

        template <class T, int N>
        struct FunctionSignatureBuilder
        {
            using VariadicTypedef_t =
                typename ct::Append<T, typename FunctionSignatureBuilder<T, N - 1>::VariadicTypedef_t>::type;
        };

        template <class T>
        struct FunctionSignatureBuilder<T, 0>
        {
            using VariadicTypedef_t = ct::VariadicTypedef<T>;
        };

        template <class T>
        struct FunctionSignatureBuilder<T, -1>
        {
            using VariadicTypedef_t = ct::VariadicTypedef<>;
        };

        namespace detail
        {

            template <class T, int I>
            using SetValue_t = typename ct::FieldSetType<T, I>::type;

            template <class T, int I>
            using GetValue_t = typename ct::FieldSetType<T, I>::type;

            template <class T, ct::index_t I>
            static auto extractReflectedImpl(T& obj, const boost::python::object& bpobj, ct::Indexer<I> idx)
                -> ct::EnableIfIsWritable<T, I, bool>
            {
                auto ptr = bpobj.ptr();
                auto name = ct::getName<I, T>().toString();
                boost::python::object python_member;
                boost::python::dict dict(bpobj);
                boost::python::object key(name);
                if (PyObject_HasAttrString(ptr, ct::getName<I, T>().cStr()))
                {
                    python_member = bpobj.attr(ct::getName<I, T>().cStr());
                }
                else if (dict && dict.has_key(key))
                {
                    python_member = dict[key];
                }
                else
                {
                    if (!dict && I < boost::python::len(bpobj))
                    {
                        python_member = bpobj[I];
                    }
                }
                if (python_member)
                {
                    using type = typename std::decay<SetValue_t<T, I>>::type;
                    type val;
                    if (convertFromPython(python_member, val))
                    {
                        auto ptr = ct::Reflect<T>::getPtr(idx);
                        ptr.set(obj, std::move(val));
                        return true;
                    }
                }
                return false;
            }

            template <class T, ct::index_t I>
            static auto extractReflectedImpl(T&, const boost::python::object&, ct::Indexer<I>)
                -> ct::DisableIfIsWritable<T, I, bool>
            {
                return false;
            }

            // These two function overloads recurse over all objects
            template <class T>
            int extractReflected(T& obj, const boost::python::object& pyobj, ct::Indexer<0> idx)
            {
                return extractReflectedImpl(obj, pyobj, idx) ? 1 : 0;
            }

            template <class T, ct::index_t I>
            int extractReflected(T& obj, const boost::python::object& bpobj, ct::Indexer<I> idx)
            {
                return (extractReflectedImpl(obj, bpobj, idx) ? 1 : 0) + extractReflected(obj, bpobj, --idx);
            }

            template <class T, ct::index_t I>
            auto initDataMembersImpl(T& data, ct::Indexer<I> idx, boost::python::object& value)
                -> ct::EnableIfIsWritable<T, I>
            {
                using type_t = typename std::decay<SetValue_t<T, I>>::type;
                type_t extracted_val;
                if (convertFromPython(value, extracted_val))
                {
                    auto accessor = ct::Reflect<T>::getPtr(idx);
                    accessor.set(data, std::move(extracted_val));
                }
            }

            template <class T, ct::index_t I>
            auto initDataMembersImpl(T&, ct::Indexer<I>, boost::python::object&) -> ct::DisableIfIsWritable<T, I>
            {
                // Well this is bad, we souldn't ever get to this instantiation
            }

            template <class T, ct::index_t I>
            void initDataMembers(T& data, ct::Indexer<I> idx, boost::python::object& value)
            {
                initDataMembersImpl(data, idx, value);
            }

            template <class T, ct::index_t I, class... Args>
            void initDataMembers(T& data, ct::Indexer<I> idx, boost::python::object& value, Args... args)
            {
                initDataMembersImpl(data, idx, value);
                initDataMembers(data, ++idx, args...);
            }

            template <class T, ct::index_t I, class... Args>
            auto initDataMembers(T& data, ct::Indexer<I> idx, boost::python::object& value, Args... args)
                -> ct::DisableIfIsWritable<T, I>
            {
                initDataMembers(data, ++idx, value, args...);
            }

            template <class... Args>
            struct CreateDataObject
            {
            };

            template <class T, ct::index_t I>
            auto populateKwargsImpl(std::array<boost::python::detail::keyword, ct::GlobWritable<T>::num>& kwargs,
                                    const ct::Indexer<I>,
                                    const size_t kwarg_index) -> EnableIfIsWritable<T, I, bool>
            {
                kwargs[kwarg_index] = (boost::python::arg(ct::getName<I, T>().cStr()) = boost::python::object());
                return true;
            }

            template <class T, ct::index_t I>
            auto populateKwargsImpl(std::array<boost::python::detail::keyword, ct::GlobWritable<T>::num>&,
                                    const ct::Indexer<I>,
                                    const size_t) -> DisableIfIsWritable<T, I, bool>
            {
                return false;
            }

            template <class T>
            void populateKwargs(std::array<boost::python::detail::keyword, ct::GlobWritable<T>::num>& kwargs,
                                const ct::Indexer<0> idx,
                                const size_t kwarg_index)
            {
                populateKwargsImpl<T>(kwargs, idx, kwarg_index);
            }

            template <class T, ct::index_t I>
            auto populateKwargs(std::array<boost::python::detail::keyword, ct::GlobWritable<T>::num>& kwargs,
                                const ct::Indexer<I> member_index,
                                const size_t kwarg_index) -> DisableIf<I == 0>
            {
                const auto increment = populateKwargsImpl<T>(kwargs, member_index, kwarg_index);
                populateKwargs<T>(kwargs, --member_index, kwarg_index - (increment ? 1 : 0));
            }

            template <class T>
            std::array<boost::python::detail::keyword, ct::GlobWritable<T>::num> makeKeywordArgs()
            {
                std::array<boost::python::detail::keyword, ct::GlobWritable<T>::num> kwargs;
                populateKwargs<T>(kwargs, ct::Reflect<T>::end(), ct::GlobWritable<T>::num - 1);
                return kwargs;
            }

            template <class T, class... Args>
            struct CreateDataObject<T, ct::VariadicTypedef<Args...>>
            {
                static const int size = ct::GlobWritable<T>::num;
                static T* create(Args... args)
                {
                    auto obj = new T();
                    initDataMembers(*obj, ct::Indexer<0>{}, args...);
                    return obj;
                }

                static boost::python::detail::keyword_range range()
                {
                    using kwptr = boost::python::detail::keyword const*;
                    static std::array<boost::python::detail::keyword, size> s_keywords = makeKeywordArgs<T>();
                    return std::make_pair<kwptr, kwptr>(&s_keywords[0], &s_keywords[0] + size);
                }
            };

            template <class T, class BP>
            EnableIf<ct::IsDefaultConstructible<T>::value && ct::GlobWritable<T>::num >= 1> addInit(BP& bpobj)
            {
                // ct::PrintVariadicTypedef<typename ct::GlobWritable<T>::types>{};
                using Signature_t = typename FunctionSignatureBuilder<boost::python::object,
                                                                      ct::GlobWritable<T>::num - 1>::VariadicTypedef_t;
                using Creator_t = CreateDataObject<T, Signature_t>;
                bpobj.def("__init__",
                          boost::python::make_constructor(
                              &Creator_t::create, boost::python::default_call_policies(), Creator_t()));
            }

            template <class T, class BP>
            EnableIf<!ct::IsDefaultConstructible<T>::value || ct::GlobWritable<T>::num == 0> addInit(BP&)
            {
            }

            template <int I, class T>
            boost::python::object pythonGet(const T& obj)
            {
                auto accessor = ct::Reflect<T>::getPtr(ct::Indexer<I>{});
                return convertToPython(accessor.get(obj));
            }

            template <int I, class T>
            bool pythonSet(T& obj, const boost::python::object& val)
            {
                using type = typename std::decay<SetValue_t<T, I>>::type;
                type extracted_value;
                if (convertFromPython(val, extracted_value))
                {
                    auto accessor = ct::Reflect<T>::getPtr(ct::Indexer<I>{});
                    accessor.set(obj, std::move(extracted_value));
                    return true;
                }
                return false;
            }

            // TODO this needs to work for properties and needs to ignore functions
            template <class T, index_t I>
            auto getItem(const T& obj, Indexer<I>) -> EnableIfIsReadable<T, I, boost::python::object>
            {
                return pythonGet<I>(obj);
            }

            template <class T, index_t I>
            auto getItem(const T&, Indexer<I>) -> DisableIfIsReadable<T, I, boost::python::object>
            {
                return {};
            }

            template <class T>
            boost::python::object getItemRecurse(const T& obj, const index_t i, Indexer<0> idx)
            {
                if (i == 0)
                {
                    return getItem(obj, idx);
                }
                return {};
            }

            template <class T, index_t I>
            boost::python::object getItemRecurse(const T& obj, const index_t i, Indexer<I> idx)
            {
                if (i == I)
                {
                    return getItem(obj, idx);
                }
                return getItemRecurse(obj, i, --idx);
            }

            template <class T>
            boost::python::object getItem(const T& obj, const index_t i)
            {
                if (size_t(i) >= GlobWritable<T>::num)
                {
                    std::stringstream ss;
                    ss << i << " out of valid range of 0 -> " << GlobWritable<T>::num;
                    throw std::out_of_range(std::move(ss).str());
                }
                return getItemRecurse(obj, i, ct::Reflect<T>::end());
            }

            template <class T, index_t I>
            auto setItem(T& obj, Indexer<I>, const boost::python::object& val) -> EnableIfIsWritable<T, I, bool>
            {
                return pythonSet<I>(obj, val);
            }

            template <class T, index_t I>
            auto setItem(T&, Indexer<I>, const boost::python::object&) -> DisableIfIsWritable<T, I, bool>
            {
                return false;
            }

            template <class T>
            bool setItemRecurse(T& obj, const index_t i, Indexer<0> idx, const boost::python::object& val)
            {
                if (i == 0)
                {
                    return setItem(obj, idx, val);
                }
                return false;
            }

            template <class T, index_t I>
            bool setItemRecurse(T& obj, const index_t i, Indexer<I> idx, const boost::python::object& val)
            {
                if (i == I)
                {
                    return setItem(obj, idx, val);
                }
                return setItemRecurse(obj, i, --idx, val);
            }

            template <class T>
            bool setItem(T& obj, const index_t i, const boost::python::object& val)
            {
                if (size_t(i) >= GlobWritable<T>::num)
                {
                    std::stringstream ss;
                    ss << i << " out of valid range of 0 -> " << GlobWritable<T>::num;
                    throw std::out_of_range(std::move(ss).str());
                }
                return setItemRecurse(obj, i, ct::Reflect<T>::end(), val);
            }

            template <class T>
            size_t len(const T&)
            {
                return GlobWritable<T>::num;
            }

            template <int I, class T>
            boost::python::object pythonInvoke(const T& obj)
            {
                auto accessor = ct::Reflect<T>::getPtr(ct::Indexer<I>());
                return boost::python::object(accessor.template invoke<0>(obj));
            }

            template <int I, class T>
            boost::python::object pythonInvokeMutable(const T& obj)
            {
                auto accessor = ct::Reflect<T>::getPtr(ct::Indexer<I>());
                return accessor.invoke<0>(obj);
            }

            template <class T, class BP, class PROPERTY, index_t I>
            auto addPropertyImpl(BP& bpobj, PROPERTY, Indexer<I>)
                -> EnableIf<!(flags<PROPERTY>() & Flags::WRITABLE) && (flags<PROPERTY>() & Flags::READABLE)>
            {
                bpobj.add_property(ct::getName<I, T>().cStr(), &pythonGet<I, T>);
            }

            template <class T, class BP, class PROPERTY, index_t I>
            auto addPropertyImpl(BP& bpobj, PROPERTY, Indexer<I>)
                -> EnableIf<(flags<PROPERTY>() & Flags::WRITABLE) && (flags<PROPERTY>() & Flags::READABLE)>
            {
                bpobj.add_property(ct::getName<I, T>().cStr(), &pythonGet<I, T>, &pythonSet<I, T>);
            }

            template <class T, class BP, class PROPERTY>
            void addPropertyImpl(BP&, PROPERTY, index_t)
            {
            }

            template <class T, class BP, index_t I, class U, Flag_t FLAGS, class MDATA, class... PTRS>
            void addMemberFunctionImpl(BP& bpobj, MemberFunctionPointers<U, FLAGS, MDATA, PTRS...> ptrs, Indexer<I>)
            {
                auto ptr = ptrs.template getPtr<I>();
                bpobj.def(ptrs.getName().cStr(), ptr.m_ptr);
            }

            template <class T, class BP, class PTRS>
            void addMemberFunctionRecurse(BP& bpobj, PTRS ptrs, Indexer<0> idx)
            {
                addMemberFunctionImpl<T>(bpobj, ptrs, idx);
            }

            template <class T, class BP, index_t I, class PTRS>
            void addMemberFunctionRecurse(BP& bpobj, PTRS ptrs, Indexer<I> idx)
            {
                addMemberFunctionImpl<T>(bpobj, ptrs, idx);
                addMemberFunctionRecurse<T>(bpobj, ptrs, --idx);
            }

            template <class T, class BP, class U, Flag_t FLAGS, class MDATA, class... PTRS>
            auto addPropertyImpl(BP& bp, MemberFunctionPointers<U, FLAGS, MDATA, PTRS...> ptr, index_t)
            {
                addMemberFunctionRecurse<T>(bp, ptr, Indexer<sizeof...(PTRS) - 1>());
            }

            template <class T, index_t I, class BP>
            void addProperty(BP& bp, Indexer<I> idx)
            {
                auto ptr = ct::Reflect<T>::getPtr(idx);
                addPropertyImpl<T>(bp, ptr, idx);
            }

            template <class T, class BP>
            void addPropertyRecurse(BP& bpobj, const ct::Indexer<0> idx)
            {
                addProperty<T>(bpobj, idx);
            }

            template <class T, ct::index_t I, class BP>
            void addPropertyRecurse(BP& bpobj, const ct::Indexer<I> idx)
            {
                addProperty<T>(bpobj, idx);
                addPropertyRecurse<T>(bpobj, --idx);
            }

            template <class T, class BP>
            void addProperties(BP& bpobj)
            {
                addPropertyRecurse<T>(bpobj, ct::Reflect<T>::end());
            }

            template <class PTR>
            auto registerPropertyReturn(PTR) -> EnableIf<(flags<PTR>() & Flags::READABLE) == 0>
            {
            }

            template <class PTR>
            auto registerPropertyReturn(PTR) -> EnableIf<(flags<PTR>() & Flags::READABLE) != 0>
            {
                using type = typename GetType<PTR>::type;
                registerToPython<decay_t<type>>();
            }

            template <class T, Flag_t FLAGS, class MDATA, class... PTR>
            auto registerMemberFunctionReturn(MemberFunctionPointers<T, FLAGS, MDATA, PTR...> ptrs, Indexer<0>)
            {
                auto ptr = ptrs.template getPtr<0>();
                using type = typename decltype(ptr)::Ret_t;
                registerToPython<decay_t<type>>();
            }

            template <index_t I, class T, Flag_t FLAGS, class MDATA, class... PTR>
            auto registerMemberFunctionReturn(MemberFunctionPointers<T, FLAGS, MDATA, PTR...> ptrs, Indexer<I>)
            {
                auto ptr = ptrs.template getPtr<I>();
                using type = typename decltype(ptr)::Ret_t;
                registerToPython<decay_t<type>>();
            }

            template <class T, Flag_t FLAGS, class MDATA, class... PTR>
            void registerPropertyReturn(MemberFunctionPointers<T, FLAGS, MDATA, PTR...> ptr)
            {
                registerMemberFunctionReturn(ptr, Indexer<sizeof...(PTR) - 1>());
            }

            template <class T, index_t I>
            void registerProperty(Indexer<I> idx)
            {
                auto type = Reflect<T>::getPtr(idx);
                registerPropertyReturn(type);
            }

            template <class T>
            void ensureRegisteredRecurse(Indexer<0> idx)
            {
                registerProperty<T>(idx);
            }

            template <class T, index_t I>
            void ensureRegisteredRecurse(Indexer<I> idx)
            {
                registerProperty<T>(idx);
                ensureRegisteredRecurse<T>(--idx);
            }

            template <class T>
            void ensureRegistered()
            {
                ensureRegisteredRecurse<T>(Reflect<T>::end());
            }

            template <class T>
            std::string repr(const T& obj)
            {
                std::stringstream ss;
                printStruct(ss, obj);
                return std::move(ss).str();
            }

            template <class T, class BPOBJ>
            void registerEnumToPython(BPOBJ& obj, ct::Indexer<0> idx)
            {
                auto ptr = ct::Reflect<T>::getPtr(idx);
                obj.value(ptr.getName().cStr(), T(ptr.value().getValue()));
            }

            template <class T, class BPOBJ, index_t I>
            void registerEnumToPython(BPOBJ& obj, ct::Indexer<I> idx)
            {
                auto ptr = ct::Reflect<T>::getPtr(idx);
                obj.value(ptr.getName().cStr(), T(ptr.value().getValue()));
                registerEnumToPython<T>(obj, --idx);
            }

        } // namespace detail

        template <class T>
        std::string PythonConverter<T, 4, EnableIfIsEnum<T>>::repr(const T& obj)
        {
            std::stringstream ss;
            ss << obj;
            return ss.str();
        }

        template <class T>
        void PythonConverter<T, 4, EnableIfIsEnum<T>>::registerToPython(const char* name)
        {
            boost::python::enum_<T> bpobj(name);
            auto end = ct::Reflect<T>::end();
            detail::registerEnumToPython<T>(bpobj, end);
        }

        template <class T>
        bool PythonConverter<T, 4, EnableIfIsEnum<T>>::convertFromPython(const boost::python::object&, T& val)
        {
            (void)val;
            return false;
        }

        template <class T>
        boost::python::object PythonConverter<T, 4, EnableIfIsEnum<T>>::convertToPython(const T& val)
        {
            boost::python::object ret(val);
            return ret;
        }

        template <class T>
        void ReflectedConverter<T, 1, void>::registerToPython(const char* name)
        {
            boost::python::class_<T> bpobj(name);
            detail::addProperties<T>(bpobj);
            detail::addInit<T>(bpobj);
            bpobj.def("__getitem__", &detail::getItem<T>);
            bpobj.def("__setitem__", &detail::setItem<T>);
            bpobj.def("__repr__", &detail::repr<T>);
            bpobj.def("__len__", &detail::len<T>);

            // Now we ensure any returned datatype is also registered
            detail::ensureRegistered<T>();
        }

        template <class T>
        bool ReflectedConverter<T, 1, void>::convertFromPython(const boost::python::object& pyobj, T& val)
        {
            boost::python::extract<T> extractor(pyobj);
            if (extractor.check())
            {
                val = extractor();
                return true;
            }
            else
            {
                if (detail::extractReflected(val, pyobj, ct::Reflect<T>::end()) > 0)
                {
                    return true;
                }
            }
            return false;
        }

        template <class T>
        boost::python::object ReflectedConverter<T, 1, void>::convertToPython(const T& result)
        {
            return boost::python::object(result);
        }
    } // namespace bp
} // namespace ct

namespace boost
{
    namespace python
    {
        namespace detail
        {
            template <class... T>
            struct is_keywords<ct::bp::detail::CreateDataObject<T...>>
            {
                static const bool value = true;
            };
        } // namespace detail
    }     // namespace python
} // namespace boost

#endif // CT_INTEROP_BOOST_PYTHON_REFLECTED_CONVERTER_HPP
