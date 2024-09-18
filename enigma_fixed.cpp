#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
using namespace std;

void printBinary(char c)
{
    for (int i = 7; i >= 0; i--)
    {
        std::cout << ((c >> i) & 1);
    }
    std::cout << std::endl;
}

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
        // cout<< "symbol" << c;
        for (char i = 0; i < alphabet_size; i++)
        {
            if (wiring[i] == c)
            {
                return i;
            }
        }
        // cout<< "symbol" << c;
        throw runtime_error("the symbol is not found in the alphabet:" + int(c));
        return -1;
    }

    void rotate()
    {
        char temp_symbol = wiring[0];
        for (int i = 0; i < alphabet_size - 1; i++)
        {
            wiring[i] = wiring[i + 1];
        }
        wiring[alphabet_size - 1] = temp_symbol;
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

    char reverse_splitting_bytes(char low_half, char high_half)
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

    void rotate_rotors()
    {
        int rotors_mod = 1;
        for (int i = 0; i < rotors.size(); ++i)
        {
            if (turns_count % rotors_mod == 0)
            {
                rotors[i].rotate();
            }
            rotors_mod *= rotors.size();
        }
    }

    string encrypt(string plaintext)
    {
        string encryptedText;
        for (char c : plaintext)
        {

            pair<char, char> part_byte = split_byte(c);
            char lower_half = part_byte.first;
            char high_half = part_byte.second;
            char encrypted_lower_half = encrypt_char(lower_half);
            rotate_rotors();
            char encrypted_high_half = encrypt_char(high_half);
            rotate_rotors();
            encryptedText += encrypted_lower_half;
            encryptedText += encrypted_high_half;
        }
        return encryptedText;
    }

    string decrypt(string ciphertext)
    {
        string decryptedText;
        for (int i = 0; i < ciphertext.size(); i += 2)
        {
            char lower_half = ciphertext[i];
            char high_half = ciphertext[i + 1];
            char decrypted_lower_half = decrypt_char(lower_half);
            rotate_rotors();
            char decrypted_high_half = decrypt_char(high_half);
            rotate_rotors();
            char result = reverse_splitting_bytes(decrypted_lower_half, decrypted_high_half);
            decryptedText += result;
        }
        return decryptedText;
    }

    void encrypt_file(const std::string &file_path)
    {
        // Open the input file
        std::ifstream input_file(file_path);
        if (!input_file.is_open())
        {
            std::cerr << "Error opening file: " << file_path << std::endl;
            return;
        }

        // Create the output file
        std::string output_file_path = file_path + ".enigma";
        std::ofstream output_file(output_file_path, std::ios::trunc);
        if (!output_file.is_open())
        {
            std::cerr << "Error creating file: " << output_file_path << std::endl;
            return;
        }

        char buffer[51]; // 50 bytes + 1 for the null terminator
        while (input_file.read(buffer, 50) && input_file.gcount() > 0)
        {
            // Get the number of bytes read
            size_t bytes_read = input_file.gcount();

            // Create a string from the read bytes
            std::string chunk(buffer, bytes_read);

            // Encrypt the chunk
            std::string encrypted_chunk = encrypt(chunk);

            // Write the encrypted chunk to the output file
            output_file << encrypted_chunk;
        }

        input_file.close();
        output_file.close();
        std::cout << "File successfully encrypted and saved to: " << output_file_path << std::endl;
    }

    void decrypt_file(const std::string &file_path)
    {
        // Open the input file
        std::ifstream input_file(file_path);
        if (!input_file.is_open())
        {
            std::cerr << "Error opening file: " << file_path << std::endl;
            return;
        }

        // Create the output file
        size_t lastDotPos = file_path.find_last_of('.');
        string baseName = file_path.substr(0, lastDotPos);
        string outputFilePath = baseName + "_decrypted" + file_path.substr(lastDotPos);
        std::ofstream output_file(outputFilePath, std::ios::trunc);
        if (!output_file.is_open())
        {
            std::cerr << "Error creating file: " << outputFilePath << std::endl;
            return;
        }

        char buffer[51];
        while (input_file.read(buffer, 50) && input_file.gcount() > 0)
        {
            // Get the number of bytes read
            size_t bytes_read = input_file.gcount();

            // Create a string from the read bytes
            std::string chunk(buffer, bytes_read);

            // Decrypt the chunk
            std::string decrypted_chunk = decrypt(chunk);

            // Write the decrypted chunk to the output file
            output_file << decrypted_chunk;
        }

        input_file.close();
        output_file.close();
        std::cout << "File successfully decrypted and saved to: " << outputFilePath << std::endl;
    }
};

int main()
{
    // Example usage: Define different rotor configurations
    vector<Rotor> rotors1 = {
        Rotor({4, 10, 12, 5, 11, 6, 3, 15, 7, 14, 1, 13, 0, 2, 8, 9}, 2), // Rotor I
        Rotor({0, 9, 3, 10, 15, 8, 14, 13, 12, 11, 2, 7, 1, 6, 4, 5}, 1), // Rotor II
        Rotor({9, 12, 3, 4, 5, 11, 1, 2, 14, 7, 13, 15, 6, 10, 8, 0}, 12) // Rotor III
    };

    // Define reflector wiring (example)
    vector<pair<char, char>> reflector_wiring = {
        {0, 4}, {1, 9}, {2, 12}, {3, 15}, {4, 0}, {5, 11}, {6, 14}, {7, 13}, {8, 10}, {9, 1}, {10, 8}, {11, 5}, {12, 2}, {13, 7}, {14, 6}, {15, 3}};
    Reflector reflector(reflector_wiring);

    // Define plugboard connections (example)
    vector<pair<char, char>> plugboard_connections = {
        {0, 4}, {1, 9}, {2, 12}, {3, 15}, {4, 0}, {5, 11}, {6, 14}, {7, 13}, {8, 10}, {9, 1}, {10, 8}, {11, 5}, {12, 2}, {13, 7}, {14, 6}, {15, 3}};

    Plugboard plugboard(plugboard_connections); // Correct constructor

    // Create an Enigma machine instance
    EnigmaMachine enigma1(rotors1, reflector, plugboard);
    EnigmaMachine enigma2(rotors1, reflector, plugboard);
    // Example encryption and decryption
    string plaintext = "i need some sleep";
    string ciphertext = enigma1.encrypt(plaintext);
    cout << "Text to encrypt:" << plaintext << endl;
    cout << "Ciphertext: " << ciphertext << endl;
    string decryptedText = enigma2.decrypt(ciphertext);
    cout << "Decrypted Text: " << decryptedText << endl;
    enigma1.encrypt_file("/home/andrew/uni/info_defend/docs.zip");
    enigma2.decrypt_file("/home/andrew/uni/info_defend/docs.zip.enigma");
    return 0;
}