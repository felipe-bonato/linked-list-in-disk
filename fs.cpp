#ifndef fs_h
#define fs_h
#include <string>

#include <fstream>
#include <array>

#include <iostream>
#include <iomanip>

#define NAME_OFFSET 4
#define NEXT_PTR_OFFSET 24

size_t get_char_str_len(const char* str)
{
    // If char* is 20 bytes, this will break.
    size_t len = 0;
    while(str[len] != '\0'){
        len++;
    }
    return len;
}

size_t convert_bytes_to_int(const char* bytes)
{
    return bytes[3] << 32
        | bytes[2] << 16
        | bytes[1] << 8
        | bytes[0];
}

/**
 * @param arquivoDaLista nome do arquivo em disco que contem a lista encadeada
 * @param novoNome nome a ser adicionado apos depoisDesteNome
 * @param depoisDesteNome um nome presente na lista
 */
void adiciona(std::string arquivoDaLista, std::string novoNome, std::string depoisDesteNome)
{
    // Steps:
    //  1. Read file
    //  2. Read first 4 bytes
    //  3. Find node with name depoisDesteNome
    //  3. Loop:
    //     If 0, store data
    //     If 1, skip 20 bytes, read 4 bytes go to the pointer
    //  4. Store data

    std::ifstream src_file{arquivoDaLista, std::ios::binary};
    std::array<char, 284> nodes{};
    src_file.read(nodes.data(), nodes.size());
    
    size_t cur_node_index = convert_bytes_to_int(&nodes[0]);

    // Find node with name depoisDesteNome
    while(true){
        // Convert node c-style char* to std::string
        std::string cur_node_name{
            &nodes[cur_node_index + NAME_OFFSET],
            get_char_str_len(&nodes[cur_node_index + NAME_OFFSET])
        };
        // If true, we found what we wanted
        if(cur_node_name == depoisDesteNome){
            // Set cur_node_index to the index of the first node after depoisDesteNome
            cur_node_index = convert_bytes_to_int(&nodes[cur_node_index + NEXT_PTR_OFFSET]);
            break;
        }
        // If not, get next node index
        cur_node_index = convert_bytes_to_int(&nodes[cur_node_index + NEXT_PTR_OFFSET]);
    }
}

#endif /* fs_h */