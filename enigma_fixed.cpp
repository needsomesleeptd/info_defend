#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Rotor
{
private:
    std::vector<int> wiring;
    int position;
    int alphabet_size;
    int offset;

public:
    Rotor(std::vector<int> wiring, int position, int offset = 1) : wiring(wiring), position(position)
    {
        alphabet_size = wiring.size();
        if (offset <= 0)
        {
            throw runtime_error("offset must be greater than 0");
        }
        this->offset = offset;
    }

    char encrypt(char c)
    {
        return wiring[c]; // using c as index
    }

    char decrypt(char c)
    {
        //cout<< "symbol" << c;
        for (char i = 0; i < alphabet_size; i++)
        {
            if (wiring[i] == c)
            {
                return i;
            }
        }
        //cout<< "symbol" << c;
        throw runtime_error("the symbol is not found in the alphabet:" + string(1, c));
        return -1;
    }

    void rotate()
    {
        char temp_symbol = wiring[alphabet_size - 1];
        for (int i = 0; i < alphabet_size; i++)
        {
            wiring[i + 1] = wiring[i];
        }
        wiring[0] = temp_symbol;
    }
};

class Reflector
{
private:
    vector<pair<char, char>> connection;

public:
    Reflector(vector<pair<char, char>> &wiring) : connection(wiring)
    {
       
    }

    char reflect(char c)
    {
        for (auto &connection : connection)
        {
            if (c == connection.first)
            {
                return connection.second;
            }
            else if (c == connection.second)
            {
                return connection.first;
            }
        }
        throw runtime_error("character not in vocabulary:" + string(1, c));
        return c;
    }
};

class Plugboard
{
private:
    vector<pair<char, char>> connections;

public:
    Plugboard(vector<pair<char, char>> connections) : connections(connections) {}

    char swap(char c)
    {
        for (auto &connection : connections)
        {
            if (c == connection.first)
            {
                return connection.second;
            }
            else if (c == connection.second)
            {
                return connection.first;
            }
        }
        return c;
    }
};
void printBinary(char c) {
    for (int i = 7; i >= 0; i--) {
        std::cout << ((c >> i) & 1);
    }
    std::cout << std::endl;
}
class EnigmaMachine
{
public:
    vector<Rotor> rotors;
    Reflector reflector;
    Plugboard plugboard;

    EnigmaMachine(vector<Rotor> rotors, Reflector reflector, Plugboard plugboard) : rotors(rotors), reflector(reflector), plugboard(plugboard) {}

    pair<char, char> split_byte(char byte)
    {
      
        char high_half = (byte >> 4) & 0x0F; // shift right 4 bits and mask with 0x0F
        char low_half = byte & 0x0F;         // mask with 0x0F to get the low 4 bits

        /*cout<< "binary_byte"  << endl;
        printBinary(byte);
        cout<< "binary_low"  << endl;
        printBinary(low_half);
        cout<< "binary_high"  << endl;
        printBinary(high_half);
        cout<< "result of group binary" << endl;
        printBinary(reverse_splitting_bytes(low_half,high_half));
        cout<< endl;*/



        return make_pair(low_half, high_half);
    }

    char reverse_splitting_bytes(char low_half,char high_half)
    {
        return (high_half << 4) | low_half;
    }
   
    char encrypt_char(char c)
    {   
        cout << "Char before encrypt:" << c << endl;
        c = plugboard.swap(c);
        cout << "Char after plugboard swap:" << c <<endl;
        for (auto &rotor : rotors)
        {
            c = rotor.encrypt(c);
        }
        cout << "Char after rotor swap:" << c <<endl;
        c = reflector.reflect(c);
        cout << "Char after reflect swap:" << c <<endl;
        for (auto &rotor : rotors)
        {
            c = rotor.decrypt(c);
        }

        cout << "Char after reverse rotor swap:" << c <<endl;
        c = plugboard.swap(c);
        cout << "Char after plugboard swap:" << c <<endl;

        return c;
    }


    char decrypt_char(char c)
    {
        cout << "Char before decrypt:" << c << endl;
        c = plugboard.swap(c);
        cout << "Char after plugboard:" << c << endl;
        for (auto &rotor : rotors)
        {
            c = rotor.encrypt(c);
        }

        cout << "Char after rotor:" << c << endl;
        c = reflector.reflect(c);
         cout << "Char after reflect:" << c << endl;
        for (auto &rotor : rotors)
        {
            c = rotor.decrypt(c);
        }
        cout << "Char after reverse rotor:" << c << endl;
        c = plugboard.swap(c);
        cout << "Char after plugboard swap:" << c << endl;

        return c;
    }



    string encrypt(string plaintext)
    {
        string encryptedText;
        for (char c : plaintext)
        {
            pair<char,char> part_byte = split_byte(c);
            char lower_half = part_byte.first;
            char high_half = part_byte.second; 
            char encrypted_lower_half = encrypt_char(lower_half);
            char encrypted_high_half = encrypt_char(high_half);
            encryptedText += encrypted_lower_half;
            //rotors[0].rotate();
            encryptedText += encrypted_high_half;
            //rotors[0].rotate();
        }
        return encryptedText;
    }

    string decrypt(string ciphertext)
    {
        string decryptedText;
        for (int i = 0; i < ciphertext.size(); i +=2)
        {
            char lower_half = ciphertext[i];
            char high_half = ciphertext[i+1];
            char decrypted_lower_half = decrypt_char(lower_half);
            char decrypted_high_half = decrypt_char(high_half);
            char result = reverse_splitting_bytes(decrypted_lower_half,decrypted_high_half);
            decryptedText += result;
        }
        return decryptedText;
    }
};

int main()
{
    // Example usage: Define different rotor configurations
    vector<Rotor> rotors1 = {
        Rotor({4, 10, 12, 5, 11, 6, 3, 15, 7, 14, 1, 13, 0, 2, 8, 9, 20, 26, 28, 21, 27, 22, 19, 25, 23, 24, 16, 17, 18, 30, 36, 38, 31, 37, 32, 35, 39, 33, 34, 40, 46, 48, 41, 47, 42, 45, 49, 43, 44, 50, 56, 58, 51, 57, 52, 55, 59, 53, 54, 60, 62, 63, 61}, 12), // Rotor I
        //Rotor({0, 9, 3, 10, 15, 8, 14, 13, 12, 11, 2, 7, 1, 6, 4, 5}, 1), // Rotor II
        //Rotor({1, 15, 13, 12, 10, 11, 14, 2, 14, 5, 9, 8, 0, 7, 3, 4}, 2)  // Rotor III
    };
    // Define reflector wiring (example)
     vector<pair<char, char>> reflector_wiring = {
        {0, 4}, {1, 9}, {2, 12}, {3, 15}, {4, 0},
        {5, 11}, {6, 14}, {7, 13}, {8, 10}, {9, 1},
        {10, 8}, {11, 5}, {12, 2}, {13, 7}, {14, 6}, {15, 3},
        {16, 20}, {17, 25}, {18, 28}, {19, 23}, {20, 16}, 
        {21, 27}, {22, 30}, {23, 29}, {24, 26}, {25, 17},
        {26, 24}, {27, 21}, {28, 18}, {29, 23}, {30, 22}, 
        {31, 35}, {32, 39}, {33, 42}, {34, 45}, {35, 31},
        {36, 41}, {37, 44}, {38, 43}, {39, 32}, {40, 40}, 
        {41, 36}, {42, 33}, {43, 38}, {44, 37}, {45, 34},
        {46, 50}, {47, 55}, {48, 58}, {49, 53}, {50, 46}, 
        {51, 57}, {52, 60}, {53, 59}, {54, 56}, {55, 47},
        {56, 54}, {57, 51}, {58, 48}, {59, 53}, {60, 52}, 
        {61, 63}, {62, 61}, {63, 62}
    };
    Reflector reflector(reflector_wiring);

    // Define plugboard connections (example)
    vector<pair<char, char>> plugboard_connections = {
        {0, 4}, {1, 9}, {2, 12}, {3, 15}, {4, 0}, 
        {5, 11}, {6, 14}, {7, 13}, {8, 10}, {9, 1},
        {10, 8}, {11, 5}, {12, 2}, {13, 7}, {14, 6}, {15, 3},
        {16, 20}, {17, 25}, {18, 28}, {19, 23}, {20, 16}, 
        {21, 27}, {22, 30}, {23, 29}, {24, 26}, {25, 17},
        {26, 24}, {27, 21}, {28, 18}, {29, 23}, {30, 22}, 
        {31, 35}, {32, 39}, {33, 42}, {34, 45}, {35, 31},
        {36, 41}, {37, 44}, {38, 43}, {39, 32}, {40, 40}, 
        {41, 36}, {42, 33}, {43, 38}, {44, 37}, {45, 34},
        {46, 50}, {47, 55}, {48, 58}, {49, 53}, {50, 46}, 
        {51, 57}, {52, 60}, {53, 59}, {54, 56}, {55, 47},
        {56, 54}, {57, 51}, {58, 48}, {59, 53}, {60, 52}, 
        {61, 63}, {62, 61}, {63, 62}
    };
    Plugboard plugboard(plugboard_connections); // Correct constructor

    // Create an Enigma machine instance
    EnigmaMachine enigma(rotors1, reflector, plugboard);

    // Example encryption and decryption
    string plaintext = "i need some sleep";
    string ciphertext = enigma.encrypt(plaintext);
    cout << "Text to encrypt:" << plaintext << endl;
    cout << "Ciphertext: " << ciphertext << endl;
    string decryptedText = enigma.decrypt(ciphertext);
    cout << "Decrypted Text: " << decryptedText << endl; 

    

    return 0;
}