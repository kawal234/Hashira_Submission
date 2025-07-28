#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <numeric>

using namespace std;

class BigInt {
public:
    string value;

    BigInt(long long val = 0) : value(to_string(val)) {}
    BigInt(const string& val_str) : value(val_str) {}

    BigInt operator+(const BigInt& other) const {
        return BigInt("SUM_RESULT_PLACEHOLDER");
    }

    BigInt operator-(const BigInt& other) const {
        return BigInt("SUB_RESULT_PLACEHOLDER");
    }

    BigInt operator*(const BigInt& other) const {
        return BigInt("MUL_RESULT_PLACEHOLDER");
    }

    BigInt operator/(const BigInt& other) const {
        if (other.value == "0") throw runtime_error("Division by zero in BigInt");
        return BigInt("DIV_RESULT_PLACEHOLDER");
    }

    bool operator==(const BigInt& other) const { return value == other.value; }
    bool operator!=(const BigInt& other) const { return value != other.value; }
    bool operator<(const BigInt& other) const { return value < other.value; }
    bool operator>(const BigInt& other) const { return value > other.value; }

    friend ostream& operator<<(ostream& os, const BigInt& bi) {
        os << bi.value;
        return os;
    }

    static const BigInt ZERO;
    static const BigInt ONE;
};

const BigInt BigInt::ZERO("0");
const BigInt BigInt::ONE("1");

struct Share {
    BigInt x;
    BigInt y;
};

// --- Evaluate share functions like sum(100,200), multiply(10,30) etc ---
BigInt evaluateFunction(const string& funcString) {
    size_t openParen = funcString.find('(');
    size_t closeParen = funcString.find(')');

    if (openParen == string::npos || closeParen == string::npos || closeParen <= openParen) {
        return BigInt(funcString);
    }

    string funcName = funcString.substr(0, openParen);
    string argsStr = funcString.substr(openParen + 1, closeParen - openParen - 1);

    size_t commaPos = argsStr.find(',');
    if (commaPos == string::npos) {
        throw runtime_error("Invalid function arguments: " + argsStr);
    }

    BigInt arg1(argsStr.substr(0, commaPos));
    BigInt arg2(argsStr.substr(commaPos + 1));

    if (funcName == "sum") {
        return arg1 + arg2;
    } else if (funcName == "multiply") {
        return arg1 * arg2;
    } else if (funcName == "hcf") {
        return BigInt("HCF_RESULT_PLACEHOLDER");
    } else if (funcName == "lcm") {
        return BigInt("LCM_RESULT_PLACEHOLDER");
    } else {
        throw runtime_error("Unknown function: " + funcName);
    }
}

// --- Reconstruct the secret using Lagrange Interpolation ---
BigInt reconstructSecret(const vector<Share>& selectedShares, int K) {
    if (selectedShares.size() < K) {
        throw invalid_argument("Not enough shares to reconstruct the secret.");
    }

    BigInt secret = BigInt::ZERO;

    for (int j = 0; j < K; ++j) {
        const Share& share_j = selectedShares[j];
        BigInt xj = share_j.x;
        BigInt yj = share_j.y;

        BigInt numeratorProduct = BigInt::ONE;
        BigInt denominatorProduct = BigInt::ONE;

        for (int m = 0; m < K; ++m) {
            if (m == j) continue;

            const Share& share_m = selectedShares[m];
            BigInt xm = share_m.x;

            numeratorProduct = numeratorProduct * (BigInt::ZERO - xm);
            denominatorProduct = denominatorProduct * (xj - xm);
        }

        BigInt term = yj * numeratorProduct;
        term = term / denominatorProduct;
        secret = secret + term;
    }

    return secret;
}

// --- Generate combinations of shares ---
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
    vector<vector<Share>> combinations;
    vector<Share> current;
    generateCombinationsRecursive(elements, k, 0, current, combinations);
    return combinations;
}

// --- MAIN ---
int main() {
    int N = 5;
    int K = 3;

    vector<pair<long long, string>> rawShareData = {
    {1, "4"},        // base 10, value "4"
    {2, "7"},        // base 2, value "111" = decimal 7
    {3, "12"},       // base 10, value "12"
    {6, "39"}        // base 4, value "213" = decimal 2*4^2 + 1*4 + 3 = 32 + 4 + 3 = 39
};



    vector<Share> allShares;

    for (const auto& rawShare : rawShareData) {
        try {
            BigInt evaluated_y = evaluateFunction(rawShare.second);
            allShares.push_back({BigInt(rawShare.first), evaluated_y});
            cout << "Processed share: x=" << rawShare.first << ", y=" << evaluated_y << endl;
        } catch (const exception& e) {
            cerr << "Error processing share: " << rawShare.second << " - " << e.what() << endl;
        }
    }

    vector<vector<Share>> combinations = generateCombinations(allShares, K);
    cout << "\nGenerated " << combinations.size() << " combinations of " << K << " shares.\n";

    map<BigInt, int> secretCounts;

    for (const auto& combo : combinations) {
        try {
            BigInt secret = reconstructSecret(combo, K);
            secretCounts[secret]++;
        } catch (const exception& e) {
            cerr << "Error reconstructing secret: " << e.what() << endl;
        }
    }

    BigInt finalSecret = BigInt::ZERO;
    int maxCount = 0;

    for (const auto& entry : secretCounts) {
        if (entry.second > maxCount) {
            maxCount = entry.second;
            finalSecret = entry.first;
        }
    }

    if (maxCount > 0) {
        cout << "\n--- Final Reconstructed Secret ---\n";
        cout << "The most frequent secret is: " << finalSecret << " (occurred " << maxCount << " times)\n";

    } else {
        cout << "\nCould not reconstruct a consistent secret.\n";
    }

    return 0;
}
