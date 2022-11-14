#include "../include/base_lib/SystemIO.h"

#include "../include/base_lib/framework.h"
#include "../include/base_lib/Math.h"

#if PLATFORM_WINDOWS
#include <commdlg.h>
#endif

Shared<Path> SystemIO::show_save_file_dialog(const FileDialogInfo& dialog_info)
{
    return show_save_file_dialog_inner("", dialog_info);
}

Shared<Path> SystemIO::show_save_file_dialog(const Path& default_path, const FileDialogInfo& dialog_info)
{
    return show_save_file_dialog_inner(default_path.get_absolute_string(), dialog_info);
}

Shared<Path> SystemIO::show_open_file_dialog(const FileDialogInfo& dialog_info)
{
    return show_open_file_dialog_inner("", dialog_info);
}

Shared<Path> SystemIO::show_open_file_dialog(const Path& default_path, const FileDialogInfo& dialog_info)
{
    return show_open_file_dialog_inner(default_path.get_absolute_string(), dialog_info);
}

Shared<Path> SystemIO::show_save_file_dialog_inner(const String& default_path, const FileDialogInfo& dialog_info)
{
#if PLATFORM_WINDOWS
    OPENFILENAMEA info;
    memset(&info, 0, sizeof(info));
    info.lStructSize = sizeof(info);
    //info.hwndOwner = glfwGetWin32Window(Game::instance_->window_);
    info.Flags = OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    info.lpstrDefExt = "*";

    if (dialog_info.extensions.length() > 0)
    {
        List<String> filters;
        uint filters_len = 0;
        for (auto& extension : dialog_info.extensions)
        {
            auto filter = String::format("%s (%s)", extension.description.c(), extension.ext.c()) + '\0' + extension.ext + '\0';
            filters_len += filter.length();
            filters += filter;
        }
        const auto filter_str = new char[filters_len + 1];
        uint offset = 0;
        for (auto& filter : filters)
        {
            memcpy(filter_str + offset, filter.c(), filter.length());
            offset += filter.length();
        }
        filter_str[filters_len] = '\0';
        info.lpstrFilter = filter_str;
    }

    char filename[MAX_PATH];
    memset(filename, 0, MAX_PATH);
    if (!default_path.is_empty())
    {
        memcpy(filename, default_path.c(), Math::min(default_path.length(), static_cast<uint>(MAX_PATH)));
    }

    info.lpstrFile = filename;
    info.nMaxFile = MAX_PATH;
    
    if (GetSaveFileNameA(&info))
    {
        return MakeShared<Path>(info.lpstrFile);
    }
#endif

    return nullptr;
}

Shared<Path> SystemIO::show_open_file_dialog_inner(const String& default_path, const FileDialogInfo& dialog_info)
{
#if PLATFORM_WINDOWS
    OPENFILENAMEA info;
    ZeroMemory(&info, sizeof(info));
    info.lStructSize = sizeof(info);
    //info.hwndOwner = glfwGetWin32Window(Game::instance_->window_);
    info.Flags = OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    info.lpstrDefExt = "*";

    if (dialog_info.extensions.length() > 0)
    {
        List<String> filters;
        uint filters_len = 0;
        for (auto& extension : dialog_info.extensions)
        {
            auto filter = String::format("%s (%s)", extension.description.c(), extension.ext.c()) + '\0' + extension.ext + '\0';
            filters_len += filter.length();
            filters += filter;
        }
        const auto filter_str = new char[filters_len + 1];
        uint offset = 0;
        for (auto& filter : filters)
        {
            memcpy(filter_str + offset, filter.c(), filter.length());
            offset += filter.length();
        }
        filter_str[filters_len] = '\0';
        info.lpstrFilter = filter_str;
    }

    char filename[MAX_PATH];
    memset(filename, 0, MAX_PATH);
    if (!default_path.is_empty())
    {
        memcpy(filename, default_path.c(), Math::min(default_path.length(), static_cast<uint>(MAX_PATH)));
    }
    
    info.lpstrFile = filename;
    info.nMaxFile = MAX_PATH;
    
    if (GetOpenFileNameA(&info))
    {
        return MakeShared<Path>(info.lpstrFile);
    }
#endif

    return nullptr;
}
