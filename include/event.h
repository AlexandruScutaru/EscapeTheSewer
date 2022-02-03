#ifndef EVENT_H
#define EVENT_H

template<typename T>
class Event {
public:
    typedef void (T::*Func)(void);

    Event() {}
    Event(T* obj, Func func)
        : mObj(obj)
        , mFunc(func)
    {}

    operator bool() const {
        return mObj != nullptr;
    }

    void emit() {
        if (mObj && mFunc)
            (mObj->*mFunc)();
    }

private:
    T* mObj{ nullptr };
    Func mFunc{ nullptr };

};

#endif // EVENT_H
