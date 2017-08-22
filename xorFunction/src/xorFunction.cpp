#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <random>
#include <elf.h>
#include <map>

/**
 * This is a simple little program that searches for a section called
 * "bind_shell_function" and xors the section with 0xaa.
 */

bool xor_function(std::string& p_data)
{
    if (p_data[0] != 0x7f || p_data[1] != 'E' || p_data[2] != 'L' || p_data[3] != 'F')
    {
        return false;
    }

    Elf64_Ehdr* ehdr = reinterpret_cast<Elf64_Ehdr*>(&p_data[0]);
    Elf64_Shdr* sections = reinterpret_cast<Elf64_Shdr*>(&p_data[ehdr->e_shoff]);
    Elf64_Half sections_count = ehdr->e_shnum;
    Elf64_Shdr* strings_header = reinterpret_cast<Elf64_Shdr*>(&p_data[ehdr->e_shoff] +
        (ehdr->e_shentsize * ehdr->e_shstrndx));
    const char* strings_table = &p_data[strings_header->sh_offset];

    // find all ".rc4_" sections.
    Elf64_Shdr* current = sections;
    for (int i = 0; i < sections_count; i++, current++)
    {
        std::string section_name(&strings_table[current->sh_name]);
        if (section_name.find(".bind_shell_function") == 0)
        {
            // encrypt the section
            for (std::size_t j = 0; j < current->sh_size; j++)
            {
                p_data[current->sh_offset + j] = p_data[current->sh_offset + j] ^ 0xaa;
            }
            std::cout << "[ 99%] Bind shell function obfuscated!" << std::endl;
        }
    }

    return true;
}

int main(int p_argc, char** p_argv)
{
    if (p_argc != 2)
    {
        std::cerr << "Usage: ./xorFunction <file path>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream inputFile(p_argv[1], std::ifstream::in | std::ifstream::binary);
    if (!inputFile.is_open() || !inputFile.good())
    {
        std::cerr << "Failed to open the provided file: " << p_argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std::string input((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    if (!xor_function(input))
    {
        std::cerr << "Failed to complete the xor function" << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outputFile(p_argv[1], std::ofstream::out | std::ofstream::binary);
    if (!outputFile.is_open() || !outputFile.good())
    {
        std::cerr << "Failed to wopen the provided file: " << p_argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    outputFile.write(input.data(), input.length());
    outputFile.close();

    return EXIT_SUCCESS;
}
