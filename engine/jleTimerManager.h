/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include <map>
#include <memory>

class jleTimerManager {

public:
    // Will crash if data is invalid when called, will however extend object
    // lifetime
    void executeFuncInSecondsSharedData(double seconds,
                                        void (*f)(std::shared_ptr<void>),
                                        const std::shared_ptr<void> &data);

    // Will not run if data is invalid when called (no crash), and also not
    // impact object lifetime
    void executeFuncInSecondsWeakData(double seconds,
                                      void (*f)(std::weak_ptr<void>),
                                      const std::weak_ptr<void> &data);

private:
    friend class jleGameEngine;

    void process();

    void clearTimers();

    struct jleTimerManagerFunctionSharedData {
        void (*_function)(std::shared_ptr<void>);

        std::shared_ptr<void> _data;
    };

    struct jleTimerManagerFunctionWeakData {
        void (*_function)(std::weak_ptr<void>);

        std::weak_ptr<void> _data;
    };

    std::map<double, jleTimerManagerFunctionSharedData> _functionsSharedData;

    std::map<double, jleTimerManagerFunctionWeakData> _functionsWeakData;
};
