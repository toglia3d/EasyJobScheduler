#include "pch.h"
#include "CppUnitTest.h"
#include "../EasyJobScheduler/TaskManager.h"
#include "../EasyJobScheduler/TaskExecutor.h"

#include <sstream>
#include <iostream>
#include <cstdio>
#include <fstream>
#include "../EasyJobScheduler/Helpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EasyJobSchedulerUnitTests
{
	TEST_CLASS(EasyJobSchedulerUnitTests)
	{
	public:

        // TestTaskExecutor:
        // Checking that console detects when a command is correct and when it is not
        TEST_METHOD(TestTaskExecutor)
        {
            auto value = TaskExecutor::run(R"(echo "A")");
            Assert::IsTrue(value);

            value = TaskExecutor::run(R"(BadBadNotGood)");
            Assert::IsTrue(!value);
        }

        // TestNormalCase:
        // Checking that all commands are ran in the correct order.
        // Note we're redirecting the console output to a file and checking that 
	    // this file has the correct Task order.
        // This Test will also check if the console works!
		TEST_METHOD(TestNormalCase)
		{
            task_map map;
            map["A"] = Task("A","echo A",{});
            map["B"] = Task("B", "echo B", {});
            map["C"] = Task("C", "echo C", { {"A"},{"B"} });

            const char* TestFileName = "TestNormalCase";

            FILE* file_write;
            freopen_s(&file_write, TestFileName, "a", stdout);
            Assert::IsTrue(file_write != nullptr);
            if(!file_write)
            {
                return;
            }

            std::string error;
            const auto result = TaskManager::run("C", map, error);
            Assert::AreEqual(result, true);
            Assert::IsTrue(error.empty());

            fclose(file_write);

            std::vector<std::string> lines = Helpers::parse_file_lines(TestFileName);

            Assert::IsTrue(lines.size()==3);
            Assert::IsTrue(lines[0] == "A");
            Assert::IsTrue(lines[1] == "B");
            Assert::IsTrue(lines[2] == "C");

            remove(TestFileName);
		}


        // TestDiamondCase:
        // Diamond dependency tree should be allowed.
        // Example:
        // A depends on B and C (order is important)
        // B depends on D
        // C depends on D too! Note both B and C depend on D
	    TEST_METHOD(TestDiamondCase)
        {
            task_map map;
            map["A"] = Task("A", "echo A", { {"B"}, {"C"} });
            map["B"] = Task("B", "echo B", {"D"});
            map["C"] = Task("C", "echo C", {"D"});
            map["D"] = Task("D", "echo D", { });

            const char* TestFileName = "TestDiamondCase";

            FILE* file_write;
            freopen_s(&file_write, TestFileName, "a", stdout);
            Assert::IsTrue(file_write != nullptr);
            if (!file_write)
            {
                return;
            }

            std::string error;
            const auto result = TaskManager::run("A", map, error);
            Assert::AreEqual(result, true);
            Assert::IsTrue(error.empty());

            fclose(file_write);

            std::vector<std::string> lines = Helpers::parse_file_lines(TestFileName);

            Assert::IsTrue(lines.size() == 4);
            Assert::IsTrue(lines[0] == "D");
            Assert::IsTrue(lines[1] == "B");
            Assert::IsTrue(lines[2] == "C");
            Assert::IsTrue(lines[3] == "A");

            remove(TestFileName);
        }

        // TestCircularDependency:
        // Circular dependencies are not allowed!
        // This is bad as the Tasks will loop indefinitely.
        // Example:
        // A depends on B
        // B depends on C
        // C depends on A
        TEST_METHOD(TestCircularDependency)
        {
            task_map map;
            map["A"] = Task("A", "echo A", { {"B"} });
            map["B"] = Task("B", "echo B", { {"C"} });
            map["C"] = Task("C", "echo C", { {"A"}});

            std::string error;
            const auto result = TaskManager::run("A", map, error);
            Assert::IsTrue(!result);
        }

        // TestSelfReference:
        // Self references are not allowed!
        // This is just bad.
        // Example:
        // A depends on A
        TEST_METHOD(TestSelfReference)
        {
            task_map map;
            map["A"] = Task("A", "echo A", { {"A"} });

            std::string error;
            const auto result = TaskManager::run("A", map, error);
            Assert::IsTrue(!result);
        }

	};
}
