#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cstring>
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
    int get_alphabet_size(){
        return alphabet_size;
    }
    void check()
    {
        bool found_num = true;
        for (int i = 0; i < alphabet_size; i++)
        {
            found_num = false;
            for (int j = 0; j < alphabet_size; j++)
            {
                if (i == wiring[j])
                {
                    found_num = true;
                    break;
                }
            }
            if (!found_num)
                throw runtime_error("invalid wiring rotor, not all numbers are present no number:" + to_string(i));
        }
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
    int alphabet_size; 
public:
    Reflector(vector<pair<char, char>> &wiring, int alphabet_size_src) : connection(wiring)
    {
        alphabet_size=alphabet_size_src;
    }
    int get_alphabet_size() {
        return alphabet_size;
    }
    void check()
    {
        bool found_num = true;
        for (int i = 0; i < alphabet_size; i++)
        {
            found_num = false;
            for (int j = 0; j < connection.size(); j++)
            {
                if (i == connection[j].first or i == connection[j].second)
                {
                    found_num = true;
                    break;
                }
            }
            if (!found_num)
                 throw runtime_error("invalid wiring refelctor, not all numbers are present no number:"+ to_string(i));
        }
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
private:
    int turns_count = 0;
    int batch_file;
    vector<Rotor> rotors;
    Reflector reflector;
    Plugboard plugboard;

public:
    EnigmaMachine(vector<Rotor> rotors, Reflector reflector, Plugboard plugboard, int batch_file = 40) : rotors(rotors), reflector(reflector), plugboard(plugboard)
    {
        batch_file = batch_file;
    }

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
        std::ifstream input_file(file_path, std::ios::binary);
        if (!input_file.is_open())
        {
            std::cerr << "Error opening file: " << file_path << std::endl;
            return;
        }

        // Create the output file
        std::string output_file_path = file_path + ".enigma";
        std::ofstream output_file(output_file_path, std::ios::binary | std::ios::trunc);
        if (!output_file.is_open())
        {
            std::cerr << "Error creating file: " << output_file_path << std::endl;
            return;
        }

        // Use a reasonable batch size for reading
        const int batch_size = 4096; // 4KB
        char buffer[batch_size];

        while (input_file.read(buffer, batch_size) || input_file.gcount() > 0)
        {
            size_t bytes_read = input_file.gcount();
            std::string chunk(buffer, bytes_read);

            // Encrypt the chunk
            std::string encrypted_chunk = encrypt(chunk); // Assuming you have an `encrypt` function

            // Write the encrypted chunk to the output file
            output_file.write(encrypted_chunk.data(), encrypted_chunk.size());
        }

        input_file.close();
        output_file.close();
        std::cout << "File successfully encrypted and saved to: " << output_file_path << std::endl;
    }

    void decrypt_file(const std::string &file_path)
    {
        // Open the input file
        std::ifstream input_file(file_path, std::ios::binary);
        if (!input_file.is_open())
        {
            std::cerr << "Error opening file: " << file_path << std::endl;
            return;
        }

        // Create the output file
        size_t lastDotPos = file_path.find_last_of('.');
        std::string baseName = file_path.substr(0, lastDotPos);
        std::string outputFilePath = baseName + "_decrypted" + file_path.substr(lastDotPos);
        std::ofstream output_file(outputFilePath, std::ios::binary | std::ios::trunc);
        if (!output_file.is_open())
        {
            std::cerr << "Error creating file: " << outputFilePath << std::endl;
            return;
        }

        const int batch_size = 4096; // 4KB
        char buffer[batch_size];

        while (input_file.read(buffer, batch_size) || input_file.gcount() > 0)
        {
            size_t bytes_read = input_file.gcount();
            std::string chunk(buffer, bytes_read);

            // Decrypt the chunk
            std::string decrypted_chunk = decrypt(chunk); // Assuming you have a `decrypt` function

            // Write the decrypted chunk to the output file
            output_file.write(decrypted_chunk.data(), decrypted_chunk.size());
        }

        input_file.close();
        output_file.close();
        std::cout << "File successfully decrypted and saved to: " << outputFilePath << std::endl;
    }

    void check_self()
    {
        int alphabet_size = rotors[0].get_alphabet_size();
        for (auto &rotor : rotors)
        {
            if  (rotor.get_alphabet_size() != alphabet_size) {
                throw runtime_error("invalid rotor aphabet size,required one:" + to_string(alphabet_size));
            }
            rotor.check();
        }
        if (alphabet_size != reflector.get_alphabet_size()) {
            throw runtime_error("invalid reflector alpabet size,required one:" + to_string(alphabet_size));
        }
        reflector.check();
    }
};

int main()
{
    // Example usage: Define different rotor configurations
    int alphabet_size= 16;
    vector<Rotor> rotors1 = {
        Rotor({4, 10, 12, 5, 11, 6, 3, 15, 7, 14, 1, 13, 0, 2, 8, 9}, 2), // Rotor I
        Rotor({0, 9, 3, 10, 15, 8, 14, 13, 12, 11, 2, 7, 1, 6, 4, 5}, 1), // Rotor II
        Rotor({9, 12, 3, 4, 5, 11, 1, 2, 14, 7, 13, 15, 6, 10, 8, 0}, 12) // Rotor III
    };

    // Define reflector wiring (example)
    vector<pair<char, char>> reflector_wiring = {
        {0, 4}, {1, 9}, {2, 12}, {3, 15}, {4, 0}, {5, 11}, {6, 14}, {7, 13}, {8, 10}, {9, 1}, {10, 8}, {11, 5}, {12, 2}, {13, 7}, {14, 6}, {15, 3}};
    Reflector reflector(reflector_wiring,alphabet_size);

    // Define plugboard connections (example)
    vector<pair<char, char>> plugboard_connections = {
        {0, 4}, {1, 9}, {2, 12}, {3, 15}, {4, 0}, {5, 11}, {6, 14}, {7, 13}, {8, 10}, {9, 1}, {10, 8}, {11, 5}, {12, 2}, {13, 7}, {14, 6}, {15, 3}};

    Plugboard plugboard(plugboard_connections); // Correct constructor
    
    // Create an Enigma machine instance
    EnigmaMachine enigma1(rotors1, reflector, plugboard);
    EnigmaMachine enigma2(rotors1, reflector, plugboard);
    enigma1.check_self();
    enigma2.check_self();
    // Example encryption and decryption
    string plaintext = "i need some sleep";
    string ciphertext = enigma1.encrypt(plaintext);
    cout << "Text to encrypt:" << plaintext << endl;
    cout << "Ciphertext: " << ciphertext << endl;
    string decryptedText = enigma2.decrypt(ciphertext);
    cout << "Decrypted Text: " << decryptedText << endl;
    enigma1.encrypt_file("bmstu-iu7-OS-main-linux (1).zip");
    enigma2.decrypt_file("bmstu-iu7-OS-main-linux (1).zip.enigma");
    return 0;
}