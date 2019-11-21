#ifndef SAVER_HPP
#define SAVER_HPP

namespace Frosty
{
	class Saver
	{
	protected:
		Saver() = default;
	public:
		virtual ~Saver() = default;

		static void SaveAll();

	private:


	};
}

#endif // !SAVER_HPP
