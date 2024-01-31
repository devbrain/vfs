#ifndef UNITTEST_SANDBOX_HH
#define UNITTEST_SANDBOX_HH

#include <filesystem>

class sandbox {
 public:
	sandbox ();
	~sandbox ();

	[[nodiscard]] std::string root () const noexcept;

	void mkdir (const std::string& pth);
	void create_file (const std::string& path, const std::string& text);
 private:
	std::filesystem::path _root;
};

#endif
