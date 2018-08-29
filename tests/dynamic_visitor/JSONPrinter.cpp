#include "JSONPrinter.hpp"

namespace ct
{

template<class T>
void DynamicPrintVisitor::writePod(T* val, const std::string& name, const size_t cnt)
{
    if(m_state.back().prev_elem)
    {
        m_os << ",\n";
    }
    indent();
    if(cnt > 1)
    {
        m_os << name << ": [";
        for(size_t i = 0; i < cnt; ++i)
        {
            if(i != 0)
            {
                m_os << ",";
            }
            m_os << val[i];
        }
        m_os << "] ";
    }else
    {
        m_os << name << ":" << *val;
    }
    m_state.back().prev_elem = true;
}

DynamicPrintVisitor::DynamicPrintVisitor(std::ostream& out):
    m_os(out)
{
    m_os << "{\n";
    m_state.push_back(State());
}

DynamicPrintVisitor::~DynamicPrintVisitor()
{
    m_os << "\n}";
}

IDynamicVisitor& DynamicPrintVisitor::operator()(int8_t* ptr,const std::string& name, const size_t cnt)
{
    writePod(ptr, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(uint8_t* ptr, const std::string& name, const size_t cnt)
{
    writePod(ptr, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(int16_t* ptr, const std::string& name, const size_t cnt)
{
    writePod(ptr, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(uint16_t* ptr, const std::string& name, const size_t cnt)
{
    writePod(ptr, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(int32_t* ptr, const std::string& name, const size_t cnt)
{
    writePod(ptr, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(uint32_t* ptr, const std::string& name, const size_t cnt)
{
    writePod(ptr, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(int64_t* ptr, const std::string& name, const size_t cnt)
{
    writePod(ptr, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(uint64_t* ptr, const std::string& name, const size_t cnt)
{
    writePod(ptr, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::startContainer(IContainerTraits& trait, const std::string& name)
{
    if(trait.keyType() == TypeInfo(typeid(void)))
    {
        if(m_state.back().prev_elem)
        {
            m_os << ",\n";
        }
        indent();
        if(name.empty())
        {
            m_os << "value" << m_state.back().unnamed_count;
            ++m_state.back().unnamed_count;
        }else
        {
            m_os << name;
        }
        m_os << ": [\n";
        m_state.push_back(State());
        m_state.back().writing_array = true;
    }
    namespaces.push_back(name.c_str());
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::endContainer()
{
    const bool writing_array = m_state.back().writing_array;
    if(writing_array)
    {
        m_os << "\n";
    }
    m_state.pop_back();
    namespaces.pop_back();
    if(writing_array)
    {
        indent();
        m_os << "]";
    }
    m_state.back().prev_elem = true;
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(float* val, const std::string& name, const size_t cnt)
{
    writePod(val, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(double* ptr, const std::string& name, const size_t cnt)
{
    writePod(ptr, name, cnt);
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(void* ptr, const std::string& name, const size_t cnt)
{
    return *this;
}

IDynamicVisitor& DynamicPrintVisitor::operator()(IStructTraits* val, const std::string& name)
{
    if(m_state.back().prev_elem)
    {
        m_os << ",\n";
    }
    indent();
    if(!m_state.back().writing_array)
    {
        if(name.empty())
        {
            m_os << "value" << m_state.back().unnamed_count;
            ++(m_state.back().unnamed_count);
        }else
        {
            m_os << name;
        }
        m_os  << ":{\n";
    }else
    {
        m_os  << "{\n";
    }


    m_state.back().prev_elem = false;
    IDynamicVisitor::operator()(val, name);
    m_os << "\n";
    indent();
    m_os << "}";
    m_state.back().prev_elem = true;
    return *this;
}

std::unique_ptr<IDataContainer>& DynamicPrintVisitor::accessCache(const std::string& name)
{
    return m_cache[name];
}

void DynamicPrintVisitor::indent()
{
    for(size_t i = 0; i < namespaces.size() + 1; ++i)
    {
        m_os << "  ";
    }
}
}
