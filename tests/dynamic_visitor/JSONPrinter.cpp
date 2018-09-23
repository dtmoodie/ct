#include "JSONPrinter.hpp"

namespace ct
{

    JSONWriter::JSONWriter(std::ostream& os) : m_ar(os) {}

    template <class T>
    IWriteVisitor& JSONWriter::writePod(const T* ptr, const std::string& name, const size_t cnt)
    {
        if (!name.empty())
        {
            m_ar.setNextName(name.c_str());
        }
        else
        {
        }
        for (size_t i = 0; i < cnt; ++i)
        {
            m_ar(ptr[i]);
        }
        return *this;
    }
    IWriteVisitor& JSONWriter::operator()(const char* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const int8_t* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const uint8_t* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const int16_t* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const uint16_t* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const int32_t* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const uint32_t* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const int64_t* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const uint64_t* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const float* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const double* val, const std::string& name, const size_t cnt)
    {
        return writePod(val, name, cnt);
    }

    IWriteVisitor& JSONWriter::operator()(const void* val, const std::string& name, const size_t cnt)
    {
        m_ar.saveBinaryValue(val, cnt, name.c_str());
        return *this;
    }

    IWriteVisitor& JSONWriter::operator()(const IStructTraits* val, const std::string& name)
    {
        if (!name.empty())
        {
            m_ar.setNextName(name.c_str());
        }
        m_ar.startNode();
        WriteCache::operator()(val, name);
        m_ar.finishNode();
        return *this;
    }

    IWriteVisitor& JSONWriter::operator()(const IContainerTraits* val, const std::string& name)
    {
        if (!name.empty())
        {
            m_ar.setNextName(name.c_str());
        }
        if (val->type() != TypeInfo(typeid(std::string)))
        {
            m_ar.startNode();
            if (val->keyType() != TypeInfo(typeid(std::string)))
            {
                m_ar.makeArray();
            }
        }
        val->visit(this);
        if (val->type() != TypeInfo(typeid(std::string)))
        {
            m_ar.finishNode();
        }
        return *this;
    }

    VisitorTraits JSONWriter::traits() const
    {
        VisitorTraits out;
        out.supports_named_access = true;
        out.reader = false;
        return out;
    }
}
