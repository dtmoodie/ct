#pragma once
#include "ct/DynamicVisitor.hpp"
#include <cereal/archives/json.hpp>

#include <iostream>
#include <unordered_map>
namespace ct
{

struct JSONWriter : public IDynamicVisitor
{
    JSONWriter(std::ostream& os);
    virtual IDynamicVisitor& operator()(char* val, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(int8_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(uint8_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(int16_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(uint16_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(int32_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(uint32_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(int64_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(uint64_t*, const std::string& name = "", const size_t cnt = 1) override;

    virtual IDynamicVisitor& operator()(float* val, const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(double*, const std::string&, const size_t) override;
    virtual IDynamicVisitor& operator()(void*, const std::string&, const size_t) override;
    virtual IDynamicVisitor& operator()(IStructTraits* val, const std::string& name = "") override;
    virtual IDynamicVisitor& operator()(IContainerTraits* val, const std::string& name = "") override;

    virtual VisitorTraits traits() const override;

    virtual std::unique_ptr<IDataContainer>& accessCache(const std::string& name) override;

  private:
    template <class T>
    IDynamicVisitor& writePod(T* ptr, const std::string& name, const size_t cnt);
    std::unordered_map<std::string, std::unique_ptr<ct::IDataContainer>> m_cache;
    cereal::JSONOutputArchive m_ar;
};

struct JSONReader : public IDynamicVisitor
{
};

class JSONPrinter : public IDynamicVisitor
{
  public:
    JSONPrinter(std::ostream& out = std::cout, const bool whitespace = true);
    virtual ~JSONPrinter() override;

    virtual IDynamicVisitor& operator()(char* val, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(int8_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(uint8_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(int16_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(uint16_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(int32_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(uint32_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(int64_t*, const std::string& name = "", const size_t cnt = 1) override;
    virtual IDynamicVisitor& operator()(uint64_t*, const std::string& name = "", const size_t cnt = 1) override;

    virtual IDynamicVisitor& operator()(float* val, const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(double*, const std::string&, const size_t) override;
    virtual IDynamicVisitor& operator()(void*, const std::string&, const size_t) override;
    virtual IDynamicVisitor& operator()(IStructTraits* val, const std::string& name = "") override;
    virtual IDynamicVisitor& operator()(IContainerTraits* val, const std::string& name = "") override;

    virtual VisitorTraits traits() const override;

    virtual std::unique_ptr<IDataContainer>& accessCache(const std::string& name) override;

  protected:
    struct State
    {
        uint32_t unnamed_count = 0;
        bool writing_array = false;
        bool writing_struct = false;
        bool prev_elem = false;
        bool writing_key = false;
        bool writing_value = false;
        IStructTraits* struct_trait = nullptr;
        IContainerTraits* container_trait = nullptr;
    };
    template <class T>
    void writePod(T* ptr, const std::string& name, const size_t cnt);

    void indent();
    void newline();
    void prev();
    void writeName(const std::string& n);

    std::unordered_map<std::string, std::unique_ptr<ct::IDataContainer>> m_cache;
    std::ostream& m_os;
    std::vector<State> m_state;
    bool m_whitespace = true;
};
}
