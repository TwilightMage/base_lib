#include "base_lib/File.h"

String File::read_file(const Path& path)
{
	std::ifstream file(path.get_absolute_string().std(), std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	const auto file_size = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;
}

void File::write_file(const Path& path, const String& data)
{
	std::ofstream file(path.get_absolute().to_string().std());

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	file << data;

	file.close();
}

void File::append_file(const Path& path, const String& data)
{
	std::ofstream file(path.get_absolute().to_string().std(), std::ios::app);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	file << data;

	file.close();
}

bool File::exists(const String& path)
{
	struct stat buffer;
	return (stat(path.c(), &buffer) == 0);
}

Shared<File::Reader> File::Reader::open(const Path& path)
{
	auto result = MakeSharedInternal(File::Reader);

	result->stream_ = std::ifstream(path.get_absolute().to_string().std(), std::ios::ate);

	if (!result->stream_.is_open())
	{
		return nullptr;
	}

	result->size_ = result->stream_.tellg();
	result->stream_.seekg(0);

	return result;
}

String File::Reader::read(uint lenght)
{
	const auto buff_size = std::min(size_ - stream_.tellg(), static_cast<uint64>(lenght));
	const auto buff = new char[buff_size];
	stream_.read(buff, buff_size);
	return String(buff, static_cast<uint>(buff_size));
}

String File::Reader::read_all()
{
	const auto buff_size = size_ - stream_.tellg();
	const auto buff = new char[buff_size];
	stream_.read(buff, buff_size);
	return String(buff, static_cast<uint>(buff_size));
}

String File::Reader::read_line()
{
	std::string line;
	if (!stream_.eof() && std::getline(stream_, line))
	{
		return String(line);
	}

	return "";
}

bool File::Reader::is_end_of_file() const
{
	return stream_.tellg() == size_;
}

uint File::Reader::get_size() const
{
	return static_cast<uint>(size_);
}

uint64 File::Reader::get_position() const
{
	return stream_.tellg();
}

void File::Reader::set_position(uint64 position) const
{
	stream_.seekg(position);
}

void File::Reader::move_position(int64 offset) const
{
	stream_.seekg(offset, std::ios_base::cur);
}

Shared<File::Writer> File::Writer::open(const Path& path)
{
	auto result = MakeSharedInternal(File::Writer);

	result->stream_ = std::ofstream(path.get_absolute().to_string().std());

	if (!result->stream_.is_open())
	{
		result = nullptr;
	}

	return result;
}

void File::Writer::write(const String& data)
{
	stream_ << data;
}

void File::Writer::close()
{
	stream_.close();
}