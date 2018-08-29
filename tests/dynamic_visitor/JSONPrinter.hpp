#pragma once
#include "ct/DynamicVisitor.hpp"
#include <iostream>
#include <unordered_map>
namespace ct
{
    class DynamicPrintVisitor: public IDynamicVisitor
    {
    public:
        DynamicPrintVisitor(std::ostream& out = std::cout);
        virtual ~DynamicPrintVisitor() override;
        virtual IDynamicVisitor& operator()(int8_t* ,         const std::string& , const size_t ) override;
        virtual IDynamicVisitor& operator()(uint8_t* ,        const std::string& , const size_t ) override;
        virtual IDynamicVisitor& operator()(int16_t* ,        const std::string& , const size_t ) override;
        virtual IDynamicVisitor& operator()(uint16_t* ,       const std::string& , const size_t ) override;
        virtual IDynamicVisitor& operator()(int32_t* ,        const std::string& , const size_t ) override;
        virtual IDynamicVisitor& operator()(uint32_t* ,       const std::string& , const size_t ) override;
        virtual IDynamicVisitor& operator()(int64_t* ,        const std::string& , const size_t ) override;
        virtual IDynamicVisitor& operator()(uint64_t* ,       const std::string& , const size_t ) override;

        virtual IDynamicVisitor& operator()(float* val, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(double* ,         const std::string& , const size_t ) override;
        virtual IDynamicVisitor& operator()(void* ,        const std::string& , const size_t ) override;
        virtual IDynamicVisitor& operator()(IStructTraits* val, const std::string& name = "") override;
        virtual std::unique_ptr<IDataContainer>& accessCache(const std::string& name) override;

    protected:
        struct State
        {
            uint32_t unnamed_count = 0;
            bool writing_array = false;
            bool writing_struct = false;
            bool prev_elem = false;
        };
        template<class T>
        void writePod(T* ptr, const std::string& name, const size_t cnt);

        virtual IDynamicVisitor& startContainer(IContainerTraits&, const std::string& name) override;
        virtual IDynamicVisitor& endContainer() override;
        void indent();

        std::unordered_map<std::string, std::unique_ptr<ct::IDataContainer>> m_cache;
        std::ostream& m_os;
        std::vector<State> m_state;
    };
}
