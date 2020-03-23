#pragma once
#include <vector>
#include <string>
class CipherPool {
private:
	const std::vector<unsigned char> m_cipher;
	size_t cipher_pos = 0;
public:
	CipherPool(const std::string cipher);
	CipherPool(std::vector<unsigned char>cipher);
	char get();
};

std::vector<unsigned char> perform_xor(std::string in, std::vector<unsigned char>& cipher_vec);
std::vector<unsigned char> perform_xor(std::vector<unsigned char> in, std::vector<unsigned char>& cipher_vec);
const std::string vec_to_str(std::vector<unsigned char>vec);
const std::wstring vec_to_wstr(std::vector<unsigned char>vec);