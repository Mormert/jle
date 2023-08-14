// Copyright (c) 2023. Johan Lind

#include "jleFileChangeNotifier.h"
#include "editor/jleEditorTextEdit.h"
#include <editor/jleEditor.h>

#include "jleResource.h"

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
        static std::vector<jlePath> erased;
        static std::vector<jlePath> added;
        static std::vector<jlePath> modified;
        static std::future<void> sortTranslucentAsync;

        if (sortTranslucentAsync.valid() && sortTranslucentAsync.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            for (auto &path : erased) {
                notifyErase(path);
            }
            for (auto &path : added) {
                notifyAdded(path);
            }
            for (auto &path : modified) {
                notifyModification(path);
            }
            erased.clear();
            added.clear();
            modified.clear();
        }

        sortTranslucentAsync = std::async(std::launch::async, [&] { sweep(erased, added, modified); });

        lastSweep = now;
    }
}

void
jleFileChangeNotifier::sweep(std::vector<jlePath> &erased, std::vector<jlePath> &added, std::vector<jlePath> &modified)
{
    auto it = _pathsMonitored.begin();
    while (it != _pathsMonitored.end()) {
        if (!std::filesystem::exists(it->first)) {
            erased.push_back(jlePath{it->first, false});
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
                added.push_back(jlePath{file.path().string(), false});
            } else {
                if (_pathsMonitored[file.path().string()] != current_file_last_write_time) {
                    _pathsMonitored[file.path().string()] = current_file_last_write_time;
                    if (file.is_regular_file()) {
                        modified.push_back(jlePath{file.path().string(), false});
                    }
                }
            }
        }
    }
}

void
jleFileChangeNotifier::notifyAdded(const jlePath &path)
{
    LOGI << "File indexed: " << path.getVirtualPath();
}

void
jleFileChangeNotifier::notifyModification(const jlePath &path)
{
    if (gEngine->resources().isResourceLoaded(path)) {
        LOGI << "File modified: " << path.getVirtualPath() << " (reloading resource)";
        gEngine->resources().resource(path)->loadFromFile(path);

#ifdef BUILD_EDITOR
        gEditor->editorTextEdit().reloadIfOpened(path);
#endif
    } else {
        LOGI << "File modified: " << path.getVirtualPath();
    }
}

void
jleFileChangeNotifier::notifyErase(const jlePath &path)
{
    LOGI << "File erased: " << path.getVirtualPath();
}

bool
jleFileChangeNotifier::contains(const std::string &key)
{
    auto el = _pathsMonitored.find(key);
    return el != _pathsMonitored.end();
}
