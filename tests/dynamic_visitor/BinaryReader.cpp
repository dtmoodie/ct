#include "BinaryReader.hpp"

namespace ct
{

BinaryReader::BinaryReader(std::istream& in) : m_is(in)
{
}

BinaryReader::~BinaryReader()
{
}

template <class T>
IDynamicVisitor& BinaryReader::readBinary(T* ptr, const size_t cnt)
{
    m_is.read(reinterpret_cast<char*>(ptr), cnt * sizeof(T));
    return *this;
}

IDynamicVisitor& BinaryReader::operator()(char* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(int8_t* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(uint8_t* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(int16_t* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(uint16_t* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(int32_t* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(uint32_t* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(int64_t* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(uint64_t* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(float* ptr, const std::string& name, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(double* ptr, const std::string&, const size_t cnt)
{
    return readBinary(ptr, cnt);
}

IDynamicVisitor& BinaryReader::operator()(void* ptr, const std::string&, const size_t cnt)
{
    return readBinary(reinterpret_cast<char*>(ptr), cnt);
}

IDynamicVisitor& BinaryReader::operator()(IStructTraits* val, const std::string& name)
{
    val->visit(this);
    return *this;
}

IDynamicVisitor& BinaryReader::operator()(IContainerTraits* val, const std::string& name)
{
    uint64_t size = 0;
    readBinary(&size);
    val->setValues(size);
    val->setKeys(size);
    val->visit(this);
    return *this;
}

VisitorTraits BinaryReader::traits() const
{
    ct::VisitorTraits out;
    out.reader = true;
    out.supports_named_access = false;
    return out;
}

std::unique_ptr<IDataContainer>& BinaryReader::accessCache(const std::string& name)
{
    return m_cache[name];
}
}
