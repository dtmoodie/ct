#include "BinaryWriter.hpp"

namespace ct
{

    BinaryWriter::BinaryWriter(std::ostream& in) : m_os(in) {}

    BinaryWriter::~BinaryWriter() {}

    template <class T>
    IDynamicVisitor& BinaryWriter::writeBinary(T* ptr, const std::string&, const size_t cnt)
    {
        const size_t size = sizeof(T) * cnt;
        m_os.write(reinterpret_cast<char*>(ptr), static_cast<std::streamsize>(size));
        return *this;
    }

    IDynamicVisitor& BinaryWriter::operator()(char* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IDynamicVisitor& BinaryWriter::operator()(int8_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IDynamicVisitor& BinaryWriter::operator()(uint8_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IDynamicVisitor& BinaryWriter::operator()(int16_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IDynamicVisitor& BinaryWriter::operator()(uint16_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IDynamicVisitor& BinaryWriter::operator()(int32_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IDynamicVisitor& BinaryWriter::operator()(uint32_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IDynamicVisitor& BinaryWriter::operator()(int64_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IDynamicVisitor& BinaryWriter::operator()(uint64_t* ptr, const std::string& name, const size_t cnt)
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
        return DynamicVisitor::operator()(val, name);
    }

    IDynamicVisitor& BinaryWriter::operator()(IContainerTraits* val, const std::string&)
    {
        uint64_t num_vals = val->numValues();
        writeBinary(&num_vals);
        val->visit(this);
        return *this;
    }

    VisitorTraits BinaryWriter::traits() const
    {
        VisitorTraits out;
        out.reader = false;
        out.supports_named_access = false;
        return out;
    }
}
