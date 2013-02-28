#ifndef ZONDA_COMMON_EVENT_H_
#define ZONDA_COMMON_EVENT_H_

namespace zonda
{
namespace common
{

struct IEvent
{
    virtual int get_type() const = 0;
    virtual ~IEvent() {};
};




}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_EVENT_H_
