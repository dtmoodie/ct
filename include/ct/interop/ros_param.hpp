#ifndef CT_ROS_PARAM_HPP
#define CT_ROS_PARAM_HPP

namespace ros
{
    class NodeHandle;
}

namespace ct
{
    template <class T>
    void readParams(T& data, ros::NodeHandle& nh, const std::string& path = "");

    template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA>
    auto
    readParam(MemberObjectPointer<DTYPE CTYPE::*, FLAGS, void> ptr, CTYPE& data, ros::NodeHandle& nh, std::string path)
        -> EnableIf<!Reflect<typename std::decay<DTYPE>::type>::SPECIALIZED>
    {
        if (!path.empty())
        {
            path += '.';
        }
        path += ptr.m_name.toString();
        if (FLAGS & REQUIRED)
        {
            set(ptr, data) = nh.param<DTYPE>(path);
        }
        else
        {
            set(ptr, data) = nh.param<DTYPE>(path, get(ptr, data));
        }
    }

    template <class DTYPE, class CTYPE, Flag_t FLAGS, class METADATA>
    auto
    readParam(MemberObjectPointer<DTYPE CTYPE::*, FLAGS, void> ptr, CTYPE& data, ros::NodeHandle& nh, std::string path)
        -> EnableIf<Reflect<typename std::decay<DTYPE>::type>::SPECIALIZED>
    {
        if (!path.empty())
        {
            path += '.';
        }
        path += ptr.m_name.toString();

        readParams(set(ptr, data), nh, path);
    }

    template <class T>
    void readParamsRecurse(T& data, ros::NodeHandle& nh, const std::string& path, Indexer<0> idx)
    {
        const auto ptr = ct::Reflect<T>::getPtr(idx);
        readParam(ptr, nh, path);
    }

    template <class T, index_t I>
    void readParamsRecurse(T& data, ros::NodeHandle& nh, const std::string& path, Indexer<I> idx)
    {
        const auto ptr = ct::Reflect<T>::getPtr(idx);
        readParam(ptr, nh, path);
        readParamsRecurse(data, nh, path, --idx);
    }

    template <class T>
    void readParams(T& data, ros::NodeHandle& nh, const std::string& path)
    {
        readParamsRecurse(data, nh, path, Reflect<T>::end());
    }
}

#endif // CT_ROS_PARAM_HPP
