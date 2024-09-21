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

#include "jleEditorBuild.h"

#include "core/jleResourceIndexer.h"
#include "jleEditor.h"
#include "jleScopeProfileLog.h"

#include "3rdparty/WickedEngine/wiJobSystem.h"

#include <fstream>

jleEditorBuild::jleEditorBuild(const std::string &window_name) : jleEditorWindowInterface(window_name) {}
void

jleEditorBuild::renderUI(jleEngineModulesContext &ctx, jleResourceIndexer& resourceIndexer)
{
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    static wi::jobsystem::context packagingContext;
    if (IsBusy(packagingContext)) {
        ImGui::Text("Packaging job in progress (%2f %%)\nDon't close the editor during this process...",
                    _packagingProgress * 100.0);
    } else {
        auto &indexedFiles = resourceIndexer.getIndexedFiles();

        ImGui::Text("Amount of items to package: %zu", indexedFiles.size());
        if (ImGui::Button("Package Resources")) {
            wi::jobsystem::Execute(packagingContext,
                                   [&](wi::jobsystem::JobArgs args) { packageResources(indexedFiles); });
        }

        if (ImGui::Button("Test Package")) {
            testPackagedResources();
        }
    }

    ImGui::End();
}

void
jleEditorBuild::packageResources(jleVectorSet<jlePath> indexedFiles)
{
    auto archive = libzippp::ZipArchive("gamedata.zip");

    archive.addProgressListener(this);
    archive.setProgressPrecision(0.01);
    archive.setErrorHandlerCallback(
        [](const std::string &message, const std::string &strerror, int zip_error_code, int system_error_code) {
            LOGE << message << ", " << strerror;
        });
    archive.open(libzippp::ZipArchive::New);

    int ctr = 0;
    for (auto &path : indexedFiles) {
        archive.addFile(path.getVirtualPath().c_str(), path.getRealPath().c_str());

        if (ctr > 800) {
            break;
        }
        ctr++;
    }

    // Here is where the actual zipping process takes place.
    archive.close();
}

void
jleEditorBuild::progression(double p)
{
    _packagingProgress = p;
}

int
jleEditorBuild::cancel(void)
{
    return 0;
}

void
jleEditorBuild::testPackagedResources()
{
    auto archive = libzippp::ZipArchive("gamedata.zip");

    archive.addProgressListener(this);
    archive.setProgressPrecision(0.01);
    archive.setErrorHandlerCallback(
        [](const std::string &message, const std::string &strerror, int zip_error_code, int system_error_code) {
            LOGE << message << ", " << strerror;
        });

    if (!archive.open(libzippp::ZipArchive::ReadOnly)) {
        LOGE << "Failed opening gamedata.zip";
    }

    LOGI << archive.getEntriesCount();

    {
        JLE_SCOPED_PROFILE_LOG("Using Zip");
        for (const auto &thing : archive.getEntries()) {
            std::ofstream stream;
            if (!thing.readAsBinary()) {
                continue;
            }
            std::stringstream buffer;
            buffer << stream.rdbuf();
            auto src = buffer.str();
        }
    }

    {
        JLE_SCOPED_PROFILE_LOG("Using FS");
        for (const auto &thing : archive.getEntries()) {
            auto realPath = jlePath{thing.getName()}.getRealPath();
            std::ifstream load{realPath, std::ios::binary};
            if (!load.good()) {
                continue;
            }

            std::stringstream buffer;
            buffer << load.rdbuf();
            //auto src = buffer.str();
        }
    }
}
