#pragma once

#include <functional>

#include "Pointers.h"

class Path;

struct FileExtension
{
    String ext;
    String description;
};

struct FileDialogInfo
{
    List<FileExtension> extensions = { {"*.*", "All files"} };
};

class EXPORT SystemIO
{
public:
    static Shared<Path> show_save_file_dialog(const FileDialogInfo& dialog_info = FileDialogInfo());
    static Shared<Path> show_save_file_dialog(const Path& default_path, const FileDialogInfo& dialog_info = FileDialogInfo());
    
    static Shared<Path> show_open_file_dialog(const FileDialogInfo& dialog_info = FileDialogInfo());
    static Shared<Path> show_open_file_dialog(const Path& default_path, const FileDialogInfo& dialog_info = FileDialogInfo());
    
private:
    static Shared<Path> show_save_file_dialog_inner(const String& default_path, const FileDialogInfo& dialog_info);
    static Shared<Path> show_open_file_dialog_inner(const String& default_path, const FileDialogInfo& dialog_info);
};
