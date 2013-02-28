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
1.����service����Ӧ��operation.
2.thrift����ÿ��operation��Ӧ��request��response�Ľṹ��
3.����rpc service����
4.�̳�IRpcEventHandlerʵ��һ���ࡣ
    ʵ��������handle_rpc_requst��handle_rpc_response����    
5.����һ����Ӧ��stage.

*/

#endif //ZONDA_COMMON_IEVENTSINK_H_
