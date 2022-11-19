#pragma once

#include "String.h"
#include <fstream>

#include "Pointers.h"

class EXPORT File
{
public:
	class EXPORT Reader
	{
	public:
		static Shared<Reader> open(const Path& path);

		String read(uint lenght);
		String read_all();
		String read_line();
		bool is_end_of_file() const;
		uint get_size() const;
		uint64 get_position() const;
		void set_position(uint64 position) const;
		void move_position(int64 offset) const;

	private:
		Reader() {};

		mutable std::ifstream stream_;
		uint64 size_;
	};

	class EXPORT Writer
	{
	public:
		static Shared<Writer> open(const Path& path);

		void write(const String& data);

		void close();

	private:
		Writer() {};

		std::ofstream stream_;
	};

	virtual ~File() = delete;

	static String read_file(const Path& path);
	static void write_file(const Path& path, const String& data = "");
	static void append_file(const Path& path, const String& data);

	static bool exists(const String& path);
};
