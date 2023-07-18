// Copyright (c) 2023. Johan Lind

#include "jleFileChangeNotifier.h"
#include "editor/jleEditorTextEdit.h"
#include <editor/jleEditor.h>

jleFileChangeNotifier::jleFileChangeNotifier(const std::vector<std::string> &directories)
{
    _directories = directories;
}

void
jleFileChangeNotifier::periodicSweep()
{
    using namespace std::chrono;

    long long now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    if (now > lastSweep + 250ll) {
        sweep();
        lastSweep = now;
    }
}

void
jleFileChangeNotifier::sweep()
{
    auto it = _pathsMonitored.begin();
    while (it != _pathsMonitored.end()) {
        if (!std::filesystem::exists(it->first)) {
            notifyErase(jlePath{it->first, false});
            it = _pathsMonitored.erase(it);
        } else {
            it++;
        }
    }

    for (auto &dir : _directories) {

        for (auto &file : std::filesystem::recursive_directory_iterator(dir)) {
            auto current_file_last_write_time = std::filesystem::last_write_time(file);

            if (!contains(file.path().string())) {
                _pathsMonitored[file.path().string()] = current_file_last_write_time;
                notifyAdded(jlePath{file.path().string(), false});
            } else {
                if (_pathsMonitored[file.path().string()] != current_file_last_write_time) {
                    _pathsMonitored[file.path().string()] = current_file_last_write_time;
                    if (file.is_regular_file()) {
                        notifyModification(jlePath{file.path().string(), false});
                    }
                }
            }
        }
    }
}

void
jleFileChangeNotifier::notifyAdded(const jlePath &path)
{
    LOGI << "File indexed: " << path;
}

void
jleFileChangeNotifier::notifyModification(const jlePath &path)
{
    if (gEngine->resources().isResourceLoaded(path)) {
        LOGI << "File modified: " << path << " (reloading resource)";
        gEngine->resources().resource(path)->loadFromFile(path);

        gEditor->editorTextEdit().reloadIfOpened(path);
    } else {
        LOGI << "File modified: " << path;
    }
}

void
jleFileChangeNotifier::notifyErase(const jlePath &path)
{
    LOGI << "File erased: " << path;
}

bool
jleFileChangeNotifier::contains(const std::string &key)
{
    auto el = _pathsMonitored.find(key);
    return el != _pathsMonitored.end();
}
