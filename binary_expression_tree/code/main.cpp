// your solution comes here
#include "binexptree.hpp"
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "woodpecker.hpp"
#include <iostream>

TEST("infix-postfix conversion with addition", 1) {
	// check if infix-postfix conversion gives back the correct result
  {
		BinExpTree tree = BinExpTree("1.0+2");
		auto vec = tree.parseExpression("1.0+2");
		CHECK_EQ("1.0", vec[0]);
		CHECK_EQ("2", vec[1]);
		CHECK_EQ("+", vec[2]);
  }
}


TEST("infix-postfix conversion with multiplication", 1) {
	// check if infix-postfix conversion gives back the correct result
  {
		BinExpTree tree = BinExpTree("1.0+2*3");
		auto vec = tree.parseExpression("1.0+2*3");
		CHECK_EQ("1.0", vec[0]);
		CHECK_EQ("2", vec[1]);
		CHECK_EQ("3", vec[2]);
		CHECK_EQ("*", vec[3]);
		CHECK_EQ("+", vec[4]);
  }
}

TEST("infix-postfix conversion with parenthesis", 1) {
	// check if infix-postfix conversion gives back the correct result
  {
		BinExpTree tree = BinExpTree("(1.0+2)*3");
		auto vec = tree.parseExpression("(1.0+2)*3");
		CHECK_EQ("1.0", vec[0]);
		CHECK_EQ("2", vec[1]);
		CHECK_EQ("+", vec[2]);
		CHECK_EQ("3", vec[3]);
		CHECK_EQ("*", vec[4]);
  }
}

TEST("infix-postfix conversion with multiple parentheses", 1) {
	// check if infix-postfix conversion gives back the correct result
  {
		BinExpTree tree = BinExpTree("(0.5+2.5)*(3.7+0.3)");
		auto vec = tree.parseExpression("(0.5+2.5)*(3.7+0.3)");
		CHECK_EQ("0.5", vec[0]);
		CHECK_EQ("2.5", vec[1]);
		CHECK_EQ("+", vec[2]);
		CHECK_EQ("3.7", vec[3]);
		CHECK_EQ("0.3", vec[4]);
		CHECK_EQ("+", vec[5]);
		CHECK_EQ("*", vec[6]);
  }
}

TEST("print with addition", 1) {
	// check if print gives back the correct result
  {
		BinExpTree tree = BinExpTree("1.0+2");
		CHECK_EQ("1.0+2", tree.printExpression());
  }
}

TEST("print with multiplication", 1) {
	// check if print gives back the correct result
  {
		BinExpTree tree = BinExpTree("1.0+2*3");
		CHECK_EQ("1.0+2*3", tree.printExpression());
  }
}

TEST("print with parenthesis", 1) {
	// check if print gives back the correct result
  {
		BinExpTree tree = BinExpTree("(1.0+2)*3");
		CHECK_EQ("(1.0+2)*3", tree.printExpression());
  }
}

TEST("print with multiple parenthesis", 1) {
	// check if print gives back the correct result
  {
		BinExpTree tree = BinExpTree("(0.5+2.5)*(3.7+0.3)");
		CHECK_EQ("(0.5+2.5)*(3.7+0.3)", tree.printExpression());
  }
}

TEST("evaluate with addition", 1) {
	// check if evaluate gives back the correct result
  {
		BinExpTree tree = BinExpTree("1.0+2");
		CHECK_EQ(3.0, tree.evaluateExpression());
  }
}

TEST("evaluate with multiplication", 1) {
	// check if evaluate gives back the correct result
  {
		BinExpTree tree = BinExpTree("1.0+2*3");
		CHECK_EQ(7.0, tree.evaluateExpression());
  }
}

TEST("evaluate with parenthesis", 1) {
	// check if evaluate gives back the correct result
  {
		BinExpTree tree = BinExpTree("(1.0+2)*3");
		CHECK_EQ(9.0, tree.evaluateExpression());
  }
}

TEST("evaluate with multiple parentheses", 1) {
	// check if evaluate gives back the correct result
  {
		BinExpTree tree = BinExpTree("(0.5+2.5)*(3.7+0.3)");
		CHECK_EQ(12.0, tree.evaluateExpression());
  }
}

WOODPECKER_TEST_MAIN(-1, -1)

/// My Test

/*
#include <sstream>
#include <vector>

int main()
{
    std::string str_d= "-2.8+4";
    double d;
    std::stringstream ss;
    ss << str_d;
    ss >> d;

    std::cout << d;

    return 0;
}*/
