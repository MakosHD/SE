#include "pch.h"
#include "CppUnitTest.h"
#include "../Lab_2/Lab_2.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(VerifyN_Tests)
	{
	public:

		TEST_METHOD(verifyN_give5_5returned)
		{
			std::istringstream input("5");
			Assert::AreEqual(5, verify_n(input));
		}

		TEST_METHOD(verifyN_giveNoiseand10_10returned)
		{
			std::istringstream input("hi\n-5\n0\nabc\n10");
			Assert::AreEqual(10, verify_n(input));
		}

		TEST_METHOD(verifyN_give4dot7and8_8returned)
		{
			std::istringstream input("4.7\n8");
			Assert::AreEqual(8, verify_n(input));
		}

		TEST_METHOD(verifyN_giveLargeNumber_LargeNumberReturned)
		{
			std::istringstream input("9999999");
			Assert::AreEqual(9999999, verify_n(input));
		}
	};

	TEST_CLASS(VerifyLowerLimit_Tests)
	{
	public:

		TEST_METHOD(lower_give3dot14_3dot14returned)
		{
			std::istringstream input("3.14");
			Assert::AreEqual(3.14, verify_lower_limit(input));
		}

		TEST_METHOD(lower_giveNoiseAndMinus2_Minus2returned)
		{
			std::istringstream input("go **** ******** \n.\n-2");
			Assert::AreEqual(-2.0, verify_lower_limit(input));
		}
	};

	TEST_CLASS(VerifyUpperLimit_Tests)
	{
	public:

		TEST_METHOD(upper_give10_10returned)
		{
			std::istringstream input("10");
			Assert::AreEqual(10.0, verify_upper_limit(input, 1.0));
		}

		TEST_METHOD(upper_lower_limit_invalid)
		{
			std::istringstream input("1\n5");
			Assert::AreEqual(5.0, verify_upper_limit(input, 1.0));
		}

		TEST_METHOD(upper_mess_valid)
		{
			std::istringstream input("-5\n0\n10");
			Assert::AreEqual(10.0, verify_upper_limit(input, 1.0));
		}
	};

	TEST_CLASS(VerifyStep_Tests)
	{
	public:

		TEST_METHOD(step_validNumber)
		{
			std::istringstream input("0.5");
			Assert::AreEqual(0.5, verify_step(input));
		}

		TEST_METHOD(step_noise)
		{
			std::istringstream input("aa\n-1\n0\n2");
			Assert::AreEqual(2.0, verify_step( input));
		}
	};

	TEST_CLASS(CalculateProduct_Tests)
	{
	public:

		TEST_METHOD(product_valid)
		{
			double x = 6;
			int n = 10;
			double result = calculate_product(x, n);

			Assert::AreEqual(0.163636,result, 1e-6);
		}
		TEST_METHOD(product_invalid_n)
		{
			auto f = []  { calculate_product(4.0, 5); };
			Assert::ExpectException<std::invalid_argument>(f);
		}
	};

	TEST_CLASS(CalculateProductSum_Tests)
	{
	public:

		TEST_METHOD(productSum_valid)
		{
			double x = 6;
			int n = 8;
			double result = calculate_product(x, n);

			Assert::AreEqual(0.5, result);
		}

		TEST_METHOD(ProductSum_invalid_zero)
		{
			auto f = [] { calculate_product_sum(10.0, 0); };
			Assert::ExpectException<std::invalid_argument>(f);
		}

		TEST_METHOD(sum_invalid_negative)
		{
			auto f = [] { calculate_product_sum(10.0, -5); };
			Assert::ExpectException<std::invalid_argument>(f);
		}
	};

	TEST_CLASS(ActionInputData_Tests){
	public:

		TEST_METHOD(input_valid)
		{
			std::istringstream input("10\n1\n20\n2");

			variables_for_calculations v = action_input_data(input);

			Assert::AreEqual(10, v.n);
			Assert::AreEqual(1.0, v.lower_limit);
			Assert::AreEqual(20.0, v.upper_limit);
			Assert::AreEqual(2.0, v.step);
		}
	};

	TEST_CLASS(ActionCalculate_Tests)
	{
	public:

		TEST_METHOD(calc_valid)
		{
			vector<variables_for_calculations> data;
			variables_for_calculations v;

			v.n = 10;
			v.lower_limit = 1;
			v.upper_limit = 10;
			v.step = 1;

			data.push_back(v);

			v.n = 25;
			v.lower_limit = -1;
			v.upper_limit = 10000;
			v.step = 1;

			data.push_back(v);

			action_calculate(data);

			Assert::IsTrue(!data[0].solutions.empty());
			Assert::IsTrue(!data[1].solutions.empty());
		}

		TEST_METHOD(calc_invalid)
		{
			vector<variables_for_calculations> data;
			variables_for_calculations v;

			v.n = 1;
			v.lower_limit = 1;
			v.upper_limit = 1;
			v.step = 1;

			data.push_back(v);

			action_calculate(data);

			Assert::IsTrue(data[0].error);
		}
	};
}