#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <cmath>

using namespace std;

// Convert a number from a given base to decimal (base 10)
long long baseToDecimal(const string& value, int base) {
    long long result = 0;
    for (char digit : value) {
        int val;
        if (isdigit(digit))
            val = digit - '0';
        else if (isalpha(digit))
            val = tolower(digit) - 'a' + 10;
        else
            throw invalid_argument("Invalid character in base string.");

        if (val >= base)
            throw invalid_argument("Digit exceeds base.");

        result = result * base + val;
    }
    return result;
}

// Structure to hold a share's input
struct ShareInput {
    int base;
    string value;
};

int main() {
    // JSON-like C++ map input (equivalent to your JSON)
    map<int, ShareInput> shares = {
        {1, {6, "13444211440455345511"}},
        {2, {15, "aed7015a346d63"}},
        {3, {15, "6aeeb69631c227c"}},
        {4, {16, "e1b5e05623d881f"}},
        {5, {8, "316034514573652620673"}},
        {6, {3, "2122212201122002221120200210011020220200"}},
        {7, {3, "20120221122211000100210021102001201112121"}},
        {8, {6, "20220554335330240002224253"}},
        {9, {12, "45153788322a1255483"}},
        {10, {7, "1101613130313526312514143"}}
    };

    const int N = 10;
    const int K = 7;

    // Store decimal shares
    vector<pair<int, long long>> decodedShares;

    cout << "Parsed Shares:\n";
    for (int i = 1; i <= N; ++i) {
        try {
            const auto& share = shares[i];
            long long decimalY = baseToDecimal(share.value, share.base);
            decodedShares.push_back({i, decimalY});
            cout << "Share #" << i << " (base " << share.base << "): " << share.value
                 << " => y = " << decimalY << endl;
        } catch (const exception& e) {
            cerr << "Error parsing share #" << i << ": " << e.what() << endl;
        }
    }

    // decodedShares now contains (x, y) pairs you can use with your Shamir logic
    // You can now plug `decodedShares` into your `generateCombinations()` and `reconstructSecret()` functions

    return 0;
}
