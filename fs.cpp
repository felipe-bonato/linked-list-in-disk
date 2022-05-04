#ifndef fs_h
#define fs_h
#include <string>

#include <fstream>
#include <array>

#include <iostream>
#include <iomanip>

#define NAME_OFFSET 4
#define NEXT_PTR_OFFSET 24
#define FIRST_NODE_OFFSET 4
#define NODE_SIZE 28
#define NODES_AMOUNT 10 // How many nodes are in the file

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
size_t findIndexOfNodeWithName(const std::string& name,  std::array<char, 284>& nodes, size_t startIndex)
{
    // Sketchy casting cause we need to shift 24 bits.
    // If we don't cast, the bits will loop around.
    return static_cast<uint8_t>(bytes[3]) << 24
        | static_cast<uint8_t>(bytes[2]) << 16
        | static_cast<uint8_t>(bytes[1]) << 8
        | static_cast<uint8_t>(bytes[0]);
    size_t index = startIndex;
    while(true){
        // Convert node c-style char* to std::string
        std::string curName{
            &nodes[index + NAME_OFFSET],
            getCharStrLen(&nodes[index + NAME_OFFSET])
        };
        // If true, we found what we wanted
        if(curName == name){
            return index;
        }
        // If not, get next node index
        index = nodes[index + NEXT_PTR_OFFSET];
    }
}

}

/**
 * @param arquivoDaLista nome do arquivo em disco que contem a lista encadeada
 * @param novoNome nome a ser adicionado apos depoisDesteNome
 * @param depoisDesteNome um nome presente na lista
 */
void adiciona(std::string arquivoDaLista, std::string novoNome, std::string depoisDesteNome)
{
    // Steps:
    //  1. Read file into std::array
    //  2. Read first 4 bytes to find firstNodeIndex
    //  3. Find index of the node with name depoisDesteNome
    //  4. Find index of the node after depoisDesteNome
    //  5. Find index of empty space
    //  6. Write new node with name novoNome to empty space, and point to next node

    std::ifstream src_file{arquivoDaLista, std::ios::binary};
    std::array<char, 284> nodes{};
    src_file.read(nodes.data(), nodes.size());
    src_file.close();

    size_t cur_node_index = convert_bytes_to_int(&nodes[0]);


    size_t firstNodeIndex = nodes[0];
    size_t nodeDepoisDesteNomeIndex = findIndexOfNodeWithName(depoisDesteNome, nodes, firstNodeIndex);
    
    // We now find an empty node
    /*
    while(nodes[cur_node_index] != 0){
        cur_node_index = convert_bytes_to_int(&nodes[cur_node_index + NEXT_PTR_OFFSET]);
    }
    */
}

#endif /* fs_h */