#include <QtTest>

#include "test_main.hpp"

int main(int argc, char** argv)
{
	int status       = 0;
	auto ASSERT_TEST = [&status, argc, argv](QObject* obj) {
		status |= QTest::qExec(obj, argc, argv);
		delete obj;
	};

	test_main(ASSERT_TEST);

	return status;
}
