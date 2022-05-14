//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
//
// Ice version 3.7.3
//
// <auto-generated>
//
// Generated from file `AprilTagsServer.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#ifndef __AprilTagsServer_h__
#define __AprilTagsServer_h__

#include <IceUtil/PushDisableWarnings.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/ValueF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/StreamHelpers.h>
#include <Ice/Comparable.h>
#include <Ice/Proxy.h>
#include <Ice/Object.h>
#include <Ice/GCObject.h>
#include <Ice/Value.h>
#include <Ice/Incoming.h>
#include <Ice/FactoryTableInit.h>
#include <IceUtil/ScopedArray.h>
#include <Ice/Optional.h>
#include <IceUtil/UndefSysMacros.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 307
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 >= 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 3
#       error Ice patch level mismatch!
#   endif
#endif

#ifdef ICE_CPP11_MAPPING // C++11 mapping

namespace RoboCompAprilTagsServer
{

class AprilTagsServer;
class AprilTagsServerPrx;

}

namespace RoboCompAprilTagsServer
{

struct tag
{
    int id;
    float tx;
    float ty;
    float tz;
    float rx;
    float ry;
    float rz;
    int cx;
    int cy;

    /**
     * Obtains a tuple containing all of the struct's data members.
     * @return The data members in a tuple.
     */
    std::tuple<const int&, const float&, const float&, const float&, const float&, const float&, const float&, const int&, const int&> ice_tuple() const
    {
        return std::tie(id, tx, ty, tz, rx, ry, rz, cx, cy);
    }
};

using tagsList = ::std::vector<tag>;

enum class Mode : unsigned char
{
    YUV422,
    GRAY8,
    RGB888Packet,
    RGB888Planar
};

struct Format
{
    ::RoboCompAprilTagsServer::Mode modeImage;
    int width;
    int height;
    int size;

    /**
     * Obtains a tuple containing all of the struct's data members.
     * @return The data members in a tuple.
     */
    std::tuple<const ::RoboCompAprilTagsServer::Mode&, const int&, const int&, const int&> ice_tuple() const
    {
        return std::tie(modeImage, width, height, size);
    }
};

using Frame = ::std::vector<::Ice::Byte>;

struct Image
{
    ::RoboCompAprilTagsServer::Frame data;
    ::RoboCompAprilTagsServer::Format frmt;
    int timeStamp;

    /**
     * Obtains a tuple containing all of the struct's data members.
     * @return The data members in a tuple.
     */
    std::tuple<const ::RoboCompAprilTagsServer::Frame&, const ::RoboCompAprilTagsServer::Format&, const int&> ice_tuple() const
    {
        return std::tie(data, frmt, timeStamp);
    }
};

using Ice::operator<;
using Ice::operator<=;
using Ice::operator>;
using Ice::operator>=;
using Ice::operator==;
using Ice::operator!=;

}

namespace RoboCompAprilTagsServer
{

class AprilTagsServer : public virtual ::Ice::Object
{
public:

    using ProxyType = AprilTagsServerPrx;

    /**
     * Determines whether this object supports an interface with the given Slice type ID.
     * @param id The fully-scoped Slice type ID.
     * @param current The Current object for the invocation.
     * @return True if this object supports the interface, false, otherwise.
     */
    virtual bool ice_isA(::std::string id, const ::Ice::Current& current) const override;

    /**
     * Obtains a list of the Slice type IDs representing the interfaces supported by this object.
     * @param current The Current object for the invocation.
     * @return A list of fully-scoped type IDs.
     */
    virtual ::std::vector<::std::string> ice_ids(const ::Ice::Current& current) const override;

    /**
     * Obtains a Slice type ID representing the most-derived interface supported by this object.
     * @param current The Current object for the invocation.
     * @return A fully-scoped type ID.
     */
    virtual ::std::string ice_id(const ::Ice::Current& current) const override;

    /**
     * Obtains the Slice type ID corresponding to this class.
     * @return A fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

    virtual tagsList getAprilTags(Image frame, double tagsize, double mfx, double mfy, const ::Ice::Current& current) = 0;
    /// \cond INTERNAL
    bool _iceD_getAprilTags(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    /// \cond INTERNAL
    virtual bool _iceDispatch(::IceInternal::Incoming&, const ::Ice::Current&) override;
    /// \endcond
};

}

namespace RoboCompAprilTagsServer
{

class AprilTagsServerPrx : public virtual ::Ice::Proxy<AprilTagsServerPrx, ::Ice::ObjectPrx>
{
public:

    tagsList getAprilTags(const Image& frame, double tagsize, double mfx, double mfy, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _makePromiseOutgoing<::RoboCompAprilTagsServer::tagsList>(true, this, &AprilTagsServerPrx::_iceI_getAprilTags, frame, tagsize, mfx, mfy, context).get();
    }

    template<template<typename> class P = ::std::promise>
    auto getAprilTagsAsync(const Image& frame, double tagsize, double mfx, double mfy, const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(::std::declval<P<::RoboCompAprilTagsServer::tagsList>>().get_future())
    {
        return _makePromiseOutgoing<::RoboCompAprilTagsServer::tagsList, P>(false, this, &AprilTagsServerPrx::_iceI_getAprilTags, frame, tagsize, mfx, mfy, context);
    }

    ::std::function<void()>
    getAprilTagsAsync(const Image& frame, double tagsize, double mfx, double mfy,
                      ::std::function<void(::RoboCompAprilTagsServer::tagsList)> response,
                      ::std::function<void(::std::exception_ptr)> ex = nullptr,
                      ::std::function<void(bool)> sent = nullptr,
                      const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _makeLamdaOutgoing<::RoboCompAprilTagsServer::tagsList>(response, ex, sent, this, &RoboCompAprilTagsServer::AprilTagsServerPrx::_iceI_getAprilTags, frame, tagsize, mfx, mfy, context);
    }

    /// \cond INTERNAL
    void _iceI_getAprilTags(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<::RoboCompAprilTagsServer::tagsList>>&, const Image&, double, double, double, const ::Ice::Context&);
    /// \endcond

    /**
     * Obtains the Slice type ID of this interface.
     * @return The fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

protected:

    /// \cond INTERNAL
    AprilTagsServerPrx() = default;
    friend ::std::shared_ptr<AprilTagsServerPrx> IceInternal::createProxy<AprilTagsServerPrx>();

    virtual ::std::shared_ptr<::Ice::ObjectPrx> _newInstance() const override;
    /// \endcond
};

}

/// \cond STREAM
namespace Ice
{

template<>
struct StreamableTraits<::RoboCompAprilTagsServer::tag>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 36;
    static const bool fixedLength = true;
};

template<typename S>
struct StreamReader<::RoboCompAprilTagsServer::tag, S>
{
    static void read(S* istr, ::RoboCompAprilTagsServer::tag& v)
    {
        istr->readAll(v.id, v.tx, v.ty, v.tz, v.rx, v.ry, v.rz, v.cx, v.cy);
    }
};

template<>
struct StreamableTraits< ::RoboCompAprilTagsServer::Mode>
{
    static const StreamHelperCategory helper = StreamHelperCategoryEnum;
    static const int minValue = 0;
    static const int maxValue = 3;
    static const int minWireSize = 1;
    static const bool fixedLength = false;
};

template<>
struct StreamableTraits<::RoboCompAprilTagsServer::Format>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 13;
    static const bool fixedLength = false;
};

template<typename S>
struct StreamReader<::RoboCompAprilTagsServer::Format, S>
{
    static void read(S* istr, ::RoboCompAprilTagsServer::Format& v)
    {
        istr->readAll(v.modeImage, v.width, v.height, v.size);
    }
};

template<>
struct StreamableTraits<::RoboCompAprilTagsServer::Image>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 18;
    static const bool fixedLength = false;
};

template<typename S>
struct StreamReader<::RoboCompAprilTagsServer::Image, S>
{
    static void read(S* istr, ::RoboCompAprilTagsServer::Image& v)
    {
        istr->readAll(v.data, v.frmt, v.timeStamp);
    }
};

}
/// \endcond

/// \cond INTERNAL
namespace RoboCompAprilTagsServer
{

using AprilTagsServerPtr = ::std::shared_ptr<AprilTagsServer>;
using AprilTagsServerPrxPtr = ::std::shared_ptr<AprilTagsServerPrx>;

}
/// \endcond

#else // C++98 mapping

namespace IceProxy
{

namespace RoboCompAprilTagsServer
{

class AprilTagsServer;
/// \cond INTERNAL
void _readProxy(::Ice::InputStream*, ::IceInternal::ProxyHandle< AprilTagsServer>&);
::IceProxy::Ice::Object* upCast(AprilTagsServer*);
/// \endcond

}

}

namespace RoboCompAprilTagsServer
{

class AprilTagsServer;
/// \cond INTERNAL
::Ice::Object* upCast(AprilTagsServer*);
/// \endcond
typedef ::IceInternal::Handle< AprilTagsServer> AprilTagsServerPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::RoboCompAprilTagsServer::AprilTagsServer> AprilTagsServerPrx;
typedef AprilTagsServerPrx AprilTagsServerPrxPtr;
/// \cond INTERNAL
void _icePatchObjectPtr(AprilTagsServerPtr&, const ::Ice::ObjectPtr&);
/// \endcond

}

namespace RoboCompAprilTagsServer
{

struct tag
{
    ::Ice::Int id;
    ::Ice::Float tx;
    ::Ice::Float ty;
    ::Ice::Float tz;
    ::Ice::Float rx;
    ::Ice::Float ry;
    ::Ice::Float rz;
    ::Ice::Int cx;
    ::Ice::Int cy;
};

typedef ::std::vector<tag> tagsList;

enum Mode
{
    YUV422,
    GRAY8,
    RGB888Packet,
    RGB888Planar
};

struct Format
{
    ::RoboCompAprilTagsServer::Mode modeImage;
    ::Ice::Int width;
    ::Ice::Int height;
    ::Ice::Int size;

    bool operator==(const Format& rhs_) const
    {
        if(this == &rhs_)
        {
            return true;
        }
        if(modeImage != rhs_.modeImage)
        {
            return false;
        }
        if(width != rhs_.width)
        {
            return false;
        }
        if(height != rhs_.height)
        {
            return false;
        }
        if(size != rhs_.size)
        {
            return false;
        }
        return true;
    }

    bool operator<(const Format& rhs_) const
    {
        if(this == &rhs_)
        {
            return false;
        }
        if(modeImage < rhs_.modeImage)
        {
            return true;
        }
        else if(rhs_.modeImage < modeImage)
        {
            return false;
        }
        if(width < rhs_.width)
        {
            return true;
        }
        else if(rhs_.width < width)
        {
            return false;
        }
        if(height < rhs_.height)
        {
            return true;
        }
        else if(rhs_.height < height)
        {
            return false;
        }
        if(size < rhs_.size)
        {
            return true;
        }
        else if(rhs_.size < size)
        {
            return false;
        }
        return false;
    }

    bool operator!=(const Format& rhs_) const
    {
        return !operator==(rhs_);
    }
    bool operator<=(const Format& rhs_) const
    {
        return operator<(rhs_) || operator==(rhs_);
    }
    bool operator>(const Format& rhs_) const
    {
        return !operator<(rhs_) && !operator==(rhs_);
    }
    bool operator>=(const Format& rhs_) const
    {
        return !operator<(rhs_);
    }
};

typedef ::std::vector< ::Ice::Byte> Frame;

struct Image
{
    ::RoboCompAprilTagsServer::Frame data;
    ::RoboCompAprilTagsServer::Format frmt;
    ::Ice::Int timeStamp;
};

}

namespace RoboCompAprilTagsServer
{

/**
 * Base class for asynchronous callback wrapper classes used for calls to
 * IceProxy::RoboCompAprilTagsServer::AprilTagsServer::begin_getAprilTags.
 * Create a wrapper instance by calling ::RoboCompAprilTagsServer::newCallback_AprilTagsServer_getAprilTags.
 */
class Callback_AprilTagsServer_getAprilTags_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_AprilTagsServer_getAprilTags_Base> Callback_AprilTagsServer_getAprilTagsPtr;

}

namespace IceProxy
{

namespace RoboCompAprilTagsServer
{

class AprilTagsServer : public virtual ::Ice::Proxy<AprilTagsServer, ::IceProxy::Ice::Object>
{
public:

    ::RoboCompAprilTagsServer::tagsList getAprilTags(const ::RoboCompAprilTagsServer::Image& frame, ::Ice::Double tagsize, ::Ice::Double mfx, ::Ice::Double mfy, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return end_getAprilTags(_iceI_begin_getAprilTags(frame, tagsize, mfx, mfy, context, ::IceInternal::dummyCallback, 0, true));
    }

    ::Ice::AsyncResultPtr begin_getAprilTags(const ::RoboCompAprilTagsServer::Image& frame, ::Ice::Double tagsize, ::Ice::Double mfx, ::Ice::Double mfy, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _iceI_begin_getAprilTags(frame, tagsize, mfx, mfy, context, ::IceInternal::dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_getAprilTags(const ::RoboCompAprilTagsServer::Image& frame, ::Ice::Double tagsize, ::Ice::Double mfx, ::Ice::Double mfy, const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_getAprilTags(frame, tagsize, mfx, mfy, ::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_getAprilTags(const ::RoboCompAprilTagsServer::Image& frame, ::Ice::Double tagsize, ::Ice::Double mfx, ::Ice::Double mfy, const ::Ice::Context& context, const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_getAprilTags(frame, tagsize, mfx, mfy, context, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_getAprilTags(const ::RoboCompAprilTagsServer::Image& frame, ::Ice::Double tagsize, ::Ice::Double mfx, ::Ice::Double mfy, const ::RoboCompAprilTagsServer::Callback_AprilTagsServer_getAprilTagsPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_getAprilTags(frame, tagsize, mfx, mfy, ::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_getAprilTags(const ::RoboCompAprilTagsServer::Image& frame, ::Ice::Double tagsize, ::Ice::Double mfx, ::Ice::Double mfy, const ::Ice::Context& context, const ::RoboCompAprilTagsServer::Callback_AprilTagsServer_getAprilTagsPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_getAprilTags(frame, tagsize, mfx, mfy, context, cb, cookie);
    }

    ::RoboCompAprilTagsServer::tagsList end_getAprilTags(const ::Ice::AsyncResultPtr& result);

private:

    ::Ice::AsyncResultPtr _iceI_begin_getAprilTags(const ::RoboCompAprilTagsServer::Image&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::Ice::Context&, const ::IceInternal::CallbackBasePtr&, const ::Ice::LocalObjectPtr& cookie = 0, bool sync = false);

public:

    /**
     * Obtains the Slice type ID corresponding to this interface.
     * @return A fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

protected:
    /// \cond INTERNAL

    virtual ::IceProxy::Ice::Object* _newInstance() const;
    /// \endcond
};

}

}

namespace RoboCompAprilTagsServer
{

class AprilTagsServer : public virtual ::Ice::Object
{
public:

    typedef AprilTagsServerPrx ProxyType;
    typedef AprilTagsServerPtr PointerType;

    virtual ~AprilTagsServer();

    /**
     * Determines whether this object supports an interface with the given Slice type ID.
     * @param id The fully-scoped Slice type ID.
     * @param current The Current object for the invocation.
     * @return True if this object supports the interface, false, otherwise.
     */
    virtual bool ice_isA(const ::std::string& id, const ::Ice::Current& current = ::Ice::emptyCurrent) const;

    /**
     * Obtains a list of the Slice type IDs representing the interfaces supported by this object.
     * @param current The Current object for the invocation.
     * @return A list of fully-scoped type IDs.
     */
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& current = ::Ice::emptyCurrent) const;

    /**
     * Obtains a Slice type ID representing the most-derived interface supported by this object.
     * @param current The Current object for the invocation.
     * @return A fully-scoped type ID.
     */
    virtual const ::std::string& ice_id(const ::Ice::Current& current = ::Ice::emptyCurrent) const;

    /**
     * Obtains the Slice type ID corresponding to this class.
     * @return A fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

    virtual tagsList getAprilTags(const Image& frame, ::Ice::Double tagsize, ::Ice::Double mfx, ::Ice::Double mfy, const ::Ice::Current& current = ::Ice::emptyCurrent) = 0;
    /// \cond INTERNAL
    bool _iceD_getAprilTags(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    /// \cond INTERNAL
    virtual bool _iceDispatch(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

protected:

    /// \cond STREAM
    virtual void _iceWriteImpl(::Ice::OutputStream*) const;
    virtual void _iceReadImpl(::Ice::InputStream*);
    /// \endcond
};

/// \cond INTERNAL
inline bool operator==(const AprilTagsServer& lhs, const AprilTagsServer& rhs)
{
    return static_cast<const ::Ice::Object&>(lhs) == static_cast<const ::Ice::Object&>(rhs);
}

inline bool operator<(const AprilTagsServer& lhs, const AprilTagsServer& rhs)
{
    return static_cast<const ::Ice::Object&>(lhs) < static_cast<const ::Ice::Object&>(rhs);
}
/// \endcond

}

/// \cond STREAM
namespace Ice
{

template<>
struct StreamableTraits< ::RoboCompAprilTagsServer::tag>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 36;
    static const bool fixedLength = true;
};

template<typename S>
struct StreamWriter< ::RoboCompAprilTagsServer::tag, S>
{
    static void write(S* ostr, const ::RoboCompAprilTagsServer::tag& v)
    {
        ostr->write(v.id);
        ostr->write(v.tx);
        ostr->write(v.ty);
        ostr->write(v.tz);
        ostr->write(v.rx);
        ostr->write(v.ry);
        ostr->write(v.rz);
        ostr->write(v.cx);
        ostr->write(v.cy);
    }
};

template<typename S>
struct StreamReader< ::RoboCompAprilTagsServer::tag, S>
{
    static void read(S* istr, ::RoboCompAprilTagsServer::tag& v)
    {
        istr->read(v.id);
        istr->read(v.tx);
        istr->read(v.ty);
        istr->read(v.tz);
        istr->read(v.rx);
        istr->read(v.ry);
        istr->read(v.rz);
        istr->read(v.cx);
        istr->read(v.cy);
    }
};

template<>
struct StreamableTraits< ::RoboCompAprilTagsServer::Mode>
{
    static const StreamHelperCategory helper = StreamHelperCategoryEnum;
    static const int minValue = 0;
    static const int maxValue = 3;
    static const int minWireSize = 1;
    static const bool fixedLength = false;
};

template<>
struct StreamableTraits< ::RoboCompAprilTagsServer::Format>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 13;
    static const bool fixedLength = false;
};

template<typename S>
struct StreamWriter< ::RoboCompAprilTagsServer::Format, S>
{
    static void write(S* ostr, const ::RoboCompAprilTagsServer::Format& v)
    {
        ostr->write(v.modeImage);
        ostr->write(v.width);
        ostr->write(v.height);
        ostr->write(v.size);
    }
};

template<typename S>
struct StreamReader< ::RoboCompAprilTagsServer::Format, S>
{
    static void read(S* istr, ::RoboCompAprilTagsServer::Format& v)
    {
        istr->read(v.modeImage);
        istr->read(v.width);
        istr->read(v.height);
        istr->read(v.size);
    }
};

template<>
struct StreamableTraits< ::RoboCompAprilTagsServer::Image>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 18;
    static const bool fixedLength = false;
};

template<typename S>
struct StreamWriter< ::RoboCompAprilTagsServer::Image, S>
{
    static void write(S* ostr, const ::RoboCompAprilTagsServer::Image& v)
    {
        ostr->write(v.data);
        ostr->write(v.frmt);
        ostr->write(v.timeStamp);
    }
};

template<typename S>
struct StreamReader< ::RoboCompAprilTagsServer::Image, S>
{
    static void read(S* istr, ::RoboCompAprilTagsServer::Image& v)
    {
        istr->read(v.data);
        istr->read(v.frmt);
        istr->read(v.timeStamp);
    }
};

}
/// \endcond

namespace RoboCompAprilTagsServer
{

/**
 * Type-safe asynchronous callback wrapper class used for calls to
 * IceProxy::RoboCompAprilTagsServer::AprilTagsServer::begin_getAprilTags.
 * Create a wrapper instance by calling ::RoboCompAprilTagsServer::newCallback_AprilTagsServer_getAprilTags.
 */
template<class T>
class CallbackNC_AprilTagsServer_getAprilTags : public Callback_AprilTagsServer_getAprilTags_Base, public ::IceInternal::TwowayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)(const tagsList&);

    CallbackNC_AprilTagsServer_getAprilTags(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::TwowayCallbackNC<T>(obj, cb != 0, excb, sentcb), _response(cb)
    {
    }

    /// \cond INTERNAL
    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        AprilTagsServerPrx proxy = AprilTagsServerPrx::uncheckedCast(result->getProxy());
        tagsList ret;
        try
        {
            ret = proxy->end_getAprilTags(result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::CallbackNC<T>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::CallbackNC<T>::_callback.get()->*_response)(ret);
        }
    }
    /// \endcond

private:

    Response _response;
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::RoboCompAprilTagsServer::AprilTagsServer::begin_getAprilTags.
 */
template<class T> Callback_AprilTagsServer_getAprilTagsPtr
newCallback_AprilTagsServer_getAprilTags(const IceUtil::Handle<T>& instance, void (T::*cb)(const tagsList&), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_AprilTagsServer_getAprilTags<T>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::RoboCompAprilTagsServer::AprilTagsServer::begin_getAprilTags.
 */
template<class T> Callback_AprilTagsServer_getAprilTagsPtr
newCallback_AprilTagsServer_getAprilTags(T* instance, void (T::*cb)(const tagsList&), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_AprilTagsServer_getAprilTags<T>(instance, cb, excb, sentcb);
}

/**
 * Type-safe asynchronous callback wrapper class with cookie support used for calls to
 * IceProxy::RoboCompAprilTagsServer::AprilTagsServer::begin_getAprilTags.
 * Create a wrapper instance by calling ::RoboCompAprilTagsServer::newCallback_AprilTagsServer_getAprilTags.
 */
template<class T, typename CT>
class Callback_AprilTagsServer_getAprilTags : public Callback_AprilTagsServer_getAprilTags_Base, public ::IceInternal::TwowayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception& , const CT&);
    typedef void (T::*Sent)(bool , const CT&);
    typedef void (T::*Response)(const tagsList&, const CT&);

    Callback_AprilTagsServer_getAprilTags(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::TwowayCallback<T, CT>(obj, cb != 0, excb, sentcb), _response(cb)
    {
    }

    /// \cond INTERNAL
    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        AprilTagsServerPrx proxy = AprilTagsServerPrx::uncheckedCast(result->getProxy());
        tagsList ret;
        try
        {
            ret = proxy->end_getAprilTags(result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::Callback<T, CT>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::Callback<T, CT>::_callback.get()->*_response)(ret, CT::dynamicCast(result->getCookie()));
        }
    }
    /// \endcond

private:

    Response _response;
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::RoboCompAprilTagsServer::AprilTagsServer::begin_getAprilTags.
 */
template<class T, typename CT> Callback_AprilTagsServer_getAprilTagsPtr
newCallback_AprilTagsServer_getAprilTags(const IceUtil::Handle<T>& instance, void (T::*cb)(const tagsList&, const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_AprilTagsServer_getAprilTags<T, CT>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::RoboCompAprilTagsServer::AprilTagsServer::begin_getAprilTags.
 */
template<class T, typename CT> Callback_AprilTagsServer_getAprilTagsPtr
newCallback_AprilTagsServer_getAprilTags(T* instance, void (T::*cb)(const tagsList&, const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_AprilTagsServer_getAprilTags<T, CT>(instance, cb, excb, sentcb);
}

}

#endif

#include <IceUtil/PopDisableWarnings.h>
#endif
