#include "../reflect/Data.hpp"
#include "../reflect/Reflect.hpp"
#include "../reflect/common.hpp"

#include <ct/concepts.hpp>
#include <ct/concepts/tensor.hpp>
#define BOOST_NO_AUTO_PTR
#include <boost/python.hpp>
#undef BOOST_NO_AUTO_PTR
#include <ct/interop/boost_python/PythonConverter.hpp>
#include <ct/interop/boost_python/ReflectedConverter.hpp>
#include <ct/interop/boost_python/TensorConverter.hpp>

struct PythonTester
{
    template <class T>
    void test(const T&)
    {
        ct::registerToPython<T>();
    }
};

BOOST_PYTHON_MODULE(test_ct_python)
{
#ifdef HAVE_EIGEN
    static_assert(ct::conceptCheck<Eigen::Matrix3f, ct::TensorConcept>(), "Expect matrix to match tensor concept");
#endif
    static_assert(!ct::conceptCheck<TestA, ct::TensorConcept>(), "Expect matrix to not match tensor concept");

    PythonTester tester;
    testTypes(tester);
}
