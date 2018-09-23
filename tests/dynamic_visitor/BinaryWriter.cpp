#include "BinaryWriter.hpp"

namespace ct
{

    BinaryWriter::BinaryWriter(std::ostream& in) : m_os(in) {}

    BinaryWriter::~BinaryWriter() {}

    template <class T>
    IWriteVisitor& BinaryWriter::writeBinary(const T* ptr, const std::string&, const size_t cnt)
    {
        const size_t size = sizeof(T) * cnt;
        m_os.write(reinterpret_cast<const char*>(ptr), static_cast<std::streamsize>(size));
        return *this;
    }

    IWriteVisitor& BinaryWriter::operator()(const char* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const int8_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const uint8_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const int16_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const uint16_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const int32_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const uint32_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const int64_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const uint64_t* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const float* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const double* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(ptr, name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const void* ptr, const std::string& name, const size_t cnt)
    {
        return writeBinary(reinterpret_cast<const char*>(ptr), name, cnt);
    }

    IWriteVisitor& BinaryWriter::operator()(const IStructTraits* val, const std::string& name)
    {
        return WriteCache::operator()(val, name);
    }

    IWriteVisitor& BinaryWriter::operator()(const IContainerTraits* val, const std::string&)
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
