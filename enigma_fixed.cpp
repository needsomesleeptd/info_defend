#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

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
        throw runtime_error("the symbol is not found in the alphabet:" + int(c));
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
    int turns_count = 0;
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
       // cout << "Char before encrypt:" << c << endl;
       c = plugboard.swap(c);
       // cout << "Char after plugboard swap:" << c <<endl;
        for (auto &rotor : rotors)
        {
            c = rotor.encrypt(c);
        }
     //   cout << "Char after rotor swap:" << c <<endl;
        c = reflector.reflect(c);
      //  cout << "Char after reflect swap:" << c <<endl;
        for (size_t i = rotors.size() - 1; i != -1; --i)
        {
            c = rotors[i].decrypt(c);
        }

     //   cout << "Char after reverse rotor swap:" << c <<endl;
        c = plugboard.swap(c);
     //   cout << "Char after plugboard swap:" << c <<endl;



        return c;
    }


    char decrypt_char(char c)
    {
     
      //  cout << "Char before decrypt:" << c << endl;
        c = plugboard.swap(c);
     //   cout << "Char after plugboard:" << c << endl;
        for (auto &rotor : rotors)
        {
            c = rotor.encrypt(c);
        }

      //  cout << "Char after rotor:" << c << endl;
        c = reflector.reflect(c);
    //     cout << "Char after reflect:" << c << endl;
         for (size_t i = rotors.size() - 1; i != -1; --i)
        {
            c = rotors[i].decrypt(c);
        }
    //    cout << "Char after reverse rotor:" << c << endl;
        c = plugboard.swap(c);
    //    cout << "Char after plugboard swap:" << c << endl;

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
        Rotor({4, 10, 12, 5, 11, 6, 3, 15, 7, 14, 1, 13, 0, 2, 8, 9}, 2), // Rotor I
        Rotor({0, 9, 3, 10, 15, 8, 14, 13, 12, 11, 2, 7, 1, 6, 4, 5}, 1), // Rotor II
       // Rotor({9, 12, 3, 4, 5, 11, 1, 2, 14, 7, 13, 15, 6, 10, 8, 0}, 12)  // Rotor III
    };

    // Define reflector wiring (example)
    vector<pair<char, char>> reflector_wiring = {
        {0, 4}, {1, 9}, {2, 12}, {3, 15}, {4, 0},
        {5, 11}, {6, 14}, {7, 13}, {8, 10}, {9, 1},
        {10, 8}, {11, 5}, {12, 2}, {13, 7}, {14, 6}, {15, 3}
    };
    Reflector reflector(reflector_wiring);

    // Define plugboard connections (example)
    vector<pair<char, char>> plugboard_connections = {
        {0, 4}, {1, 9}, {2, 12}, {3, 15}, {4, 0}, 
        {5, 11}, {6, 14}, {7, 13}, {8, 10}, {9, 1},
        {10, 8}, {11, 5}, {12, 2}, {13, 7}, {14, 6}, {15, 3}
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