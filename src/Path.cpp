#include "../include/Path.h"

#include <filesystem>

#include "../include/Regex.h"

Path Path::app_path = Path();

Path::Path()
    : parent("")
    , filename(".")
    , extension("")
    , is_global_(false)
{
}

Path::Path(const String& path_str)
{
	setup_from_string(path_str);
}

Path::Path(const char* str)
	: Path(String(str))
{
}

void Path::init(const String& executable_path)
{
	if (app_path.parent == "")
	{
		app_path.setup_from_string(executable_path);
	}
	else
	{
		
	}
}

bool Path::exists() const
{
	return std::filesystem::exists(get_absolute_string().c());
}

bool Path::is_global() const
{
	return is_global_;
}

void Path::create() const
{
	std::filesystem::create_directories(get_absolute_string().c());
}

Path Path::up(uint levels) const
{
	const auto separators = to_string().find('/');

	if (levels >= separators.length())
	{
		if (is_global_)
		{
			return Path(parent.substring(0, separators[0] + 1));
		}
		else
		{
			return Path(String("../") * (levels - separators.length()));
		}
	}
	else
	{
		return Path(parent.substring(0, separators[separators.length() - levels] + 1));
	}
}

Path Path::get_child(const String& child) const
{
	return Path(to_string() + '/' + child);
}

Path Path::get_absolute() const
{
	return Path(get_absolute_string());
}

List<Path> Path::list() const
{
	std::vector<Path> result;

	if (exists())
	{
		for (const auto& entry : std::filesystem::directory_iterator(get_absolute_string().c()))
		{
			result.push_back(Path(entry.path().string()));
		}
	}

	return result;
}

String Path::get_extension() const
{
	return filename + extension;
}

Path Path::with_extension(const String& new_extension)
{
	Path result = *this;
	result.extension = new_extension;
	return result;
}

String Path::sanitize(const String& path_string)
{
	String src = path_string.replace('\\', '/');
	while (src.contains("/./")) src = src.replace("/./", "/");

	return src;
}

Path Path::operator+(const Path& rhs) const
{
	return Path(to_string() + '/' + rhs.to_string());
}

String Path::to_string() const
{
	if (parent.is_empty())
	{
		return filename + extension;
	}
	else
	{
		return parent + '/' + filename + extension;
	}
}

String Path::get_absolute_string() const
{
	return !is_global_ ? (app_path.parent + "/" + to_string()) : to_string();
}

EPathType Path::get_type() const
{
	const auto path = std::filesystem::path(get_absolute_string().c());

	if (is_directory(path)) return EPathType::Directory;
	if (is_regular_file(path)) return EPathType::Regular;
	if (is_block_file(path)) return EPathType::Block;
	if (is_character_file(path)) return EPathType::Character;
	if (is_fifo(path)) return EPathType::Fifo;
	if (is_other(path)) return EPathType::Other;
	if (is_socket(path)) return EPathType::Socket;
	if (is_symlink(path)) return EPathType::Symlink;

	return EPathType::None;
}

void Path::write_to_stream(std::ostream& stream) const
{
	get_absolute_string().write_to_stream(stream);
}

void Path::read_from_stream(std::istream& stream)
{
	setup_from_string(StreamUtils::read<String>(stream));
}

void Path::setup_from_string(const String& string)
{
	if (!string.is_empty())
	{
		std::filesystem::path path(sanitize(string).c());

		parent = path.parent_path().string();
		filename = path.stem().string();
		extension = path.extension().string();

		if (filename.is_empty() && extension.is_empty())
		{
			filename = ".";
		}

		is_global_ = Regex("^([A-Z]:/|/).*").check(parent);
	}
	else
	{
		parent = "";
		filename = "";
		extension = "";
		is_global_ = false;
	}
}
