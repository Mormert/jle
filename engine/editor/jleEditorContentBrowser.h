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

#ifdef BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"
#include "jleEditorResourceEdit.h"
#include "jleTexture.h"
#include <filesystem>

class jleEditorTextEdit;

class jleEditorContentBrowser : public jleEditorWindowInterface
{
public:
    explicit jleEditorContentBrowser(const std::string &window_name,
                                     const std::shared_ptr<jleEditorTextEdit> &editorTextEdit,
                                     const std::shared_ptr<jleEditorResourceEdit> &editorResourceEdit);

    void update(jleGameEngine &ge) override;

private:
    std::shared_ptr<jleTexture> _fileIcon;
    std::shared_ptr<jleTexture> _directoryIcon;
    std::shared_ptr<jleTexture> _backDirectoryIcon;
    std::shared_ptr<jleTexture> _sceneFileIcon;
    std::shared_ptr<jleTexture> _imageFileIcon;
    std::shared_ptr<jleTexture> _jsonFileIcon;
    std::shared_ptr<jleTexture> _luaFileIcon;
    std::shared_ptr<jleTexture> _shaderFileIcon;
    std::shared_ptr<jleTexture> _materialFileIcon;
    std::shared_ptr<jleTexture> _objTemplateFileIcon;
    std::shared_ptr<jleTexture> _obj3dFileIcon;

    // Selected directory to show files in
    std::filesystem::path _selectedDirectory;

    // File selected for File Popup
    std::filesystem::path _fileSelected;

    std::pair<bool, uint32_t> directoryTreeViewRecursive(const std::filesystem::path &path,
                                                         uint32_t *count,
                                                         int *selection_mask);

    void contentHierarchy(std::string directoryPath, const std::string &folderName);

    void contentBrowser();

    void selectedFilePopup(std::filesystem::path &file);

    void selectedFilePopupScene(std::filesystem::path &file);

    void selectedFilePopupObjectTemplate(std::filesystem::path &file);

    void openAsText(std::filesystem::path &file);

    void openAsResource(std::filesystem::path &file);

    std::shared_ptr<jleEditorTextEdit> _editorTextEdit;

    std::shared_ptr<jleEditorResourceEdit> _editorResourceEdit;

    std::unordered_map<jlePath, std::shared_ptr<jleTexture>> _referencedTextures;
};

#endif // BUILD_EDITOR
