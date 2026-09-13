#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace kmr::test {

class Runner {
public:
    using Test = std::pair<std::string, std::function<void()>>;

    void add(std::string name, std::function<void()> test) {
        tests_.emplace_back(std::move(name), std::move(test));
    }

    int run() const {
        int failures = 0;
        for (const auto& [name, test] : tests_) {
            try {
                test();
                std::cout << "[PASS] " << name << '\n';
            } catch (const std::exception& ex) {
                ++failures;
                std::cerr << "[FAIL] " << name << ": " << ex.what() << '\n';
            } catch (...) {
                ++failures;
                std::cerr << "[FAIL] " << name << ": unknown exception\n";
            }
        }
        std::cout << tests_.size() - static_cast<std::size_t>(failures)
                  << "/" << tests_.size() << " tests passed\n";
        return failures == 0 ? 0 : 1;
    }

private:
    std::vector<Test> tests_;
};

inline void require(bool condition, const char* expression, const char* file, int line) {
    if (!condition) {
        std::ostringstream out;
        out << file << ':' << line << " requirement failed: " << expression;
        throw std::runtime_error(out.str());
    }
}

template <typename A, typename B>
void requireEqual(const A& actual, const B& expected,
                  const char* actualExpr, const char* expectedExpr,
                  const char* file, int line) {
    if (!(actual == expected)) {
        std::ostringstream out;
        out << file << ':' << line << " expected " << actualExpr << " == " << expectedExpr
            << " but got [" << actual << "] and [" << expected << ']';
        throw std::runtime_error(out.str());
    }
}

}  // namespace kmr::test

#define REQUIRE(expr) ::kmr::test::require((expr), #expr, __FILE__, __LINE__)
#define REQUIRE_EQ(actual, expected) ::kmr::test::requireEqual((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define REQUIRE_THROWS(expr) do { bool threw_ = false; try { (void)(expr); } catch (...) { threw_ = true; } REQUIRE(threw_); } while (false)
