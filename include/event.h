#ifndef EVENT_H
#define EVENT_H

template<typename T>
class Event {
    public:
        Event() {}
        Event(T* obj) : mObj(obj) {}

        operator bool() const {
            return mObj != nullptr;
        }

        void operator() () {
            if (mObj) mObj->fire();
        }

    private:
        T* mObj = nullptr;

};

#endif // EVENT_H
