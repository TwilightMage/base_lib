#pragma once

#include "Pointers.h"
#include "String.h"

class Module;

enum class EPathType
{
	None,
    Block,
	Character,
	Directory,
	Fifo,
	Other,
	Regular,
	Socket,
	Symlink
};

IMPLEMENT_ENUM(EPathType)

class EXPORT Path : public ISerializable
{
public:
	Path();
	Path(const String& path_str);
	Path(const char* str);

	static void init(const String& executablePath);
	FORCEINLINE static const Path& get_app_path() { return app_path; }

    bool exists() const;
	bool is_global() const;
	void create_dir() const;
	void create_file() const;
	Path up(uint levels = 1) const;
	Path get_child(const String& child) const;
	Path get_absolute() const;
	List<Path> list() const;
	String get_extension() const;
	Path with_extension(const String& new_extension);

	static String sanitize(const String& path_string);

	Path operator+(const Path& rhs) const;

	String to_string() const;
	String get_absolute_string() const;

	EPathType get_type() const;

	void write_to_stream(std::ostream& stream) const override;
	void read_from_stream(std::istream& stream) override;
	
	String parent;
	String filename;
	String extension;

private:
	void setup_from_string(const String& string);
	
	bool is_global_;

	static Path app_path;
};