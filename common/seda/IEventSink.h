#ifndef ZONDA_COMMON_IEVENTSINK_H_
#define ZONDA_COMMON_IEVENTSINK_H_

#include "IEvent.h"
namespace zonda
{
namespace common
{

class IEventSink
{
public:
    //! Put an event into the queue, if the queue is full, 
    //! this function will block until the queue is not full
    //! and put the event into the queue.
    virtual void enqueue(const IEvent* event) = 0;
    
    //! Returns the number of elements in the queue
    virtual size_t size() = 0;

    virtual ~IEventSink() {}
};

}//end namespace common
} //end namespace zonda

/*
1.定义service及对应的operation.
2.thrift定义每个operation对应的request和response的结构。
3.创建rpc service对象。
4.继承IRpcEventHandler实现一个类。
    实现这个类的handle_rpc_requst和handle_rpc_response方法    
5.创建一个对应的stage.

*/

#endif //ZONDA_COMMON_IEVENTSINK_H_
