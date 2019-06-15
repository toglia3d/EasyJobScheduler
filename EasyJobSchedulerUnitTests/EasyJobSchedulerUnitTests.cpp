#include "pch.h"
#include "CppUnitTest.h"
#include "../EasyJobScheduler/TaskManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EasyJobSchedulerUnitTests
{
	TEST_CLASS(EasyJobSchedulerUnitTests)
	{
	public:

		TEST_METHOD(SuccessCase1)
		{
            task_map map;
            map["A"] = Task("A","echo \'A\'",{});
            map["B"] = Task("B", "echo \'B\'", {});
            map["C"] = Task("C", "echo \'C\'", { {"A"},{"B"} });

            std::string error;
            const auto result = TaskManager::run("C", map, error);
            Assert::AreEqual(result, true);
            Assert::IsTrue(error.compare("") == 0);            
		}
	};
}
