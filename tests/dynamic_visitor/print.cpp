#include "ct/DynamicVisitor.hpp"
#include <iostream>
#include <unordered_map>

namespace ct
{
    class DynamicPrintVisitor: public IDynamicVisitor
    {
    public:
        DynamicPrintVisitor(std::ostream& out = std::cout):os(out)
        {
            os << "{\n";
        }
        virtual ~DynamicPrintVisitor() override
        {
            os << "\n}";
        }
        virtual IDynamicVisitor& operator()(int8_t* ,         const std::string& , const size_t ) override
        {
            return *this;
        }
        virtual IDynamicVisitor& operator()(uint8_t* ,        const std::string& , const size_t ) override
        {
            return *this;
        }
        virtual IDynamicVisitor& operator()(int16_t* ,        const std::string& , const size_t ) override
        {
            return *this;
        }
        virtual IDynamicVisitor& operator()(uint16_t* ,       const std::string& , const size_t ) override
        {
            return *this;
        }
        virtual IDynamicVisitor& operator()(int32_t* ,        const std::string& , const size_t ) override
        {
            return *this;
        }
        virtual IDynamicVisitor& operator()(uint32_t* ,       const std::string& , const size_t ) override
        {
            return *this;
        }
        virtual IDynamicVisitor& operator()(int64_t* ,        const std::string& , const size_t ) override
        {
            return *this;
        }
        virtual IDynamicVisitor& operator()(uint64_t* ,       const std::string& , const size_t ) override
        {
            return *this;
        }

        virtual IDynamicVisitor& startContainer(IContainerTraits&) override
        {
            return *this;
        }

        virtual IDynamicVisitor& endContainer() override
        {
            return *this;
        }

        virtual IDynamicVisitor& operator()(float* val,          const std::string& name, const size_t cnt) override
        {
            if(prev_elem)
            {
                os << ",\n";
            }
            indent();
            if(cnt > 1)
            {
                os << name << ": [";
                for(size_t i = 0; i < cnt; ++i)
                {
                    if(i != 0)
                    {
                        os << ",";
                    }
                    os << val[i];
                }
                os << "] ";
            }else
            {
                os << name << ":" << *val;
            }
            prev_elem = true;
            return *this;
        }
        virtual IDynamicVisitor& operator()(double* ,         const std::string& , const size_t ) override
        {
            return *this;
        }
        virtual IDynamicVisitor& operator()(void* ,        const std::string& , const size_t ) override
        {
            return *this;
        }

        virtual IDynamicVisitor& operator()(IStructTraits* val, const std::string& name = "", const size_t cnt = 1) override
        {
            if(prev_elem)
            {
                os << ",\n";
            }
            indent();
            os << name << ":{\n";
            prev_elem = false;
            IDynamicVisitor::operator()(val, name, cnt);
            os << "\n";
            indent();
            os << "}";
            prev_elem = true;
            return *this;
        }

        virtual std::unique_ptr<IDataContainer>& accessCache(const std::string& name) override
        {
            return m_cache[name];
        }

        void indent()
        {
            for(size_t i = 0; i < namespaces.size() + 1; ++i)
            {
                os << "  ";
            }
        }

        std::unordered_map<std::string, std::unique_ptr<ct::IDataContainer>> m_cache;
        std::ostream& os;
        bool prev_elem = false;
    };
}

struct Vec
{
    float x, y, z;
};
namespace ct
{
    template<>
    struct TTraits<Vec>: IStructTraits
    {
        Vec& m_vec;
        TTraits(Vec& vec):m_vec(vec){}

        virtual size_t size() const
        {
            return sizeof(Vec);
        }
        virtual bool triviallySerializable() const
        {
            return std::is_pod<Vec>::value;
        }
        void visit(ct::IDynamicVisitor* visitor)
        {
            (*visitor)(&m_vec.x, "x")(&m_vec.y, "y")(&m_vec.z, "z");
        }
        virtual const void* ptr() const
        {
            return &m_vec;
        }
        virtual void* ptr()
        {
            return &m_vec;
        }

    };
}
int main()
{
    {
        ct::DynamicPrintVisitor visitor_impl;
        ct::IDynamicVisitor& visitor = visitor_impl;
        Vec vec{0,1,2};
        auto traits = ct::makeTraits(vec);
        visitor(static_cast<ct::IStructTraits*>(&traits), "vec");
        std::vector<float> vector{0,1,2,3,4,5,6};
        visitor(&vector, "vector");
        std::vector<Vec> vecs;
        //visitor(&vecs, "vecs");

    }

    std::cout << std::endl;
}

