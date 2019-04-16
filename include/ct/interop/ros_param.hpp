#ifndef CT_ROS_PARAM_HPP
#define CT_ROS_PARAM_HPP
#include <ct/reflect.hpp>
#include <ct/reflect/visitor.hpp>
#include <ct/type_traits.hpp>
#include <ros/node_handle.h>

namespace ct
{
    namespace ros
    {
        struct RosVisitorParams : public ct::DefaultVisitorParams
        {
            constexpr static const bool VISIT_OBJECTS = true;
            constexpr static const bool VISIT_PROPERTIES = false;

            constexpr static const bool SERIALIZER = true;

            constexpr static const char* PREFIX = "";
            constexpr static const char* DELIMINATOR = "/";

            constexpr static const bool ACCUMULATE_PATH = true;

            template <class T>
            constexpr static bool visitMemberFunctions(T*)
            {
                return false;
            }
            template <class T>
            constexpr static bool visitMemberFunction(T*)
            {
                return false;
            }
            template <class T>
            constexpr static bool visitStaticFunctions(T*)
            {
                return false;
            }
            template <class T>
            constexpr static bool visitStaticFunction(T*)
            {
                return false;
            }
            template <class T>
            constexpr static bool visitMemberObject(T*)
            {
                return true;
            }
            template <class T>
            constexpr static bool visitProperty(T*)
            {
                return false;
            }
        };

        struct ParamVisitor : public ct::VisitorBase<ParamVisitor, RosVisitorParams>
        {
            template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA, class T, index_t I>
            ct::EnableIfReflected<DTYPE> visitMemberObject(T& obj,
                                                           std::string path,
                                                           MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA> ptr,
                                                           Indexer<I>,
                                                           ::ros::NodeHandle& nh)

            {
                if (path.empty())
                {
                    path = ptr.getName().toString();
                }
                else
                {
                    path += '/';
                    path += ptr.getName().toString();
                }
                visit(ptr.set(obj), path, nh);
            }

            template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA, class T, index_t I>
            ct::DisableIfReflected<DTYPE> visitMemberObject(T& obj,
                                                            std::string path,
                                                            MemberObjectPointer<DTYPE CTYPE::*, FLAGS, METADATA> ptr,
                                                            Indexer<I>,
                                                            ::ros::NodeHandle& nh)

            {
                if (path.empty())
                {
                    path = ptr.getName().toString();
                }
                else
                {
                    path += '/';
                    path += ptr.getName().toString();
                }
                if (FLAGS & REQUIRED)
                {
                    DTYPE value;
                    const bool success = nh.getParam(path, value);
                    assert(success);
                    ptr.set(obj, value);
                }
                else
                {
                    ptr.set(obj, nh.template param<DTYPE>(path, ptr.get(obj)));
                }
            }
        };

        template <class T>
        void readParams(T& params, ::ros::NodeHandle& nh)
        {
            ct::ros::ParamVisitor visitor;
            visitor.visit(params, "", nh);
        }
    }
}

#endif // CT_ROS_PARAM_HPP
