#ifndef CT_REFLECT_VISITOR_HPP
#define CT_REFLECT_VISITOR_HPP
#include "../reflect.hpp"

namespace ct
{
    struct DefaultVisitorParams
    {
        constexpr static const bool VISIT_OBJECTS = true;
        constexpr static const bool VISIT_PROPERTIES = true;

        constexpr static const bool SERIALIZER = true;

        constexpr static const char* PREFIX = "";
        constexpr static const char* DELIMINATOR = ".";

        constexpr static const bool ACCUMULATE_PATH = true;

        template <class T>
        constexpr static bool visitMemberFunctions(T*)
        {
            return true;
        }
        template <class T>
        constexpr static bool visitMemberFunction(T*)
        {
            return true;
        }
        template <class T>
        constexpr static bool visitStaticFunctions(T*)
        {
            return true;
        }
        template <class T>
        constexpr static bool visitStaticFunction(T*)
        {
            return true;
        }
        template <class T>
        constexpr static bool visitMemberObject(T*)
        {
            return true;
        }
        template <class T>
        constexpr static bool visitProperty(T*)
        {
            return true;
        }
    };

    template <class DERIVED, class OPTS = DefaultVisitorParams>
    struct VisitorBase
    {
        using VisitorParams = OPTS;

        constexpr static const bool VISIT_OBJECTS = OPTS::VISIT_OBJECTS;
        constexpr static const bool VISIT_PROPERTIES = OPTS::VISIT_PROPERTIES;
        constexpr static const bool SERIALIZER = OPTS::SERIALIZER;

        template <class T, class... ARGS>
        EnableIfReflected<T> visit(T& obj, const std::string& name, ARGS&&... args)
        {
            recurseFields(obj, name, Reflect<T>::end(), std::forward<ARGS>(args)...);
        }

        template <class T, class... ARGS>
        DisableIfReflected<T> visit(T&, const std::string&, ARGS&&...)
        {
        }

        template <class T, class... ARGS>
        EnableIfReflected<T> visit(const T& obj, const std::string& name, ARGS&&... args)
        {
            recurseFields(obj, name, Reflect<T>::end(), std::forward<ARGS>(args)...);
        }

        template <class T, class... ARGS>
        DisableIfReflected<T> visit(const T&, const std::string&, ARGS&&...)
        {
        }

        template <index_t I, class DTYPE, class... ARGS>
        EnableIfReflected<typename std::decay<DTYPE>::type> visitData(DTYPE& data, std::string path, ARGS&&... args)
        {
            if (!path.empty() && OPTS::ACCUMULATE_PATH)
            {
                path += OPTS::DELIMINATOR;
            }
            static_cast<DERIVED*>(this)->visit(data, path, std::forward<ARGS>(args)...);
        }

      protected:
        template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA, class T, index_t I, class... ARGS>
        void visitMemberObject(T& obj,
                               std::string path,
                               MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA> ptr,
                               Indexer<I>,
                               ARGS&&... args)

        {
            if (OPTS::ACCUMULATE_PATH)
            {
                path += ptr.m_name.toString();
            }
            else
            {
                path = ptr.m_name.toString();
            }

            static_cast<DERIVED*>(this)->template visitData<I>(get(ptr, obj), path, std::forward<ARGS>(args)...);
        }

        template <class R, class T, class U, class SIG, class... ARGS>
        EnableIf<!std::is_same<R, void>::value> visitMemberFunction(
            T& obj, MemberFunction<U, SIG> func, const std::string& path, R*, VariadicTypedef<ARGS...>*, ARGS&&... args)
        {
            auto ret = func.invoke(obj, std::forward<ARGS>(args)...);
            static_cast<DERIVED*>(this)->visitReturn(std::move(ret), path);
        }

        template <class R, class T, class U, class SIG, class... ARGS>
        EnableIf<!std::is_same<R, void>::value> visitMemberFunction(
            T& obj, MemberFunction<U, SIG> func, const std::string& path, R*, VariadicTypedef<>*, ARGS&&... args)
        {
            auto ret = func.invoke(obj);
            static_cast<DERIVED*>(this)->visitReturn(std::move(ret), path, std::forward<ARGS>(args)...);
        }

        template <class R, class T, class U, class SIG, class... FARGS, class... ARGS>
        EnableIf<!std::is_same<R, void>::value>
        visitMemberFunction(T&, MemberFunction<U, SIG>, const std::string&, R*, VariadicTypedef<FARGS...>*, ARGS&&...)
        {
            // cannot invoke
        }

        template <class T, class U, class SIG, class... FARGS, class... ARGS>
        void visitMemberFunction(
            T&, MemberFunction<U, SIG>, const std::string&, void*, VariadicTypedef<FARGS...>*, ARGS&&...)
        {
            // cannot invoke
        }

        template <class T, index_t I, class U, Flag_t FLAGS, class METADATA, class... PTRS, class... ARGS>
        void visitMemberFunctions(T& obj,
                                  const std::string& path,
                                  MemberFunctionPointers<U, FLAGS, METADATA, PTRS...> ptrs,
                                  Indexer<I>,
                                  ARGS&&... args)
        {

            auto mem_fun = std::get<0>(ptrs.m_ptrs);
            using Ret_t = typename std::decay<decltype(mem_fun)>::type::Ret_t;
            using Args_t = typename std::decay<decltype(mem_fun)>::type::Args_t;
            std::string path_;
            if (OPTS::ACCUMULATE_PATH)
            {
                path_ = path;
                path_ = OPTS::DELIMINATOR + ptrs.m_name.toString();
            }
            else
            {
                path_ = ptrs.m_name.toString();
            }

            static_cast<DERIVED*>(this)->visitMemberFunction(obj,
                                                             mem_fun,
                                                             path_,
                                                             static_cast<Ret_t*>(nullptr),
                                                             static_cast<Args_t*>(nullptr),
                                                             std::forward<ARGS>(args)...);
        }

        template <class R, class U, class SIG, class... ARGS>
        EnableIf<!std::is_same<R, void>::value> visitStaticFunction(
            StaticFunction<U, SIG> func, const std::string& path, R*, VariadicTypedef<ARGS...>*, ARGS&&... args)
        {
            auto ret = func.invoke(std::forward<ARGS>(args)...);
            static_cast<DERIVED*>(this)->visitReturn(std::move(ret), path);
        }

        template <class R, class U, class SIG>
        EnableIf<!std::is_same<R, void>::value>
        visitStaticFunction(StaticFunction<U, SIG> func, const std::string& path, R*, VariadicTypedef<>*)
        {
            auto ret = func.invoke();
            static_cast<DERIVED*>(this)->visitReturn(std::move(ret), path);
        }

        template <class R, class U, class SIG, class... FARGS, class... ARGS>
        EnableIf<!std::is_same<R, void>::value>
        visitStaticFunction(StaticFunction<U, SIG>, const std::string&, R*, VariadicTypedef<FARGS...>*, ARGS&&...)
        {
            // cannot invoke
        }

        template <class T, index_t I, class U, Flag_t FLAGS, class... PTRS, class... ARGS>
        void visitStaticFunctions(const std::string& path,
                                  StaticFunctions<U, FLAGS, void, PTRS...> ptrs,
                                  Indexer<I>,
                                  ARGS&&... args)
        {

            auto stat_func = std::get<0>(ptrs.m_ptrs);
            using Ret_t = typename std::decay<decltype(stat_func)>::type::Ret_t;
            using Args_t = typename std::decay<decltype(stat_func)>::type::Args_t;
            std::string path_;
            if (OPTS::ACCUMULATE_PATH)
            {
                path_ = path;
                path_ = OPTS::DELIMINATOR + ptrs.m_name.toString();
            }
            else
            {
                path_ = ptrs.m_name.toString();
            }
            static_cast<DERIVED*>(this)->visitStaticFunction(stat_func,
                                                             path_,
                                                             static_cast<Ret_t*>(nullptr),
                                                             static_cast<Args_t*>(nullptr),
                                                             std::forward<ARGS>(args)...);
        }

        // general ignore case if OPTS::visitXX returns false
        template <class PTR, class T, index_t I, class... ARGS>
        void visitField(const T&, const std::string&, PTR, Indexer<I>, ARGS&&...)
        {
        }

        template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA, class T, index_t I, class... ARGS>
        auto visitField(T& obj,
                        const std::string& path,
                        MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA> ptrs,
                        Indexer<I> idx,
                        ARGS&&... args) -> EnableIf<OPTS::visitMemberObject(static_cast<decltype(ptrs)*>(nullptr))>
        {
            static_cast<DERIVED*>(this)->template visitMemberObject(obj, path, ptrs, idx, std::forward<ARGS>(args)...);
        }

        template <class T, index_t I, class GET_PTR, class SET_PTR, Flag_t FLAGS, class METADATA, class... ARGS>
        auto visitField(T& obj,
                        const std::string& path,
                        MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA> ptrs,
                        Indexer<I> idx,
                        ARGS&&... args) -> EnableIf<OPTS::visitProperty(static_cast<decltype(ptrs)*>(nullptr))>
        {
            static_cast<DERIVED*>(this)->visitProperty(obj, path, ptrs, idx, std::forward<ARGS>(args)...);
        }

        template <class T, index_t I, class U, Flag_t FLAGS, class METADATA, class... PTRS, class... ARGS>
        auto visitField(T& obj,
                        const std::string& path,
                        MemberFunctionPointers<U, FLAGS, METADATA, PTRS...> ptrs,
                        Indexer<I> idx,
                        ARGS&&... args) -> EnableIf<OPTS::visitMemberFunctions(static_cast<decltype(ptrs)*>(nullptr))>
        {
            static_cast<DERIVED*>(this)->visitMemberFunctions(obj, path, ptrs, idx, std::forward<ARGS>(args)...);
        }

        template <class T, index_t I, class U, Flag_t FLAGS, class... PTRS, class... ARGS>
        auto visitField(const T&,
                        const std::string& path,
                        StaticFunctions<U, FLAGS, void, PTRS...> ptrs,
                        Indexer<I> idx,
                        ARGS&&... args) -> EnableIf<OPTS::visitStaticFunctions(static_cast<decltype(ptrs)*>(nullptr))>
        {
            static_cast<DERIVED*>(this)->template visitStaticFunctions<T>(path, ptrs, idx, std::forward<ARGS>(args)...);
        }

        template <class T, class... ARGS>
        void recurseFields(T& obj, const std::string& path, Indexer<0> idx, ARGS&&... args)
        {
            auto ptr = Reflect<T>::getPtr(idx);
            static_cast<DERIVED*>(this)->visitField(obj, path, ptr, idx, std::forward<ARGS>(args)...);
        }

        template <class T, index_t I, class... ARGS>
        void recurseFields(T& obj, const std::string& path, Indexer<I> idx, ARGS&&... args)
        {
            recurseFields(obj, path, --idx, std::forward<ARGS>(args)...);
            auto ptr = Reflect<T>::getPtr(idx);
            static_cast<DERIVED*>(this)->visitField(obj, path, ptr, idx, std::forward<ARGS>(args)...);
        }
        //////////////////////////////////////////////////////////////////////////
        // const versions of the above
        //////////////////////////////////////////////////////////////////////////

        template <bool ENABLE,
                  class DTYPE,
                  class CTYPE,
                  Flag_t FLAGS,
                  class METADATA,
                  class T,
                  index_t I,
                  class... ARGS>
        EnableIf<ENABLE> visitMemberObject(const T& obj,
                                           std::string path,
                                           MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA> ptr,
                                           Indexer<I>,
                                           ARGS&&... args)
        {
            if (OPTS::ACCUMULATE_PATH)
            {
                path += ptr.m_name.toString();
            }
            else
            {
                path = ptr.m_name.toString();
            }

            static_cast<DERIVED*>(this)->template visitData<I>(ptr.get(obj), path, std::forward<ARGS>(args)...);
        }

        template <class R, class T, class U, class SIG, class... ARGS>
        EnableIf<!std::is_same<R, void>::value> visitMemberFunction(const T& obj,
                                                                    MemberFunction<U, SIG> func,
                                                                    const std::string& path,
                                                                    R*,
                                                                    VariadicTypedef<ARGS...>*,
                                                                    ARGS&&... args)
        {
            auto ret = func.invoke(obj, std::forward<ARGS>(args)...);
            static_cast<DERIVED*>(this)->visitReturn(std::move(ret), path);
        }

        template <class R, class T, class U, class SIG>
        EnableIf<!std::is_same<R, void>::value>
        visitMemberFunction(const T& obj, MemberFunction<U, SIG> func, const std::string& path, R*, VariadicTypedef<>*)
        {
            auto ret = func.invoke(obj);
            static_cast<DERIVED*>(this)->visitReturn(std::move(ret), path);
        }

        template <class T, class U, class SIG>
        void visitMemberFunction(const T&, MemberFunction<U, SIG>, const std::string&, void*, VariadicTypedef<>*)
        {
            // cannot invoke
        }

        template <class T, index_t I, class U, Flag_t FLAGS, class METADATA, class... PTRS, class... ARGS>
        void visitMemberFunctions(const T& obj,
                                  const std::string& path,
                                  MemberFunctionPointers<U, FLAGS, METADATA, PTRS...> ptrs,
                                  Indexer<I>,
                                  ARGS&&... args)
        {
            auto mem_fun = std::get<0>(ptrs.m_ptrs);
            using Ret_t = typename std::decay<decltype(mem_fun)>::type::Ret_t;
            using Args_t = typename std::decay<decltype(mem_fun)>::type::Args_t;
            std::string path_;
            if (OPTS::ACCUMULATE_PATH)
            {
                path_ = path;
                path_ = OPTS::DELIMINATOR + ptrs.m_name.toString();
            }
            else
            {
                path_ = ptrs.m_name.toString();
            }

            static_cast<DERIVED*>(this)->visitMemberFunction(obj,
                                                             mem_fun,
                                                             path_,
                                                             static_cast<Ret_t*>(nullptr),
                                                             static_cast<Args_t*>(nullptr),
                                                             std::forward<ARGS>(args)...);
        }

        template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA, class T, index_t I, class... ARGS>
        auto visitField(const T& obj,
                        const std::string& path,
                        MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA> ptrs,
                        Indexer<I> idx,
                        ARGS&&... args) -> EnableIf<OPTS::visitMemberObject(static_cast<decltype(ptrs)*>(nullptr))>
        {
            static_cast<DERIVED*>(this)->template visitMemberObject<VISIT_OBJECTS && !(FLAGS & DO_NOT_SERIALIZE)>(
                obj, path, ptrs, idx, std::forward<ARGS>(args)...);
        }

        template <class T, index_t I, class GET_PTR, class SET_PTR, Flag_t FLAGS, class METADATA, class... ARGS>
        auto visitField(const T& obj,
                        const std::string& path,
                        MemberPropertyPointer<GET_PTR, SET_PTR, FLAGS, METADATA> ptrs,
                        Indexer<I> idx,
                        ARGS&&... args) -> EnableIf<OPTS::visitProperty(static_cast<decltype(ptrs)*>(nullptr))>
        {
            static_cast<DERIVED*>(this)->visitProperty(obj, path, ptrs, idx, std::forward<ARGS>(args)...);
        }

        template <class T, index_t I, class U, Flag_t FLAGS, class METADATA, class... PTRS, class... ARGS>
        auto visitField(const T& obj,
                        const std::string& path,
                        MemberFunctionPointers<U, FLAGS, METADATA, PTRS...> ptrs,
                        Indexer<I> idx,
                        ARGS&&... args) -> EnableIf<OPTS::visitMemberFunctions(static_cast<decltype(ptrs)*>(nullptr))>
        {
            static_cast<DERIVED*>(this)->visitMemberFunctions(obj, path, ptrs, idx, std::forward<ARGS>(args)...);
        }

        template <class T, class... ARGS>
        void recurseFields(const T& obj, const std::string& path, Indexer<0> idx, ARGS&&... args)
        {
            auto ptr = Reflect<T>::getPtr(idx);
            static_cast<DERIVED*>(this)->visitField(obj, path, ptr, idx, std::forward<ARGS>(args)...);
        }

        template <class T, index_t I, class... ARGS>
        void recurseFields(const T& obj, const std::string& path, Indexer<I> idx, ARGS&&... args)
        {
            recurseFields(obj, path, --idx, std::forward<ARGS>(args)...);
            auto ptr = Reflect<T>::getPtr(idx);
            static_cast<DERIVED*>(this)->visitField(obj, path, ptr, idx, std::forward<ARGS>(args)...);
        }
    };
}

#endif // CT_REFLECT_VISITOR_HPP
