#include "pch.h"
#include "CppUnitTest.h"
#include "../EasyJobScheduler/TaskManager.h"
#include "../EasyJobScheduler/TaskExecutor.h"

#include <sstream>
#include <iostream>
#include <cstdio>
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EasyJobSchedulerUnitTests
{
	TEST_CLASS(EasyJobSchedulerUnitTests)
	{
	public:

        TEST_METHOD(TestTaskExecutor)
        {
            auto value = TaskExecutor::run(R"(echo "A")");
            Assert::IsTrue(value);

            value = TaskExecutor::run(R"(BadBadNotGood)");
            Assert::IsTrue(!value);
        }

        static std::string slurp(std::ifstream& in) {
            std::stringstream stream;
            stream << in.rdbuf();
            return stream.str();
        }

		TEST_METHOD(TestNormalCase)
		{
            task_map map;
            map["A"] = Task("A","echo A",{});
            map["B"] = Task("B", "echo B", {});
            map["C"] = Task("C", "echo C", { {"A"},{"B"} });

            const char* TestName = "TestNormalCase";
            FILE* file_write;
            freopen_s(&file_write, TestName, "a", stdout);
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

            std::string line;
            std::vector<std::string> lines;
            std::ifstream file_read(TestName);
            if (file_read.is_open())
            {
                while (getline(file_read, line))
                {
                    lines.push_back(line);
                }
                file_read.close();
            }

            Assert::IsTrue(lines.size()==3);
            Assert::IsTrue(lines[0] == "A");
            Assert::IsTrue(lines[1] == "B");
            Assert::IsTrue(lines[2] == "C");
		}

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

        TEST_METHOD(ErrorInScriptTest)
        {
            task_map map;
            map["A"] = Task("A", "echo A", { {"B"} });
            map["B"] = Task("B", "BadBadScript", { });

            std::string error;
            const auto result = TaskManager::run("A", map, error);
            Assert::IsTrue(!result);
        }
	};
}
