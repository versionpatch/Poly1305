#include <iostream>
#include <vector>
#include <array>
#include <gmpxx.h>
#include <fstream>



std::string reverse_endianness(std::string s)
{
    std::string output;
    for (int i = 0;i < s.length()/2;i++)
    {
        int rev_idx = (s.length() / 2) - 1 - i;
        output += s[rev_idx*2];
        output += s[rev_idx*2 + 1];
    }
    return output;
}

std::string format_hex(std::string s)
{
    std::string output;
    for (int i = 0;i < s.length()/2 - 1;i++)
    {
        output += s[i * 2];
        output += s[i * 2 + 1];
        output += ":";
    }

    output += s[s.length() - 2];
    output += s[s.length() - 1];

    return output;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Incorrect input format." << std::endl;
        return -1;
    }
    std::string input_key = argv[1];
    if (input_key.length() != 64)
    {
        std::cerr << "Key must be 32 bytes and written in hexadecimal." << std::endl;
        return -1; 
    }
    mpz_class R,S;
    std::string R_sub = reverse_endianness(input_key.substr(0, 32));
    std::string S_sub = reverse_endianness(input_key.substr(32,32));
    if (R.set_str(R_sub, 16) != 0)
    {
        std::cerr << "Key must be in hexadecimal." << std::endl;
        return -1;
    }
    if (S.set_str(S_sub, 16) != 0)
    {
        std::cerr << "Key must be in hexadecimal." << std::endl;
        return -1; 
    }

    //Apply mask on R to clamp
    std::string mask_hex = "0x";
    mask_hex += ("0ffffffc0ffffffc0ffffffc0fffffff");

    mpz_class clamp_mask(mask_hex);
    R &= clamp_mask;

    std::ifstream file;
    file.open(argv[2], std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cerr << "Error occured while opening file." << std::endl;
        return -1; 
    }

    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    mpz_class p("0x3fffffffffffffffffffffffffffffffb");
    mpz_class pow128(1);
    mpz_class acc(0);
    pow128 <<= 128;

    for (int i = 0;i < file_size / 16;i++)
    {
        char m_chr[16];
        file.read(m_chr, 16);
        mpz_class m;
        mpz_import(m.get_mpz_t(), 16, -1, sizeof(char), 1, 0, m_chr);
        m += pow128;
        acc += m;
        acc = (R * acc) % p;
    }


    //read remaining bytes if there are any
    if (file_size % 16 != 0)
    {
        char m_chr[file_size % 16];
        file.read(m_chr, file_size % 16);
        mpz_class m;
        mpz_import(m.get_mpz_t(), file_size % 16, -1, sizeof(char), 1, 0, m_chr);
        mpz_class new_pow(1);
        new_pow <<= ((file_size % 16) * 8);
        m += new_pow;
        acc += m;
        acc = (R * acc) % p;
    }

    acc += S;
    clamp_mask.set_str("ffffffffffffffffffffffffffffffff", 16);
    acc &= clamp_mask;
    std::string out = acc.get_str(16);
    std::cout << format_hex(reverse_endianness(out)) << std::endl;



    file.close();

    return 0;


}