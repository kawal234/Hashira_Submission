#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <stdexcept>

using namespace std;

// Convert number string in arbitrary base to decimal
long long convertToDecimal(const string& number, int base) {
    long long result = 0;
    for (char c : number) {
        int digit;
        if (isdigit(c)) digit = c - '0';
        else if (isalpha(c)) digit = tolower(c) - 'a' + 10;
        else throw invalid_argument("Invalid character in base string");

        if (digit >= base) throw invalid_argument("Digit out of base range");
        result = result * base + digit;
    }
    return result;
}

struct Share {
    long long x;
    long long y;
};

// Lagrange interpolation to find constant term (x=0)
long long reconstructSecret(const vector<Share>& shares) {
    long double secret = 0.0;
    int k = shares.size();

    for (int i = 0; i < k; ++i) {
        long double term = shares[i].y;
        for (int j = 0; j < k; ++j) {
            if (i != j) {
                term *= -shares[j].x;
                term /= (shares[i].x - shares[j].x);
            }
        }
        secret += term;
    }

    return llround(secret); // rounding to nearest integer
}

// Generate combinations
void generateCombinationsRecursive(const vector<Share>& elements, int k, int start,
                                   vector<Share>& current, vector<vector<Share>>& result) {
    if (current.size() == k) {
        result.push_back(current);
        return;
    }
    for (int i = start; i < elements.size(); ++i) {
        current.push_back(elements[i]);
        generateCombinationsRecursive(elements, k, i + 1, current, result);
        current.pop_back();
    }
}

vector<vector<Share>> generateCombinations(const vector<Share>& elements, int k) {
    vector<vector<Share>> result;
    vector<Share> current;
    generateCombinationsRecursive(elements, k, 0, current, result);
    return result;
}

int main() {
    // Example Test Case
    int N = 4, K = 3;
    map<int, pair<int, string>> input = {
        {1, {10, "4"}},
        {2, {2, "111"}},
        {3, {10, "12"}},
        {6, {4, "213"}}
    };

    vector<Share> shares;

    for (const auto& entry : input) {
        int index = entry.first;
        int base = entry.second.first;
        string value = entry.second.second;

        try {
            long long y_decimal = convertToDecimal(value, base);
            shares.push_back({index, y_decimal});
            cout << "Processed share x=" << index << ", y=" << y_decimal << endl;
        } catch (const exception& e) {
            cerr << "Error converting share " << index << ": " << e.what() << endl;
        }
    }

    vector<vector<Share>> combinations = generateCombinations(shares, K);
    map<long long, int> secretFrequency;
    for (const auto& combo : combinations) {
        try {
            long long secret = reconstructSecret(combo);
            secretFrequency[secret]++;
        } catch (...) {
            continue;
        }
    }

    // Find most frequent secret
    long long finalSecret = 0;
    int maxCount = 0;
    for (const auto& entry : secretFrequency) {
        if (entry.second > maxCount) {
            finalSecret = entry.first;
            maxCount = entry.second;
        }
    }

    cout << "\n--- Final Reconstructed Secret ---\n";
    cout << "The most frequent secret is: " << finalSecret << " (occurred " << maxCount << " times)\n";

    return 0;
}
