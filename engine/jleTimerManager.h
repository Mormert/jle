// Copyright (c) 2022. Johan Lind

#ifndef JLETIMERMANAGER_H
#define JLETIMERMANAGER_H

#include <map>
#include <memory>

class jleTimerManager {

public:
    // Will crash if data is invalid when called, will however extend object
    // lifetime
    void ExecuteFuncInSecondsSharedData(double seconds,
                                        void (*f)(std::shared_ptr<void>),
                                        const std::shared_ptr<void>& data);

    // Will not run if data is invalid when called (no crash), and also not
    // impact object lifetime
    void ExecuteFuncInSecondsWeakData(double seconds,
                                      void (*f)(std::weak_ptr<void>),
                                      const std::weak_ptr<void>& data);

private:
    friend class jleCore;

    friend class jleGameEngine;

    void Process();

    void ClearTimers();

    struct jleTimerManagerFunctionSharedData {
        void (*mFunction)(std::shared_ptr<void>);

        std::shared_ptr<void> mData;
    };

    struct jleTimerManagerFunctionWeakData {
        void (*mFunction)(std::weak_ptr<void>);

        std::weak_ptr<void> mData;
    };

    std::map<double, jleTimerManagerFunctionSharedData> mFunctionsSharedData;

    std::map<double, jleTimerManagerFunctionWeakData> mFunctionsWeakData;
};

#endif // JLETIMERMANAGER_H
