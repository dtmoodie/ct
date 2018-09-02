#include "BinaryWriter.hpp"

namespace ct
{

BinaryWriter::BinaryWriter(std::ostream& in):
    m_os(in)
{
}

BinaryWriter::~BinaryWriter()
{

}

template<class T>
IDynamicVisitor& BinaryWriter::writeBinary(T* ptr, const std::string& name, const size_t cnt)
{
    const size_t size = sizeof(T) * cnt;
    m_os.write(reinterpret_cast<char*>(ptr), size);
    return *this;
}

IDynamicVisitor& BinaryWriter::operator()(char* ptr, const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(int8_t* ptr,         const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(uint8_t* ptr,        const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(int16_t* ptr,        const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(uint16_t* ptr,       const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(int32_t* ptr,        const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(uint32_t* ptr,       const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(int64_t* ptr,        const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(uint64_t* ptr,       const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(float* ptr, const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(double* ptr, const std::string& name, const size_t cnt)
{
    return writeBinary(ptr, name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(void* ptr, const std::string& name, const size_t cnt)
{
    return writeBinary(reinterpret_cast<char*>(ptr), name, cnt);
}

IDynamicVisitor& BinaryWriter::operator()(IStructTraits* val, const std::string& name)
{
    val->visit(this);
    return *this;
}

std::unique_ptr<IDataContainer>& BinaryWriter::accessCache(const std::string& name)
{
    return m_cache[name];
}

IDynamicVisitor& BinaryWriter::startContainer(IContainerTraits & trait,
    const std::string &name)
{
    uint64_t num_vals = trait.numValues();
    writeBinary(&num_vals);
    return *this;
}

IDynamicVisitor& BinaryWriter::endContainer()
{
    return *this;
}

bool BinaryWriter::reading() const
{
    return true;
}

bool BinaryWriter::isTextVisitor() const
{
    return false;
}

}
