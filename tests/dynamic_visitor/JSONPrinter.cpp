#include "JSONPrinter.hpp"

namespace ct {

template <class T>
void JSONPrinter::writePod(T *val, const std::string &name, const size_t cnt) {
  if (m_state.back().prev_elem) {
    m_os << ",";
    newline();
  }
  indent();
  if (cnt > 1) {
    m_os << name << ": [";
    for (size_t i = 0; i < cnt; ++i) {
      if (i != 0) {
        m_os << ",";
      }
      m_os << val[i];
    }
    m_os << "] ";
  } else {
    if (m_state.back().writing_key) {
      m_os << "{";
      newline();
      indent();
    }
    m_os << name << ":" << *val;
    if (m_state.back().writing_value) {
      newline();
      m_os << "}";
    }
    if (m_state.back().writing_key) {
      m_state.back().writing_key = false;
      m_state.back().writing_value = true;
      m_state.push_back(State());
    }
    if (m_state.back().writing_value) {
      m_os << "}";
      m_state.pop_back();
      m_state.back().writing_key = true;
      m_state.back().writing_value = false;
    }
  }

  m_state.back().prev_elem = true;
}

JSONPrinter::JSONPrinter(std::ostream &out, const bool whitespace)
    : m_os(out), m_whitespace(whitespace)

{
  m_os << "{";
  newline();
  m_state.push_back(State());
}

JSONPrinter::~JSONPrinter() {
  if (m_state.back().prev_elem) {
    newline();
  }
  m_os << "}";
  newline();
}

bool JSONPrinter::reading() const { return true; }

IDynamicVisitor &JSONPrinter::operator()(char *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(int8_t *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(uint8_t *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(int16_t *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(uint16_t *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(int32_t *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(uint32_t *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(int64_t *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(uint64_t *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::startContainer(IContainerTraits &trait,
                                             const std::string &n) {
  prev();
  indent();
  writeName(n);
  if (trait.keyType() == TypeInfo(typeid(void))) {
    m_os << ": [";
    newline();
    m_state.push_back(State());
    m_state.back().writing_array = true;
  } else {
    if (trait.keyType() == TypeInfo(typeid(std::string))) {
      m_os << ": {";
      newline();
      m_state.push_back(State());
      m_state.back().writing_struct = true;
    } else {
      m_os << ": [";
      newline();
      m_state.push_back(State());
      m_state.back().writing_array = true;
      m_state.back().writing_key = true;
    }
  }
  namespaces.push_back(n.c_str());
  return *this;
}

IDynamicVisitor &JSONPrinter::endContainer() {
  const bool writing_array = m_state.back().writing_array;
  const bool writing_struct = m_state.back().writing_struct;
  if (writing_array || writing_struct) {
    newline();
  }
  m_state.pop_back();
  namespaces.pop_back();
  if (writing_struct) {
    indent();
    m_os << "}";
  }
  if (writing_array) {
    indent();
    m_os << "]";
  }
  m_state.back().prev_elem = true;
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(float *val, const std::string &name,
                                         const size_t cnt) {
  writePod(val, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(double *ptr, const std::string &name,
                                         const size_t cnt) {
  writePod(ptr, name, cnt);
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(void *ptr, const std::string &name,
                                         const size_t cnt) {
  return *this;
}

IDynamicVisitor &JSONPrinter::operator()(IStructTraits *val,
                                         const std::string &name) {

  if (m_state.back().prev_elem) {
    m_os << ",";
    newline();
  }
  indent();
  if (!m_state.back().writing_array) {
    if (name.empty()) {
      m_os << "value" << m_state.back().unnamed_count;
      ++(m_state.back().unnamed_count);
    } else {
      m_os << name;
    }
    m_os << ":{";
    newline();
  } else {
    m_os << "{";
    newline();
  }

  m_state.back().prev_elem = false;
  IDynamicVisitor::operator()(val, name);
  newline();
  indent();
  m_os << "}";
  m_state.back().prev_elem = true;
  return *this;
}

std::unique_ptr<IDataContainer> &
JSONPrinter::accessCache(const std::string &name) {
  return m_cache[name];
}

void JSONPrinter::prev() {
  if (m_state.back().prev_elem) {
    m_os << ",";
    newline();
  }
}

void JSONPrinter::writeName(const std::string &name) {
  if (name.empty()) {
    m_os << "value" << m_state.back().unnamed_count;
    ++m_state.back().unnamed_count;
  } else {
    m_os << name;
  }
}

void JSONPrinter::indent() {
  if (m_whitespace) {
    for (size_t i = 0; i < namespaces.size() + 1; ++i) {
      m_os << "  ";
    }
  }
}
void JSONPrinter::newline() {
  if (m_whitespace) {
    m_os << "\n";
  }
}
}
