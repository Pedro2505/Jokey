#include <string>
#include <set>

class ReservedWords
{
	public:
		ReservedWords();

		bool isReserved(const std::string& word) const;

	private:
		std::set<std::string> reservedWords;
};

