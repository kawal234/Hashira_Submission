#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <algorithm>

using namespace std;

// Simple BigInt class for addition, multiplication, and division by long long
// (Basic implementation, extend as needed)
class BigInt {
public:
    string value; // digits stored in normal order, no leading zeros except for zero itself

    BigInt() : value("0") {}
    BigInt(const string& val) : value(val) { normalize(); }
    BigInt(long long val) : value(to_string(val)) {}

    void normalize() {
        // Remove leading zeros
        while (value.size() > 1 && value[0] == '0') value.erase(0,1);
    }

    // Add two BigInts
    BigInt operator+(const BigInt& other) const {
        string a = value;
        string b = other.value;
        string res;

        int carry = 0;
        int i = (int)a.size() - 1;
        int j = (int)b.size() - 1;

        while (i >= 0 || j >= 0 || carry) {
            int x = (i >= 0) ? (a[i] - '0') : 0;
            int y = (j >= 0) ? (b[j] - '0') : 0;
            int sum = x + y + carry;
            carry = sum / 10;
            res.push_back((sum % 10) + '0');
            i--; j--;
        }
        reverse(res.begin(), res.end());
        return BigInt(res);
    }

    // Multiply BigInt by long long (for simplicity)
    BigInt operator*(const BigInt& other) const {
        // We'll implement multiplication by another BigInt (grade-school)
        const string& a = value;
        const string& b = other.value;
        int n = (int)a.size(), m = (int)b.size();
        vector<int> result(n + m, 0);

        for (int i = n - 1; i >= 0; i--) {
            for (int j = m - 1; j >= 0; j--) {
                int mul = (a[i] - '0') * (b[j] - '0');
                int sum = result[i + j + 1] + mul;
                result[i + j + 1] = sum % 10;
                result[i + j] += sum / 10;
            }
        }
        string res;
        int k = 0;
        while (k < (int)result.size() -1 && result[k] == 0) k++; // skip leading zeros
        for (; k < (int)result.size(); k++)
            res.push_back(result[k] + '0');

        return BigInt(res);
    }

    // Subtract other BigInt from this (assume *this >= other)
    BigInt operator-(const BigInt& other) const {
        const string& a = value;
        const string& b = other.value;
        string res;
        int carry = 0;
        int i = (int)a.size() - 1;
        int j = (int)b.size() - 1;

        while (i >= 0) {
            int x = a[i] - '0' - carry;
            int y = (j >= 0) ? (b[j] - '0') : 0;
            if (x < y) {
                x += 10;
                carry = 1;
            } else {
                carry = 0;
            }
            res.push_back(x - y + '0');
            i--; j--;
        }

        while (res.size() > 1 && res.back() == '0') res.pop_back();
        reverse(res.begin(), res.end());
        return BigInt(res);
    }

    // Division by long long (returns quotient only)
    BigInt operator/(const BigInt& other) const {
        // Only support division by small integer (fits in long long)
        long long divisor = 0;
        for (char c : other.value) {
            divisor = divisor * 10 + (c - '0');
        }
        if (divisor == 0) throw runtime_error("Division by zero");

        string quotient;
        long long remainder = 0;
        for (size_t i = 0; i < value.size(); ++i) {
            remainder = remainder * 10 + (value[i] - '0');
            long long q = remainder / divisor;
            remainder = remainder % divisor;
            if (!(quotient.empty() && q == 0)) {
                quotient.push_back(q + '0');
            }
        }
        if (quotient.empty()) quotient = "0";
        return BigInt(quotient);
    }

    bool operator==(const BigInt& other) const { return value == other.value; }
    bool operator!=(const BigInt& other) const { return value != other.value; }
    bool operator<(const BigInt& other) const {
        if (value.size() != other.value.size())
            return value.size() < other.value.size();
        return value < other.value;
    }
    bool operator>(const BigInt& other) const { return other < *this; }

    friend ostream& operator<<(ostream& os, const BigInt& bi) {
        os << bi.value;
        return os;
    }

    static const BigInt ZERO;
    static const BigInt ONE;
};

const BigInt BigInt::ZERO = BigInt("0");
const BigInt BigInt::ONE = BigInt("1");

// Convert string number from arbitrary base to decimal BigInt
BigInt convertToDecimal(const string& numStr, int base) {
    BigInt result = BigInt::ZERO;
    BigInt baseBig = BigInt(base);

    for (char c : numStr) {
        int digit = 0;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
        else throw runtime_error("Invalid digit in input");

        if (digit >= base) throw runtime_error("Digit outside base");

        result = result * baseBig + BigInt(digit);
    }
    return result;
}

struct Share {
    BigInt x;
    BigInt y;
};

// Lagrange interpolation at x=0 to find secret (constant term)
BigInt reconstructSecret(const vector<Share>& shares) {
    int k = (int)shares.size();
    BigInt secret = BigInt::ZERO;

    for (int j = 0; j < k; j++) {
        BigInt numerator = BigInt::ONE;
        BigInt denominator = BigInt::ONE;

        for (int m = 0; m < k; m++) {
            if (m == j) continue;
            numerator = numerator * (BigInt::ZERO - shares[m].x);
            denominator = denominator * (shares[j].x - shares[m].x);
        }

        // division by denominator (which should be invertible)
        // Here we do exact division since shares are integers and denominator divides numerator*yj exactly
        BigInt term = shares[j].y * numerator;
        // denominator must be converted to long long for division operator, here we convert BigInt to long long safely only if small enough:
        // But our BigInt division only supports divisor as long long, so we must implement BigInt division fully or limit input size.
        // For simplicity, let's convert denominator to long long if fits, else throw (for this demo).
        long long denomLL = 0;
        try {
            denomLL = stoll(denominator.value);
        } catch (...) {
            throw runtime_error("Denominator too large for division");
        }
        term = term / BigInt(denomLL);
        secret = secret + term;
    }
    return secret;
}

int main() {
    // Inputs: map from share number -> pair(base, string value)
    map<int, pair<int, string>> input = {
        {1, {6,  "13444211440455345511"}},
        {2, {15, "aed7015a346d63"}},
        {3, {15, "6aeeb69631c227c"}},
        {4, {16, "e1b5e05623d881f"}},
        {5, {8,  "316034514573652620673"}},
        {6, {3,  "2122212201122002221120200210011020220200"}},
        {7, {3,  "20120221122211000100210021102001201112121"}},
        {8, {6,  "20220554335330240002224253"}},
        {9, {12, "45153788322a1255483"}},
        {10,{7,  "1101613130313526312514143"}}
    };

    int K = 7; // threshold

    vector<Share> shares;
    try {
        for (const auto& item : input) {
            int x = item.first;
            int base = item.second.first;
            string val = item.second.second;

            BigInt y = convertToDecimal(val, base);
            shares.push_back({BigInt(x), y});
            cout << "Share #" << x << ": y = " << y << endl;
        }
    } catch (const exception& e) {
        cerr << "Error converting shares: " << e.what() << endl;
        return 1;
    }

    try {
        // Use only first K shares for reconstruction
        vector<Share> selectedShares(shares.begin(), shares.begin() + K);

        BigInt secret = reconstructSecret(selectedShares);
        cout << "Reconstructed secret (decimal): " << secret << endl;
    } catch (const exception& e) {
        cerr << "Error reconstructing secret: " << e.what() << endl;
        return 1;
    }

    return 0;
}
