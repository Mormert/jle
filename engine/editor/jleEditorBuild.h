// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"

#include "jlePath.h"

#include "jleString.h"
#include "jleVectorSet.h"

#include <libzippp.h>

#include <atomic>

class jleEditorBuild : public jleEditorWindowInterface, public libzippp::ZipProgressListener
{
public:
    explicit jleEditorBuild(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    void progression(double p) override;

    int cancel(void) override;

private:

    void testPackagedResources();

    void packageResources(jleVectorSet<jlePath> indexedFiles);
    std::atomic<double> _packagingProgress{};
};

#endif // BUILD_EDITOR