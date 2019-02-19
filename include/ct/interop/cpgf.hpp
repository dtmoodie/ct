#ifndef CT_CPGF_HPP
#define CT_CPGF_HPP
#include <ct/reflect.hpp>

#include <cpgf/gmetaapiutil.h>
#include <cpgf/gmetadefine.h>

#include <iostream>

namespace ct
{
    template<class T, class DEFINITION, index_t I>
    typename std::enable_if<std::is_same<typename AccessorType<T, I>::GetterType_t, FieldGetterType>::value>::type
    registerHelperImpl(DEFINITION& def, const Indexer<I> idx)
    {
        auto accessor = Reflect<T>::getPtr(idx);
        def._field(getName<I, T>(), accessor.m_getter);
    }

    template<class T, class DEFINITION, index_t I>
    typename std::enable_if<std::is_same<typename AccessorType<T, I>::GetterType_t, MemberAccessorGetterType>::value && !std::is_same<typename AccessorType<T, I>::SetterType_t, void>::value>::type
    registerHelperImpl(DEFINITION& def, const Indexer<I> idx)
    {
        auto accessor = Reflect<T>::getPtr(idx);
        def._property(getName<I, T>(), accessor.m_getter, accessor.m_setter);
    }

    template<class T, class DEFINITION, index_t I>
    typename std::enable_if<std::is_same<typename AccessorType<T, I>::GetterType_t, MemberAccessorGetterType>::value && std::is_same<typename AccessorType<T, I>::SetterType_t, void>::value>::type
    registerHelperImpl(DEFINITION& def, const Indexer<I> idx)
    {
        auto accessor = Reflect<T>::getPtr(idx);
        def._property(getName<I, T>(), accessor.m_getter, 0);
    }

    template<class T, class DEFINITION, index_t I>
    void registerHelper(DEFINITION& def, const Indexer<I> idx)
    {
        auto name = getName<I, T>();
        registerHelperImpl<T>(def, idx);
    }

    template<class T, class DEFINITION>
    void recursiveRegisterHelper(DEFINITION& def, Indexer<0> idx)
    {
        registerHelper<T>(def, idx);
    }

    template<class T, class DEFINITION, index_t I>
    void recursiveRegisterHelper(DEFINITION& def, Indexer<I> idx)
    {
        recursiveRegisterHelper<T>(def, --idx);
        registerHelper<T>(def, idx);
    }

    template<class T, class ... BASES>
    auto createDefinition(const char* name, VariadicTypedef<BASES...>) -> decltype(cpgf::GDefineMetaClass<T, BASES...>::define(name))
    {
        return cpgf::GDefineMetaClass<T, BASES...>::define(name);
    }


    template<class T>
    auto createDefinition(const char* name, VariadicTypedef<void>) -> decltype(cpgf::GDefineMetaClass<T>::define(name))
    {
        return cpgf::GDefineMetaClass<T>::define(name);
    }

    template<class T>
    void registerReflectedClass(const char* name_ = nullptr)
    {
        using namespace cpgf;

        std::string name = std::string("class::");
        if(name_)
        {
            name += name_;
        }else
        {
             name += Reflect<T>::getName();
        }
        auto definition = createDefinition<T>(name.c_str(), typename Reflect<T>::Base{});
        recursiveRegisterHelper<T>(definition, Reflect<T>::end());
    }
}

namespace cpgf
{
    std::ostream& operator<<(std::ostream& os, const GMetaClass* cls)
    {
        os << cls->getQualifiedName() << "\n";
        auto num_bases = cls->getBaseCount();
        os << "Bases: " << num_bases << "\n";

        os << "CanCopy: " << cls->canCopyInstance() << "\n";
        os << "CanCreate: " << cls->canCreateInstance() << "\n";
        os << "Constructors: " << cls->getConstructorCount() << "\n";
        const auto num_methods = cls->getMethodCount();
        os << "Methods: " << num_methods << "\n";
        for(int i = 0; i < num_methods; ++i)
        {
            const GMetaMethod* method = cls->getMethodAt(i);
            const auto num_params = method->getParamCount();

            auto result = method->getResultType();
            auto result_name = result.getBaseName();
            if(result_name == nullptr)
            {
                result_name = result.getBaseType().getStdTypeInfo().name();
            }
            if(result.isConst())
            {
                os << "const ";
            }

            os << result_name;
            if(result.isReference())
            {
                os << "&";
            }
            if(result.isPointer())
            {
                os << "*";
            }

            const auto& name = method->getName();
            os << " " << name << "(";
            for(int j = 0; j < num_params; ++j)
            {
                if(j != 0)
                {
                    os << ", ";
                }
                GMetaType type = method->getParamType(j);
                if(type.isConst())
                {
                    os << "const ";
                }
                const char* type_name = type.getBaseName();
                if(!type_name)
                {
                    type_name = type.getBaseType().getStdTypeInfo().name();
                }

                os << type_name;

                if(type.isReference())
                {
                    os << "& ";
                }
            }
            os << ")";
            if(method->getItemType().isConstFunction())
            {
                os << " const\n";
            }
            else
            {
                os << "\n";
            }
        }
        const auto num_fields = cls->getFieldCount();
        os << "Fields: " << num_fields << "\n";
        for(int i = 0; i < num_fields; ++i)
        {
            auto field = cls->getFieldAt(i);
            os << " " << field->getName() << " " << field->getItemType().getBaseType().getStdTypeInfo().name() << "\n";
        }

        const auto num_properties = cls->getPropertyCount();
        os << "Properties: " << num_properties << "\n";
        for(int i = 0; i < num_properties; ++i)
        {
            auto prop = cls->getPropertyAt(i);
            os << " " << prop->getItemType().getBaseType().getStdTypeInfo().name() << " " << prop->getName() << "()\n";
        }
        return os;
    }
}

#endif // CT_CPGF_HPP
